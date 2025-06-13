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

#include "vdisk/disk.h"

#include <stdint.h>

/*
 * UEFI GPT SPEC:
 * https://uefi.org/specs/UEFI/2.10_A/05_GUID_Partition_Table_Format.html
 */

#define GPT_SIGNATURE "EFI PART"
#define GPT_REVISION_1_0 0x00010000

#define GPT_MIN_PARTITION_ENTRY_ARRAY_SIZE 16384

struct __attribute__((packed)) gpt_header {
    char Signature[8];
    uint32_t Revision;

    uint32_t HeaderSize;
    uint32_t HeaderCRC32;

    uint32_t Reserved;
    
    uint64_t MyLBA;
    uint64_t AlternateLBA;

    uint64_t FirstUsableLBA;
    uint64_t LastUsableLBA;

    uint8_t DiskGUID[16];

    uint64_t PartitionEntryLBA;
    uint32_t NumberOfPartitionEntries;
    uint32_t SizeOfPartitionEntry;
    uint32_t PartitionEntryArrayCRC32;
};

_Static_assert(sizeof(struct gpt_header) == 92);

struct __attribute__((packed)) gpt_partition_entry {
    uint8_t PartitionTypeGUID[16];
    uint8_t UniquePartitionGUID[16];

    uint64_t StartingLBA;
    uint64_t EndingLBA;

    uint64_t Attributes;

    char PartitionName[72];
};

_Static_assert(sizeof(struct gpt_partition_entry) == 128);

int GPT_disk_format(disk_t disk);
int GPT_disk_verify(disk_t disk);

int GPT_disk_update(disk_t disk);
