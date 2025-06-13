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

#ifndef _VDISK_PART_H
#define _VDISK_PART_H


#include "vdisk/disk.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef enum {
    PART_TABLE_UNKNOWN = -1,

    PART_TABLE_GPT = 1,
} part_table_type_t;

part_table_type_t disk_identify(disk_t disk);
int disk_format(disk_t disk, part_table_type_t table_type);
int disk_verify(disk_t disk, part_table_type_t table_type);


#ifdef __cplusplus
}
#endif


#endif
