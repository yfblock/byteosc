#include <vfs.h>
#include <errno.h>

int ext4_fs_open(d_file_t *df, char *pathname, int flags) {
    return 0;
}

static file_ops_t ext4_file_ops = {
    .open = ext4_fs_open
};
static fs_ops_t ext4_fs_ops = {
};