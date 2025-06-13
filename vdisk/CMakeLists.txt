# Copyright (C) 2025  Aleksa Radomirovic
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

link_libraries(modlogc)

easyconfig_option(VDISK_BUILD_SHARED_LIBS "OFF" BOOL "Build vdisk executable non-portably (build libraries shared)")
set(BUILD_SHARED_LIBS "${VDISK_BUILD_SHARED_LIBS}")

easyconfig_add_gtest(vdisk_test)

add_library(
    vdisk_common
        common/src/guid.c
        common/src/logging.c
)
target_include_directories(
    vdisk_common
    PUBLIC
        common/include
        common/test
)

easyconfig_add_gtest(
    vdisk_common_basic
    PARENT vdisk_test
        common/test/VDiskCommonTest.cpp
)
target_link_libraries(
    vdisk_common_basic
    PRIVATE
        vdisk_common
)

link_libraries(vdisk_common)

add_library(
    vdisk_disk
        disk/src/disk.c
        disk/src/io.c
)
target_include_directories(
    vdisk_disk
    PUBLIC
        disk/include
        disk/test
)

easyconfig_add_gtest(
    vdisk_disk_basic
    PARENT vdisk_test
        disk/test/VDiskDiskTest.cpp
)
target_link_libraries(
    vdisk_disk_basic
    PRIVATE
        vdisk_disk
)

add_library(
    vdisk_part
        part/src/gpt/format.c

        part/src/part.c
)
target_include_directories(
    vdisk_part
    PUBLIC
        part/include
        part/test
)
target_link_libraries(
    vdisk_part
    PUBLIC
        vdisk_disk
    PRIVATE
        ZLIB::ZLIB
)

easyconfig_add_gtest(
    vdisk_part_gpt
    PARENT vdisk_test
        part/test/GPTTest.cpp
)
target_link_libraries(
    vdisk_part_gpt
    PRIVATE
        vdisk_part
)

add_executable(
    vdisk
        main/src/main.c
        main/src/part.c
)
target_link_libraries(
    vdisk
    PRIVATE
        modlogc
        vdisk_common
        vdisk_disk
        vdisk_part
)
