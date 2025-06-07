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

# LIBS

if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(BUILD_SHARED_LIBS OFF)
endif()

include_directories(
    include
)

add_library(
    mkdisk-common
#   STATIC
        common/src/logger.c
)
target_include_directories(
    mkdisk-common
    PUBLIC
        common/include
)
target_link_libraries(
    mkdisk-common
    PUBLIC
        logc
)

add_library(
    mkdisk-disk
#   STATIC
        disk/src/disk.c
        disk/src/io.c
)
target_include_directories(
    mkdisk-disk
    PUBLIC
        disk/include
)
target_link_libraries(
    mkdisk-disk
    PUBLIC
        mkdisk-common
)

# TESTS

link_libraries(
    mkdisk-common
)

easytest_add_gtest(
    mkdisk_common
        test/mkdisktest.cpp
)

link_libraries(
    mkdisk-disk
)

easytest_add_gtest(
    mkdisk_disk
        test/disktest.cpp
)
