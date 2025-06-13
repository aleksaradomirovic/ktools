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

#ifndef _VDISK_DISK_H
#define _VDISK_DISK_H


#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef struct disk * disk_t;

typedef struct {
    off_t sector;
    off_t offset;
} diskpos_t;

disk_t disk_open(const char *filename);
int disk_close(disk_t disk);

off_t disk_sector_size(disk_t disk);
diskpos_t disk_size(disk_t disk);

int disk_read(disk_t disk, diskpos_t pos, void *buf, size_t len);
int disk_write(disk_t disk, diskpos_t pos, const void *buf, size_t len);


#ifdef __cplusplus
}
#endif


#endif
