#include <console.h>
#include <driver.h>
#include <virtio.h>

void virtio_init(dtb_node_t *node) {
    dtb_prop_t *reg_prop = dtb_find_prop(node, "reg");
    auto virtio = (virtio_regs *)dtb_read_prop_cell(reg_prop->first_cell, 2);
    if(virtio->MagicValue != VIRTIO_MAGIC_VALUE) return;
    if(virtio->Version != VIRTIO_VERSION_1) return;
    if(virtio->DeviceID == 0) return;

    debug("virtio address = %x", virtio);
    debug("virtio magic: %x", virtio->MagicValue);
    debug("virtio version: %x", virtio->Version);
    debug("virtio device id: %x", virtio->DeviceID);

    uint32_t in_features = virtio->DeviceFeatures;
    debug("status before initialize: %d", virtio->Status);
    virtio->Status = 0;
    virtio->Status |= VIRTIO_STATUS_ACK;
    virtio->Status |= VIRTIO_STATUS_DRIVER;
    debug("status after initialize: %d", virtio->Status);

    if(virtio->DeviceID == VIRTIO_DEVICE_ID_BLK) {
        debug("This is a BLK device");
    }
    virtio_blk_config *blk_config = (virtio_blk_config *)virtio->Config;
    debug("virtio blk capacity: %x", blk_config->capacity);
    debug("virtio blk queue max num: %d", virtio->QueueNumMax);
    debug("virtio blk interrupt status: %d", virtio->InterruptStatus);
}

static struct udevice_id virtio_ids[] = {
    {.compatible =  "virtio,mmio"},
    {}
};

DEFINE_DRIVER virtio_driver = {
    .ids = virtio_ids,
    .probe = virtio_init
};
