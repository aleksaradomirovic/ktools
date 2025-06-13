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

#include "disk.h"
#include "vdisk.h"

#include <errno.h>
#include <modlogc.h>
#include <stdint.h>
#include <stdckdint.h>

static diskpos_t off_to_diskpos(disk_t disk, off_t offset) {
    if(offset < 0) {
        errno = EINVAL;
        return (diskpos_t) { -1, -1 };
    }

    return (diskpos_t) { offset / disk->sector_size, offset % disk->sector_size };
}

static off_t diskpos_to_off(disk_t disk, diskpos_t pos) {
    if(pos.sector < 0) {
        errno = EINVAL;
        return -1;
    }

    off_t offset;
    if(ckd_mul(&offset, pos.sector, disk->sector_size)) {
        errno = EOVERFLOW;
        return -1;
    }

    if(ckd_add(&offset, offset, pos.offset)) {
        errno = EOVERFLOW;
        return -1;
    }

    if(offset < 0) {
        errno = EINVAL;
        return -1;
    }

    return offset;
}

static off_t disk_size_raw(disk_t disk) {
    off_t end = lseek(disk->fd, 0, SEEK_END);
    if(end < 0) {
        return -1;
    }

    return end;
}

diskpos_t disk_size(disk_t disk) {
    off_t end = disk_size_raw(disk);
    if(end < 0) {
        return (diskpos_t) { -1, -1 };
    }

    diskpos_t pos = off_to_diskpos(disk, end);
    if(pos.sector >= 0) {
        lprintf(log_vdisk, LOG_LEVEL_DEBUG, "queried %i disk size: (%jd sectors, %jd extra bytes)", disk->fd, (intmax_t) pos.sector, (intmax_t) pos.offset);
    }
    return pos;
}

static int disk_check_and_seek(disk_t disk, diskpos_t pos, size_t len) {
    off_t offset = diskpos_to_off(disk, pos);
    if(offset < 0) {
        return -1;
    }

    off_t end = disk_size_raw(disk);
    if(end < 0) {
        return -1;
    }

    off_t op_end;
    if(ckd_add(&op_end, offset, len)) {
        errno = EOVERFLOW;
        return -1;
    }

    if(op_end > end) {
        errno = ENOSPC;
        return -1;
    }

    if(lseek(disk->fd, offset, SEEK_SET) < 0) {
        return -1;
    }

    return 0;
}

int disk_read(disk_t disk, diskpos_t pos, void *buf, size_t len) {
    lprintf(log_vdisk, LOG_LEVEL_DEBUG, "reading %zu bytes from disk %i at (%jd, %jd)", len, disk->fd, (intmax_t) pos.sector, (intmax_t) pos.offset);

    if(disk_check_and_seek(disk, pos, len) != 0) {
        return -1;
    }

    size_t total = 0;
    while(total < len) {
        ssize_t count = read(disk->fd, buf + total, len - total);
        if(count < 0) {
            return -1;
        } else if(count == 0) {
            errno = ENOSPC;
            return -1;
        }

        total += (size_t) count;
    }

    return 0;
}

int disk_write(disk_t disk, diskpos_t pos, const void *buf, size_t len) {
    lprintf(log_vdisk, LOG_LEVEL_DEBUG, "writing %zu bytes to disk %i at (%jd, %jd)", len, disk->fd, (intmax_t) pos.sector, (intmax_t) pos.offset);

    if(disk_check_and_seek(disk, pos, len) != 0) {
        return -1;
    }

    size_t total = 0;
    while(total < len) {
        ssize_t count = write(disk->fd, buf + total, len - total);
        if(count < 0) {
            return -1;
        } else if(count == 0) {
            errno = ENOSPC;
            return -1;
        }

        total += (size_t) count;
    }

    return 0;
}
