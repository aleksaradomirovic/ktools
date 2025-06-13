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

cmake_minimum_required(VERSION 3.28)
project(ktools LANGUAGES C)

include(FetchContent)
FetchContent_Declare(
    EasyConfig
    GIT_REPOSITORY https://github.com/aleksaradomirovic/easyconfig.git
    GIT_TAG        master
    GIT_SHALLOW    ON
)
FetchContent_MakeAvailable(EasyConfig)

easyconfig_project()

set(CMAKE_C_FLAGS "-Wall -Wextra")
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_EXTENSIONS ON)

FetchContent_Declare(
    ModLogC
    GIT_REPOSITORY https://github.com/aleksaradomirovic/modlogc.git
    GIT_TAG        master
    GIT_SHALLOW    ON
    SYSTEM
    EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(ModLogC)

if(EASYCONFIG_BUILD_TESTING)
    enable_language(CXX)

    FetchContent_Declare(
        GTest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        52eb8108c5bdec04579160ae17225d66034bd723
        GIT_SHALLOW    ON
        SYSTEM
        EXCLUDE_FROM_ALL
        FIND_PACKAGE_ARGS
    )
    FetchContent_MakeAvailable(GTest)
endif()

add_subdirectory(vdisk)
