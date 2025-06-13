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
#include <fcntl.h>
#include <modlogc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

disk_t disk_open(const char *filename) {
    lprintf(log_vdisk, LOG_LEVEL_DEBUG, "opening disk '%s'", filename);

    disk_t disk = malloc(sizeof(struct disk));
    if(disk == NULL) {
        return NULL;
    }

    disk->sector_size = 512;

    disk->fd = open(filename, O_RDWR, 00666);
    if(disk->fd < 0) {
        int err = errno;
        lprintf(log_vdisk, LOG_LEVEL_ERROR, "failed to open disk '%s'", filename);
        free(disk);
        errno = err;
        return NULL;
    }

    lprintf(log_vdisk, LOG_LEVEL_DEBUG, "disk '%s' has fd %i", filename, disk->fd);
    return disk;
}

int disk_close(disk_t disk) {
    lprintf(log_vdisk, LOG_LEVEL_DEBUG, "closing disk with fd '%i'", disk->fd);

    if(close(disk->fd) != 0) {
        return -1;
    }

    free(disk);
    return 0;
}

off_t disk_sector_size(disk_t disk) {
    return disk->sector_size;
}
