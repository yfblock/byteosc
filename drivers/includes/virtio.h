#pragma once
#include <common.h>
#include <smoldtb.h>

#define QUEUE_NUM                   8

#define VIRTIO_MAGIC_VALUE          0x74726976
#define VIRTIO_VERSION_1            0x1
#define VIRTIO_VERSION_2            0x2

#define VIRTIO_STATUS_ACK           1
#define VIRTIO_STATUS_DRIVER        2
#define VIRTIO_STATUS_FAILED        128
#define VIRTIO_STATUS_FEATURE_OK    8
#define VIRTIO_STATUS_DRIVER_OK     4
#define VIRTIO_STATUS_NEED_RESET    64

/* https://docs.oasis-open.org/virtio/virtio/v1.1/csprd01/virtio-v1.1-csprd01.html#x1-1930005
 */
#define VIRTIO_DEVICE_ID_NET        1
#define VIRTIO_DEVICE_ID_BLK        2
#define VIRTIO_DEVICE_ID_CONSOLE    3

// device feature bits
#define VIRTIO_BLK_F_RO             5  /* Disk is read-only */
#define VIRTIO_BLK_F_SCSI           7  /* Supports scsi command passthru */
#define VIRTIO_BLK_F_CONFIG_WCE     11 /* Writeback mode available in config */
#define VIRTIO_BLK_F_MQ             12 /* support more than one vq */
#define VIRTIO_F_ANY_LAYOUT         27
#define VIRTIO_RING_F_INDIRECT_DESC 28
#define VIRTIO_RING_F_EVENT_IDX     29

typedef volatile struct {
    uint32_t MagicValue;
    uint32_t Version;
    uint32_t DeviceID;
    uint32_t VendorID;
    uint32_t DeviceFeatures;
    uint32_t DeviceFeaturesSel;
    uint32_t _reserved0[2];
    uint32_t DriverFeatures;
    uint32_t DriverFeaturesSel;
    uint32_t _reserved1[2];
    uint32_t QueueSel;
    uint32_t QueueNumMax;
    uint32_t QueueNum;
    uint32_t _reserved2[2];
    uint32_t QueueReady;
    uint32_t _reserved3[2];
    uint32_t QueueNotify;
    uint32_t _reserved4[3];
    uint32_t InterruptStatus;
    uint32_t InterruptACK;
    uint32_t _reserved5[2];
    uint32_t Status;
    uint32_t _reserved6[3];
    uint32_t QueueDescLow;
    uint32_t QueueDescHigh;
    uint32_t _reserved7[2];
    uint32_t QueueAvailLow;
    uint32_t QueueAvailHigh;
    uint32_t _reserved8[2];
    uint32_t QueueUsedLow;
    uint32_t QueueUsedHigh;
    uint32_t _reserved9[21];
    uint32_t ConfigGeneration;
    uint32_t Config[0];
} virtio_regs;

typedef struct {
    uint64_t capacity;
    uint32_t size_max;
    uint32_t seg_max;
    struct {
        uint16_t cylinders;
        uint8_t  heads;
        uint8_t  sectors;
    } geometry;
    uint32_t blk_size;
    struct {
        uint8_t  physical_block_exp;
        uint8_t  alignment_offset;
        uint16_t min_io_size;
        uint32_t opt_io_size;
    } topology;
    uint8_t writeback;
} virtio_blk_config;

typedef struct {
    uint32_t type;
    uint32_t reserved;
    uint64_t sector;
    uint8_t  data[512];
    uint8_t  status;
} virtio_blk_req;

struct virtqueue {
    /* Physical base address of the full data structure. */
    uint32_t                         phys;
    uint32_t                         len;
    uint32_t                         seen_used;
    uint32_t                         free_desc;

    volatile struct virtqueue_desc  *desc;
    volatile struct virtqueue_avail *avail;
    volatile uint16_t               *used_event;
    volatile struct virtqueue_used  *used;
    volatile uint16_t               *avail_event;
    void                           **desc_virt;
};

// a single descriptor, from the spec.
struct virtq_desc {
    uint64_t addr;
    uint32_t len;
    uint16_t flags;
    uint16_t next;
};

#define VRING_DESC_F_NEXT  1 // chained with another descriptor
#define VRING_DESC_F_WRITE 2 // device writes (vs read)

// the (entire) avail ring, from the spec.
struct virtq_avail {
    uint16_t flags;           // always zero
    uint16_t idx;             // driver will write ring[idx] next
    uint16_t ring[QUEUE_NUM]; // descriptor numbers of chain heads
    uint16_t unused;
};

// one entry in the "used" ring, with which the
// device tells the driver about completed requests.
struct virtq_used_elem {
    uint32_t id; // index of start of completed descriptor chain
    uint32_t len;
};

struct virtq_used {
    uint16_t               flags; // always zero
    uint16_t               idx; // device increments when it adds a ring[] entry
    struct virtq_used_elem ring[QUEUE_NUM];
};

// these are specific to virtio block devices, e.g. disks,
// described in Section 5.2 of the spec.

#define VIRTIO_BLK_T_IN  0 // read the disk
#define VIRTIO_BLK_T_OUT 1 // write the disk

// the format of the first descriptor in a disk request.
// to be followed by two more descriptors containing
// the block, and a one-byte status.
struct virtio_blk_req {
    uint32_t type; // VIRTIO_BLK_T_IN or ..._OUT
    uint32_t reserved;
    uint64_t sector;
};

void *virtio_block_probe(virtio_regs *virtio, dtb_node_t *node);
