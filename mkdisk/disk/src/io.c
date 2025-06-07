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

#include <errno.h>
#include <limits.h>

static off_t disk_size_raw(disk_t disk) {
    return lseek(disk->file, 0, SEEK_END);
}

diskpos_t disk_size(disk_t disk) {
    off_t size = disk_size_raw(disk);
    if(size < 0) {
        return DISKPOS_INVALID;
    }

    return pos_from_off(disk, size);
}

static int disk_check_and_seek(disk_t disk, diskpos_t pos, size_t len) {
    off_t size = disk_size_raw(disk);
    if(size < 0) {
        return -1;
    }

    off_t off = off_from_pos(disk, pos);
    if(off < 0) {
        return -1;
    }

    off_t end;
    if(ckd_add(&end, off, len)) {
        errno = EOVERFLOW;
        return -1;
    }

    if(end > size) {
        errno = ENOSPC;
        return -1;
    }

    if(lseek(disk->file, off, SEEK_SET) < 0) {
        return -1;
    }

    return 0;
}

int disk_read(disk_t disk, diskpos_t pos, void *buf, size_t len) {
    if(disk_check_and_seek(disk, pos, len) != 0) {
        return -1;
    }

    size_t total = 0;
    while(total < len) {
        ssize_t dlen = (len - total) <= SSIZE_MAX ? (len - total) : SSIZE_MAX;
        dlen = read(disk->file, buf, dlen);
        if(dlen < 0) {
            return -1;
        } else if(dlen == 0) {
            errno = ENOSPC;
            return -1;
        }

        total += (size_t) dlen;
    }

    return 0;
}

int disk_write(disk_t disk, diskpos_t pos, const void *buf, size_t len) {
    if(disk_check_and_seek(disk, pos, len) != 0) {
        return -1;
    }

    size_t total = 0;
    while(total < len) {
        ssize_t dlen = (len - total) <= SSIZE_MAX ? (len - total) : SSIZE_MAX;
        dlen = write(disk->file, buf, dlen);
        if(dlen < 0) {
            return -1;
        } else if(dlen == 0) {
            errno = ENOSPC;
            return -1;
        }

        total += (size_t) dlen;
    }

    return 0;
}
