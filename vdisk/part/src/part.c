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

#include "vdisk.h"
#include "vdisk/part.h"

#include <errno.h>
#include <modlogc.h>

#include "gpt/gpt.h"

int disk_format(disk_t disk, part_table_type_t table_type) {
    switch(table_type) {
        case PART_TABLE_GPT:
            return GPT_disk_format(disk);
        default:
            errno = table_type <= 0 ? EINVAL : ENOTSUP;
            return -1;
    }
}

int disk_verify(disk_t disk, part_table_type_t table_type) {
    int result;
    switch(table_type) {
        case PART_TABLE_GPT:
            result = GPT_disk_verify(disk);
            break;
        default:
            errno = table_type <= 0 ? EINVAL : ENOTSUP;
            return -1;
    }

    if(result == 0) {
        lprintf(log_vdisk, LOG_LEVEL_INFO, "disk is valid");
    }

    return result;
}

part_table_type_t disk_identify(disk_t disk) {
    if(GPT_disk_verify(disk) == 0) {
        lprintf(log_vdisk, LOG_LEVEL_DEBUG, "disk is GPT");
        return PART_TABLE_GPT;
    }

    int err = errno;
    lprintf(log_vdisk, LOG_LEVEL_ERROR, "unknown disk type");
    errno = err;
    return PART_TABLE_UNKNOWN;
}
