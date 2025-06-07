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

#include "mkdisktest.hpp"

#include "mkdisk/disk.h"

#include <cstdio>

class DiskTest : public MkDiskTest {
  protected:
    disk_t disk;

    void SetUp() override {
        FILE *tmp;
        ASSERT_NE(tmp = tmpfile(), nullptr);

        int fd;
        ASSERT_GE(fd = fileno(tmp), 0);
        
        ASSERT_EQ(ftruncate(fd, 64 * 1024 * 1024), 0);

        ASSERT_NE(disk = disk_openfrom(fd), nullptr);
        ASSERT_EQ(fclose(tmp), 0);
    }

    void TearDown() override {
        ASSERT_EQ(disk_close(disk), 0);
    }
};
