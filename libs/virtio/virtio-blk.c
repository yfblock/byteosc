#include <virtio.h>
#include <console.h>

CTOR void virtio_blk_test() {
    debug("test virtio_blk_test");
    virtio_regs *virtio = 0;
    for(uintptr_t ptr = 0xa000000; ptr < 0xa004000;ptr += 0x200) {
        virtio = (typeof(virtio))ptr;
        if(virtio->MagicValue != VIRTIO_MAGIC_VALUE) continue;
        if(virtio->Version != VIRTIO_VERSION_1) continue;
        if(virtio->DeviceID == 0) continue;
        debug("ptr = %x", ptr);
        debug("virtio magic: %x", virtio->MagicValue);
        debug("virtio version: %x", virtio->Version);
        debug("virtio device id: %x", virtio->DeviceID);
        break;
    }
    if((uintptr_t)virtio >= 0xa004000) return;

    // uint32_t in_features = virtio->DeviceFeatures;
    debug("status before initialize: %d", virtio->Status);
    virtio->Status = 0;
    virtio->Status |= VIRTIO_STATUS_ACK;
    virtio->Status |= VIRTIO_STATUS_DRIVER;
    debug("status after initialize: %d", virtio->Status);

    if(virtio->DeviceID = VIRTIO_DEVICE_ID_BLK) {
        debug("This is a BLK device");
    }
    virtio_blk_config *blk_config = (virtio_blk_config *)virtio->Config;
    debug("virtio blk capacity: %x", blk_config->capacity);
    debug("virtio blk queue max num: %d", virtio->QueueNumMax);
    debug("virtio blk interrupt status: %d", virtio->InterruptStatus);

    
    // virtio->Status
}
