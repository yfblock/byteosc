#include <common.h>
#include <console.h>
#include <ext4.h>
#include <virtio.h>

#define BSIZE        0x1000
#define DEV_BLK_SIZE 0x200
#define BLK_MULTI    BSIZE / DEV_BLK_SIZE

extern virtio_regs *g_virtio;
void virtio_disk_rw(size_t block_no, size_t count, uint8_t *buffer, int write);
static uint8_t _ph_bbuf[(BSIZE)];

static int     file_dev_open(struct ext4_blockdev *bdev) {
    auto blk_cfg      = (virtio_blk_config *)g_virtio->Config;
    bdev->part_offset = 0;
    bdev->part_size   = blk_cfg->capacity * DEV_BLK_SIZE;
    return EOK;
};

static int file_dev_close(struct ext4_blockdev *bdev) {
    return EOK;
}

static int file_dev_bread(struct ext4_blockdev *bdev, void *buf,
                          uint64_t blk_id, uint32_t blk_cnt) {
    virtio_disk_rw(blk_id * BLK_MULTI, blk_cnt * BLK_MULTI, buf, 0);
    return EOK;
}

static int file_dev_bwrite(struct ext4_blockdev *bdev, const void *buf,
                           uint64_t blk_id, uint32_t blk_cnt) {
    debug("bwrite block %d size: %d", blk_id, blk_cnt * BSIZE);
    return EOK;
}

static struct ext4_blockdev_iface bd_iface = {

    // TODO:
    .open = file_dev_open,     .bread = file_dev_bread,
    .bwrite = file_dev_bwrite, .close = file_dev_close,

    .lock = nullptr,           .unlock = nullptr,

    .ph_bsize = BSIZE,         .ph_bcnt = 0,
    .ph_bbuf = _ph_bbuf};
struct ext4_blockdev bd = {
    .bdif = &bd_iface, .part_offset = 0, .part_size = (0) * (BSIZE)};

CTOR void init_lwext4(void) {
    ext4_dmask_set(DEBUG_ALL);

    int r;
    r = ext4_device_register(&bd, "ext4_fs");
    debug("ext4 device register ret: %d", r);
}
