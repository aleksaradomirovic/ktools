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

#pragma once

#include "mkdisk/disk.h"

#include <errno.h>
#include <stdckdint.h>

struct disk {
    int file;
    off_t sector_size;
};

#define DISK_DEFAULT_SECTOR_SIZE 512
#define DISK_DEFAULT (struct disk) { .file = -1, .sector_size = DISK_DEFAULT_SECTOR_SIZE }

#define DISKPOS_INVALID ((diskpos_t) { -1, -1 })

[[maybe_unused]]
static diskpos_t pos_from_off(disk_t disk, off_t offset) {
    if(offset < 0) {
        errno = EINVAL;
        return DISKPOS_INVALID;
    }

    return (diskpos_t) { offset / disk->sector_size, offset % disk->sector_size };
}

[[maybe_unused]]
static off_t off_from_pos(disk_t disk, diskpos_t position) {
    if(position.sector < 0) {
        errno = EINVAL;
        return -1;
    }

    off_t offset;
    if(ckd_mul(&offset, position.sector, disk->sector_size)) {
        errno = EOVERFLOW;
        return -1;
    }

    if(ckd_add(&offset, offset, position.offset)) {
        errno = EOVERFLOW;
        return -1;
    }

    if(offset < 0) {
        errno = ERANGE;
        return -1;
    }

    return offset;
}
