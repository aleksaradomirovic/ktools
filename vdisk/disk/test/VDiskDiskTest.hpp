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

#include <gtest/gtest.h>

#include <cstdlib>
#include <filesystem>
#include <unistd.h>

#include "vdisk/disk.h"

#include "VDiskCommonTest.hpp"

class DiskTest : public testing::Test {
  protected:
    std::filesystem::path tmpfile;

    disk_t disk;

    void SetUp() override {
        char tmpname[] = "DiskTest.XXXXXX";

        int fd = mkstemp(tmpname);
        ASSERT_NE(fd, -1);
        ASSERT_EQ(close(fd), 0);

        tmpfile = std::filesystem::absolute(tmpname);
        std::filesystem::resize_file(tmpfile, 64 * 1024 * 1024);

        disk = disk_open(tmpfile.c_str());
        ASSERT_NE(disk, nullptr);
    }

    void TearDown() override {
        ASSERT_EQ(disk_close(disk), 0);
        ASSERT_TRUE(std::filesystem::remove(tmpfile));
    }
};
