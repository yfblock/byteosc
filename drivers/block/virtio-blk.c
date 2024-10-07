#include <arch.h>
#include <driver.h>
#include <virtio.h>
#include <console.h>

void *virtio_block_probe(virtio_regs *virtio, dtb_node_t *node) {
    debug("This is a BLK device");

    uint32_t in_features = virtio->DeviceFeatures;
    debug("status before initialize: %d", virtio->Status);
    virtio->Status = 0;
    nop();
    virtio->Status |= VIRTIO_STATUS_ACK;
    nop();
    virtio->Status |= VIRTIO_STATUS_DRIVER;
    nop();
    debug("status after initialize: %d", virtio->Status);

    virtio_blk_config *blk_config = (virtio_blk_config *)virtio->Config;
    debug("virtio blk capacity: %x", blk_config->capacity);
    debug("virtio blk queue max num: %d", virtio->QueueNumMax);
    debug("virtio blk interrupt status: %d", virtio->InterruptStatus);

    return nullptr;
}
