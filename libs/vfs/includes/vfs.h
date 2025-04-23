#pragma once
#include <stddef.h>
#include <sys/stat.h>

typedef struct _d_file_t d_file_t;

typedef struct {
    int  (*open)(d_file_t *df, char *path, int flags);
    int  (*read_at)(d_file_t *df, char *buffer, int offset, size_t count);
    int  (*write_at)(d_file_t *df, const char *data, int offset, size_t count);
    int  (*truncate)(d_file_t *df, size_t length);
    int  (*seek)(d_file_t *df, int offset, int whence);

    int  (*mkdir)(d_file_t *df, const char *path);
    int  (*touch)(d_file_t *df, const char *path);
    int  (*unlink)(d_file_t *df, const char *path);
    int  (*stat)(d_file_t *df, const char *path, struct stat *buf);

    int  (*poll)(d_file_t *df, int events);

    void (*flush)(d_file_t *df);
    void (*close)(d_file_t *df);
} file_ops_t;

typedef struct {
    int  (*mount)(char *path);
    int  (*unmount)();
    char (*name)();
} fs_ops_t;

struct _d_file_t {
    file_ops_t *ops;
    size_t      offset;
    void       *data;
};
