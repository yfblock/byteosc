#include <arch.h>
#include <driver.h>
#include <virtio.h>
#include <console.h>

#define QUEUE_NUM 8

void *virtio_block_probe(virtio_regs *virtio, dtb_node_t *node) {
    debug("This is a BLK device");

    uint32_t in_features = virtio->DeviceFeatures;
    debug("status before initialize: %d", virtio->Status);
    virtio->Status = 0;;
    virtio->Status |= VIRTIO_STATUS_ACK;
    virtio->Status |= VIRTIO_STATUS_DRIVER;

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
    virtio->DeviceFeatures = device_features;

    virtio->Status |= VIRTIO_STATUS_FEATURE_OK;
    if(!(virtio->Status & VIRTIO_STATUS_FEATURE_OK)) {
        panic("VIRTIO-BLK is not initialize with specified features");
    }

    virtio->QueueSel = 0;

    if(virtio->QueueReady)
        panic("virtio disk should not be ready");
    uint32_t max = virtio->QueueNumMax;
    if(max == 0) panic("VIRTIO-BLK queue is empty");
    debug("virtio queue max: %d", max);
    virtio->QueueNum = QUEUE_NUM;

    debug("status after initialize: %d", virtio->Status);

    virtio_blk_config *blk_config = (virtio_blk_config *)virtio->Config;
    debug("virtio blk capacity: %x", blk_config->capacity);
    debug("virtio blk queue max num: %d", virtio->QueueNumMax);
    debug("virtio blk interrupt status: %d", virtio->InterruptStatus);

    return nullptr;
}
