#include <arch.h>
#include <buddy_alloc.h>
#include <console.h>
#include <driver.h>
#include <virtio.h>

static struct disk {
    // a set (not a ring) of DMA descriptors, with which the
    // driver tells the device where to read and write individual
    // disk operations. there are NUM descriptors.
    // most commands consist of a "chain" (a linked list) of a couple of
    // these descriptors.
    struct virtq_desc *desc;

    // a ring in which the driver writes descriptor numbers
    // that the driver would like the device to process.  it only
    // includes the head descriptor of each chain. the ring has
    // NUM elements.
    struct virtq_avail *avail;

    // a ring in which the device writes descriptor numbers that
    // the device has finished processing (just the head of each chain).
    // there are NUM used ring entries.
    struct virtq_used *used;

    // our own book-keeping.
    char free[QUEUE_NUM]; // is a descriptor free?
    uint16_t used_idx;    // we've looked this far in used[2..NUM].

    // track info about in-flight operations,
    // for use when completion interrupt arrives.
    // indexed by first descriptor index of chain.
    struct {
        size_t block_no;
        uint8_t *buffer;
        char status;
    } info[QUEUE_NUM];

    // disk command headers.
    // one-for-one with descriptors, for convenience.
    virtio_blk_req ops[QUEUE_NUM];

    virtio_regs *virtio;
} disk;

virtio_regs *g_virtio;

void hexdump(uint8_t *buffer, size_t len) {
    if(buffer == nullptr)
        return;
    constexpr size_t LINE_WIDTH = 16;
    for(size_t i = 0; i < len; i += LINE_WIDTH) {
        if(i % LINE_WIDTH == 0) {
            if(i < 0x10)
                printf("0x000%lx | ", i);
            else if(i < 0x100)
                printf("0x00%lx | ", i);
            else if(i < 0x1000)
                printf("0x0%lx | ", i);
            else
                printf("0x%lx | ", i);
        }
        for(int j = 0; j < LINE_WIDTH; j++) {
            if(buffer[i + j] == 0)
                printf("00 ");
            else if(buffer[i + j] < 0x10)
                printf("0%x ", buffer[i + j]);
            else
                printf("%x ", buffer[i + j]);

            if(j % 8 == 7)
                printf("  ");
        }

        printf("|");
        for(int j = 0; j < LINE_WIDTH; j++) {
            char c = buffer[i + j];
            if(c > 0x1f && c < 0x7f)
                printf("%c", c);
            else
                printf(".");
        }
        printf("|");

        printf("\n");
    }
}

void *virtio_block_probe(virtio_regs *virtio, dtb_node_t *node) {
    debug("This is a BLK device");

    debug("status before initialize: %d", virtio->Status);
    uint32_t status = 0;

    virtio->Status = status;

    status |= VIRTIO_STATUS_ACK;
    virtio->Status = status;

    status |= VIRTIO_STATUS_DRIVER;
    virtio->Status = status;

    // Device initialize
    // https://docs.oasis-open.org/virtio/virtio/v1.1/csprd01/virtio-v1.1-csprd01.html#x1-920001
    uint32_t device_features = virtio->DeviceFeatures;
    device_features &= ~(1 << VIRTIO_BLK_F_RO);
    device_features &= ~(1 << VIRTIO_BLK_F_SCSI);
    device_features &= ~(1 << VIRTIO_BLK_F_CONFIG_WCE);
    device_features &= ~(1 << VIRTIO_BLK_F_MQ);
    device_features &= ~(1 << VIRTIO_F_ANY_LAYOUT);
    device_features &= ~(1 << VIRTIO_RING_F_EVENT_IDX);
    device_features &= ~(1 << VIRTIO_RING_F_INDIRECT_DESC);
    virtio->DriverFeatures = device_features;

    status |= VIRTIO_STATUS_FEATURE_OK;
    virtio->Status = status;

    status = virtio->Status;
    if(!(status & VIRTIO_STATUS_FEATURE_OK)) {
        panic("VIRTIO-BLK is not initialize with specified features");
    }

    virtio->QueueSel = 0;

    if(virtio->QueueReady)
        panic("virtio disk should not be ready");
    uint32_t max = virtio->QueueNumMax;
    if(max == 0)
        panic("VIRTIO-BLK queue is empty");
    debug("virtio queue max: %d", max);

    disk.desc = kalloc(1);
    disk.avail = kalloc(1);
    disk.used = kalloc(1);

    if(!(disk.desc && disk.avail && disk.used))
        panic("virtio disk alloc error");
    virtio->QueueNum = QUEUE_NUM;

    // write physical addresses.
    virtio->QueueDescLow = (uint64_t)disk.desc;
    virtio->QueueDescHigh = (uint64_t)disk.desc >> 32;
    virtio->QueueAvailLow = (uint64_t)disk.avail;
    virtio->QueueAvailHigh = (uint64_t)disk.avail >> 32;
    virtio->QueueUsedLow = (uint64_t)disk.used;
    virtio->QueueUsedHigh = (uint64_t)disk.used >> 32;

    // Queue is Ready
    virtio->QueueReady = 1;
    assert(virtio->QueueReady == 1);

    for(int i = 0; i < QUEUE_NUM; i++)
        disk.free[i] = 1;

    status |= VIRTIO_STATUS_DRIVER_OK;
    virtio->Status = status;

    disk.virtio = virtio;
    g_virtio = virtio;

    debug("status after initialize: %d", virtio->Status);

    virtio_blk_config *blk_config = (virtio_blk_config *)virtio->Config;
    debug("virtio blk capacity: %x", blk_config->capacity);
    debug("virtio blk queue max num: %d", virtio->QueueNumMax);
    debug("virtio blk interrupt status: %d", virtio->InterruptStatus);

    uint8_t *buffer = calloc(1024, sizeof(uint8_t));

    void virtio_disk_rw(size_t, size_t, uint8_t *, int);
    virtio_disk_rw(0, 1, buffer, 0);

    return nullptr;
}

