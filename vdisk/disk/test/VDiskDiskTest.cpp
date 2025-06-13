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

#include "VDiskDiskTest.hpp"

#include <cerrno>
#include <cstring>

TEST_F(DiskTest, InitFini) {

}

TEST_F(DiskTest, BasicRead) {
    char buf[512];
    ASSERT_EQ(disk_read(disk, (diskpos_t) { 0, 0 }, buf, sizeof(buf)), 0);
}

TEST_F(DiskTest, BasicWrite) {
    char buf[512];
    ASSERT_EQ(disk_write(disk, (diskpos_t) { 0, 0 }, buf, sizeof(buf)), 0);
}

TEST_F(DiskTest, CheckedWrite) {
    char buf[512];
    ASSERT_EQ(disk_read(disk, (diskpos_t) { 0, 0 }, buf, sizeof(buf)), 0);

    char nbuf[512];
    for(size_t i = 0; i < sizeof(nbuf); i+=2) {
        nbuf[i] = ~buf[i];
    }
    ASSERT_EQ(disk_write(disk, (diskpos_t) { 0, 0 }, nbuf, sizeof(nbuf)), 0);

    char cbuf[512];
    ASSERT_EQ(disk_read(disk, (diskpos_t) { 0, 0 }, cbuf, sizeof(cbuf)), 0);

    EXPECT_NE(std::memcmp(buf, cbuf, sizeof(buf)), 0);
    EXPECT_EQ(std::memcmp(nbuf, cbuf, sizeof(buf)), 0);
}

TEST_F(DiskTest, InvalidPosRead) {
    char buf[512];
    ASSERT_NE(disk_read(disk, (diskpos_t) { -1, 0 }, buf, sizeof(buf)), 0);
    EXPECT_EQ(errno, EINVAL);
}

TEST_F(DiskTest, InvalidPosWrite) {
    char buf[512];
    ASSERT_NE(disk_write(disk, (diskpos_t) { -1, 0 }, buf, sizeof(buf)), 0);
    EXPECT_EQ(errno, EINVAL);
}

TEST_F(DiskTest, OutOfBoundsRead) {
    diskpos_t size = disk_size(disk);
    ASSERT_GE(size.sector, 0);

    char buf[512];
    ASSERT_NE(disk_read(disk, size, buf, sizeof(buf)), 0);
    EXPECT_EQ(errno, ENOSPC);
}

TEST_F(DiskTest, OutOfBoundsWrite) {
    diskpos_t size = disk_size(disk);
    ASSERT_GE(size.sector, 0);

    char buf[512];
    ASSERT_NE(disk_write(disk, size, buf, sizeof(buf)), 0);
    EXPECT_EQ(errno, ENOSPC);
}
