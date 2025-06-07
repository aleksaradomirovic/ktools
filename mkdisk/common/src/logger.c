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

#include "logger.h"
#include "mkdisk.h"

logger_t mkdisklog = NULL;

int mkdisk_enable_log(log_level_t level) {
    if(mkdisklog == NULL) {
        mkdisklog = log_create(level, stderr, 0);
        if(mkdisklog == NULL) {
            return -1;
        }
    } else {
        if(log_set_level(mkdisklog, level) != level) {
            return -1;
        }
    }

    return 0;
}

int mkdisk_disable_log() {
    if(mkdisklog == NULL) {
        errno = EFAULT;
        return -1;
    }

    if(log_destroy(mkdisklog) != 0) {
        return -1;
    }

    return 0;
}

__attribute__((destructor))
void fini_mkdisklog() {
    mkdisk_disable_log();
}
