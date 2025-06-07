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

#include "disktest.hpp"

#include <cerrno>
#include <filesystem>

TEST(DiskInitTest, Create) {
    disk_t disk;
    ASSERT_NE(disk = disk_create("DiskInitTest_Create.img", (diskpos_t) { 200, 0 }), nullptr);

    diskpos_t size = disk_size(disk);
    ASSERT_EQ(size.sector, 200);
    ASSERT_EQ(size.offset, 0);

    ASSERT_EQ(disk_close(disk), 0);

    std::filesystem::remove("DiskInitTest_Create.img");
}

TEST(DiskInitTest, CreateAndOpen) {
    disk_t disk;
    ASSERT_NE(disk = disk_create("DiskInitTest_CreateAndOpen.img", (diskpos_t) { 200, 0 }), nullptr);

    diskpos_t size = disk_size(disk);
    ASSERT_EQ(size.sector, 200);
    ASSERT_EQ(size.offset, 0);

    ASSERT_EQ(disk_close(disk), 0);


    ASSERT_NE(disk = disk_open("DiskInitTest_CreateAndOpen.img"), nullptr);

    size = disk_size(disk);
    ASSERT_EQ(size.sector, 200);
    ASSERT_EQ(size.offset, 0);

    ASSERT_EQ(disk_close(disk), 0);

    std::filesystem::remove("DiskInitTest_CreateAndOpen.img");
}

TEST(DiskInitTest, OpenFrom) {
    FILE *tmp;
    ASSERT_NE(tmp = tmpfile(), nullptr);

    int fd;
    ASSERT_GE(fd = fileno(tmp), 0);

    disk_t disk;
    ASSERT_NE(disk = disk_openfrom(fd), nullptr);
    ASSERT_EQ(fclose(tmp), 0);

    ASSERT_EQ(disk_close(disk), 0);
}

TEST_F(DiskTest, Read) {
    char buf[512];
    ASSERT_EQ(disk_read(disk, (diskpos_t) { 0, 0 }, buf, sizeof(buf)), 0);
}

TEST_F(DiskTest, Write) {
    char buf[] = "TEST STR";
    ASSERT_EQ(disk_write(disk, (diskpos_t) { 0, 0 }, buf, sizeof(buf)), 0);

    char rbuf[sizeof(buf)];
    ASSERT_EQ(disk_read(disk, (diskpos_t) { 0, 0 }, rbuf, sizeof(buf)), 0);

    EXPECT_EQ(memcmp(buf, rbuf, sizeof(buf)), 0);
}

TEST_F(DiskTest, ReadInvalid) {
    char buf[8192];
    ASSERT_NE(disk_read(disk, (diskpos_t) { -1, 0 }, buf, sizeof(buf)), 0);

    EXPECT_EQ(errno, EINVAL);
}

TEST_F(DiskTest, WriteInvalid) {
    char buf[8192];
    ASSERT_NE(disk_write(disk, (diskpos_t) { -1, 0 }, buf, sizeof(buf)), 0);

    EXPECT_EQ(errno, EINVAL);
}

TEST_F(DiskTest, ReadOutOfBounds) {
    diskpos_t size = disk_size(disk);
    ASSERT_GE(size.sector, 0);

    char buf[8192];
    ASSERT_NE(disk_read(disk, size, buf, sizeof(buf)), 0);

    EXPECT_EQ(errno, ENOSPC);
}

TEST_F(DiskTest, WriteOutOfBounds) {
    diskpos_t size = disk_size(disk);
    ASSERT_GE(size.sector, 0);

    char buf[8192];
    ASSERT_NE(disk_write(disk, size, buf, sizeof(buf)), 0);

    EXPECT_EQ(errno, ENOSPC);
}