// find a free descriptor, mark it non-free, return its index.
static int alloc_desc() {
    for(int i = 0; i < QUEUE_NUM; i++) {
        if(disk.free[i]) {
            disk.free[i] = 0;
            return i;
        }
    }
    return -1;
}

// mark a descriptor as free.
static void free_desc(int i) {
    if(i >= QUEUE_NUM)
        panic("free_desc 1");
    if(disk.free[i])
        panic("free_desc 2");
    disk.desc[i].addr = 0;
    disk.desc[i].len = 0;
    disk.desc[i].flags = 0;
    disk.desc[i].next = 0;
    disk.free[i] = 1;
    // TODO: Wake up Queue
}

// free a chain of descriptors.
static void free_chain(int i) {
    while(1) {
        int flag = disk.desc[i].flags;
        int nxt = disk.desc[i].next;
        free_desc(i);
        if(flag & VRING_DESC_F_NEXT)
            i = nxt;
        else
            break;
    }
}

// allocate three descriptors (they need not be contiguous).
// disk transfers always use three descriptors.
static int alloc3_desc(int *idx) {
    for(int i = 0; i < 3; i++) {
        idx[i] = alloc_desc();
        if(idx[i] < 0) {
            for(int j = 0; j < i; j++)
                free_desc(idx[j]);
            return -1;
        }
    }
    return 0;
}

void virtio_disk_rw(size_t block_no, size_t count, uint8_t *buffer, int write) {
    // the spec's Section 5.2 says that legacy block operations use
    // three descriptors: one for type/reserved/sector, one for the
    // data, one for a 1-byte status result.

    // allocate the three descriptors.
    int idx[3];
    while(1) {
        if(alloc3_desc(idx) == 0) {
            break;
        }
        // TODO: Sleep to wait empty queue
    }

    // format the three descriptors.
    // qemu's virtio-blk.c reads them.

    virtio_blk_req *buf0 = &disk.ops[idx[0]];
    if(write)
        buf0->type = VIRTIO_BLK_T_OUT; // write the disk
    else
        buf0->type = VIRTIO_BLK_T_IN; // read the disk
    buf0->reserved = 0;
    buf0->sector = block_no;

    disk.desc[idx[0]].addr = (uint64_t)buf0;
    disk.desc[idx[0]].len = sizeof(virtio_blk_req);
    disk.desc[idx[0]].flags = VRING_DESC_F_NEXT;
    disk.desc[idx[0]].next = idx[1];

    disk.desc[idx[1]].addr = (uint64_t)buffer;
    disk.desc[idx[1]].len = count * 512;

    if(write)
        disk.desc[idx[1]].flags = 0; // device reads b->data
    else
        disk.desc[idx[1]].flags = VRING_DESC_F_WRITE; // device writes b->data
    disk.desc[idx[1]].flags |= VRING_DESC_F_NEXT;
    disk.desc[idx[1]].next = idx[2];

    disk.info[idx[0]].status = 0xff; // device writes 0 on success
    disk.desc[idx[2]].addr = (uint64_t)&disk.info[idx[0]].status;
    disk.desc[idx[2]].len = 1;
    disk.desc[idx[2]].flags = VRING_DESC_F_WRITE; // device writes the status
    disk.desc[idx[2]].next = 0;

    // record struct buf for virtio_disk_intr().
    disk.info[idx[0]].block_no = block_no;
    disk.info[idx[0]].buffer = buffer;

    // tell the device the first index in our chain of descriptors.
    disk.avail->ring[disk.avail->idx % QUEUE_NUM] = idx[0];

    __sync_synchronize();

    // tell the device another avail ring entry is available.
    disk.avail->idx += 1; // not % NUM ...

    __sync_synchronize();

    disk.virtio->QueueNotify = 0;

    // Wait for virtio_disk_intr() to say request has finished.
    // while(b->disk == 1) {
    // // TODO: Sleep to wait empty Queue
    // }

    while(disk.info[idx[0]].status == 0xff) {
        nop();
    }
    disk.virtio->InterruptACK = 1;

    disk.info[idx[0]].block_no = 0;
    disk.info[idx[0]].buffer = nullptr;
    free_chain(idx[0]);
}
