/*
 * Copyright (C) 2025  Aleksa Radomirovic
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "diskimpl.h"
#include "logger.h"

#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>

static disk_t disk_init() {
    struct disk *disk = malloc(sizeof(struct disk));
    if(disk == NULL) {
        return NULL;
    }

    *disk = DISK_DEFAULT;

    return disk;
}

disk_t disk_open(const char *file) {
    lprintf(mkdisklog, LOG_LEVEL_TRACE, "%s", __FUNCTION__);
    lprintf(mkdisklog, LOG_LEVEL_DEBUG, "opening disk image from file '%s'", file);

    disk_t disk = disk_init();
    if(disk == NULL) {
        return NULL;
    }

    int fd = open(file, O_RDWR, 00666);
    if(fd < 0) {
        int err = errno;
        lprintf(mkdisklog, LOG_LEVEL_ERROR, "failed to open disk image file '%s': %s", file, strerrno);
        errno = err;
        free(disk);
        return NULL;
    }
    disk->file = fd;

    return disk;
}

disk_t disk_openfrom(int file) {
    lprintf(mkdisklog, LOG_LEVEL_TRACE, "%s", __FUNCTION__);
    lprintf(mkdisklog, LOG_LEVEL_DEBUG, "opening disk image from file descriptor %d", file);

    int flags = fcntl(file, F_GETFL);
    if(flags == -1) {
        return NULL;
    }

    if((flags & O_ACCMODE) != O_RDWR) {
        lprintf(mkdisklog, LOG_LEVEL_ERROR, "disk image file descriptor is not read-write");
        errno = EBADF;
        return NULL;
    }

    disk_t disk = disk_init();
    if(disk == NULL) {
        return NULL;
    }

    int fd = dup(file);
    if(fd < 0) {
        free(disk);
        return NULL;
    }

    disk->file = fd;
    return disk;
}

int disk_close(disk_t disk) {
    lprintf(mkdisklog, LOG_LEVEL_TRACE, "%s", __FUNCTION__);
    if(disk->file != -1) {
        if(close(disk->file) != 0) {
            return -1;
        }
    }

    free(disk);
    return 0;
}

disk_t disk_create(const char *file, diskpos_t size) {
    lprintf(mkdisklog, LOG_LEVEL_TRACE, "%s", __FUNCTION__);
    lprintf(mkdisklog, LOG_LEVEL_DEBUG, "creating disk image file '%s' of size %jd%+jdb", file, (intmax_t) size.sector, (intmax_t) size.offset);

    off_t abs_size;
    {
        struct disk dummy = DISK_DEFAULT;
        abs_size = off_from_pos(&dummy, size);
        if(abs_size < 0) {
            return NULL;
        }
    }

    disk_t disk = disk_init();
    if(disk == NULL) {
        return NULL;
    }

    int fd = open(file, O_RDWR | O_CREAT | O_EXCL, 00666);
    if(fd < 0) {
        free(disk);
        return NULL;
    }

    if(ftruncate(fd, abs_size) != 0) {
        int err = errno;
        close(fd);
        unlink(file);
        errno = err;
        free(disk);
        return NULL;
    }

    disk->file = fd;
    return disk;
}
