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

#include <fcntl.h>
#include <unistd.h>

int generate_guid(uint8_t guid[16]) {
    int rng = open("/dev/urandom", O_RDONLY);
    if(rng < 0) {
        return -1;
    }

    size_t total = 0;
    while(total < 16) {
        ssize_t rlen = read(rng, guid + total, 16 - total);
        if(rlen < 0) {
            int err = errno;
            close(rng);
            errno = err;
            return -1;
        } else if(rlen == 0) {
            close(rng);
            errno = EIO;
            return -1;
        }

        total += (size_t) rlen;
    }

    if(close(rng) != 0) {
        return -1;
    }
    return 0;
}
