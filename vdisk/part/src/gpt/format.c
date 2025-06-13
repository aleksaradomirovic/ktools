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

#include "gpt.h"
#include "vdisk.h"

#include <errno.h>
#include <fcntl.h>
#include <modlogc.h>
#include <stdint.h>
#include <zlib.h>

static int gpt_update_header(disk_t disk, diskpos_t pos) {
    struct gpt_header header;
    if(disk_read(disk, pos, &header, sizeof(header)) != 0) {
        return -1;
    }

    if(header.MyLBA != (uint64_t) pos.sector) {
        lprintf(log_vdisk, LOG_LEVEL_ERROR, "Header at sector %jd specifies its own LBA incorrectly!", (intmax_t) pos.sector);
        errno = EINVAL;
        return -1;
    }

    unsigned char partitions[header.NumberOfPartitionEntries * header.SizeOfPartitionEntry];
    if(disk_read(disk, (diskpos_t) { header.PartitionEntryLBA, 0 }, partitions, sizeof(partitions)) != 0) {
        return -1;
    }

    uint32_t partition_crc32 = crc32(0, NULL, 0);
    partition_crc32 = crc32(partition_crc32, (const void *) partitions, sizeof(partitions));

    header.PartitionEntryArrayCRC32 = partition_crc32;

    header.HeaderCRC32 = 0;
    
    uint32_t header_crc32 = crc32(0, NULL, 0);
    header_crc32 = crc32(header_crc32, (const void *) &header, sizeof(header));
    
    header.HeaderCRC32 = header_crc32;

    if(disk_write(disk, (diskpos_t) { header.MyLBA, 0 }, &header, sizeof(header)) != 0) {
        return -1;
    }

    if(pos.sector == 1) {
        struct gpt_header alt_header = header;
        alt_header.MyLBA = header.AlternateLBA;
        alt_header.AlternateLBA = header.MyLBA;
        alt_header.PartitionEntryLBA = alt_header.MyLBA + 1;

        if(disk_write(disk, (diskpos_t) { alt_header.MyLBA, 0 }, &alt_header, sizeof(alt_header)) != 0) {
            return -1;
        }

        if(disk_write(disk, (diskpos_t) { alt_header.PartitionEntryLBA, 0 }, partitions, sizeof(partitions)) != 0) {
            return -1;
        }

        if(gpt_update_header(disk, (diskpos_t) { alt_header.MyLBA, 0 }) != 0) {
            return -1;
        }
    }

    return 0;
}

int GPT_disk_update(disk_t disk) {
    if(gpt_update_header(disk, (diskpos_t) { 1, 0 }) != 0) {
        return -1;
    }

    return 0;
}

int GPT_disk_format(disk_t disk) {
    lprintf(log_vdisk, LOG_LEVEL_DEBUG, "partitioning disk as GPT");

    off_t sector_size = disk_sector_size(disk);
    if(sector_size <= 0) {
        return -1;
    }

    diskpos_t total_size = disk_size(disk);
    if(total_size.sector < 0) {
        return -1;
    }

    off_t partition_array_sectors = 1 + ((GPT_MIN_PARTITION_ENTRY_ARRAY_SIZE - 1) / sector_size); // ceildiv
    off_t gpt_total_sectors = partition_array_sectors + 1;
    off_t gpt_disk_min_sectors = (2 * gpt_total_sectors) + 1 + 1; // boot record and at least one free sector

    if(total_size.sector < gpt_disk_min_sectors) {
        lprintf(log_vdisk, LOG_LEVEL_ERROR, "disk not large enough for GPT: %jd sectors available, %jd needed", (intmax_t) (total_size.sector), (intmax_t) (gpt_disk_min_sectors));
        errno = ENOSPC;
        return -1;
    }

    lprintf(log_vdisk, LOG_LEVEL_DEBUG, "space available for partitions: %jd sectors", (intmax_t) (total_size.sector - gpt_disk_min_sectors + 1));

    struct gpt_header header = {
        .Signature = GPT_SIGNATURE,
        .Revision  = GPT_REVISION_1_0,

        .HeaderSize = sizeof(struct gpt_header),

        .MyLBA        = 1,
        .AlternateLBA = total_size.sector - gpt_total_sectors,

        .FirstUsableLBA = 1 + gpt_total_sectors,
        .LastUsableLBA  = total_size.sector - gpt_total_sectors - 1,

        .PartitionEntryLBA        = 1 + 1,
        .NumberOfPartitionEntries = 0,
        .SizeOfPartitionEntry     = sizeof(struct gpt_partition_entry),
    };

    if(generate_guid(header.DiskGUID) != 0) {
        return -1;
    }

    if(disk_write(disk, (diskpos_t) { header.MyLBA, 0 }, &header, sizeof(header)) != 0) {
        return -1;
    }

    if(GPT_disk_update(disk) != 0) {
        return -1;
    }

    return 0;
}

static int gpt_verify_header(disk_t disk, diskpos_t pos) {
    struct gpt_header header;
    if(disk_read(disk, pos, &header, sizeof(header)) != 0) {
        return -1;
    }

    if(memcmp(header.Signature, GPT_SIGNATURE, 8) != 0) {
        lprintf(log_vdisk, LOG_LEVEL_DEBUG, "'EFI PART' signature missing");
        errno = EINVAL;
        return -1;
    }

    switch(header.Revision) {
        case GPT_REVISION_1_0:
            if(header.HeaderSize != sizeof(struct gpt_header)) {
                lprintf(log_vdisk, LOG_LEVEL_ERROR, "Invalid GPT header size %zu, expected %zu", (size_t) header.HeaderSize, sizeof(struct gpt_header));
                errno = EINVAL;
                return -1;
            }
            break;
        default:
            lprintf(log_vdisk, LOG_LEVEL_ERROR, "Unknown revision value: %#8jx", (uintmax_t) header.Revision);
            errno = EINVAL;
            return -1;
    }

    unsigned char partitions[header.NumberOfPartitionEntries * header.SizeOfPartitionEntry];
    if(disk_read(disk, (diskpos_t) { header.PartitionEntryLBA, 0 }, partitions, sizeof(partitions)) != 0) {
        return -1;
    }

    uint32_t partition_crc32 = crc32(0, NULL, 0);
    partition_crc32 = crc32(partition_crc32, (const void *) partitions, sizeof(partitions));

    if(header.PartitionEntryArrayCRC32 != partition_crc32) {
        lprintf(log_vdisk, LOG_LEVEL_ERROR, "Invalid partition entry array checksum");
        errno = EINVAL;
        return -1;
    }

    uint32_t header_crc32_old = header.HeaderCRC32;

    header.HeaderCRC32 = 0;
    
    uint32_t header_crc32 = crc32(0, NULL, 0);
    header_crc32 = crc32(header_crc32, (const void *) &header, sizeof(header));
    
    if(header_crc32_old != header_crc32) {
        lprintf(log_vdisk, LOG_LEVEL_ERROR, "Invalid header checksum");
        errno = EINVAL;
        return -1;
    }

    if(pos.sector == 1) {
        if(gpt_verify_header(disk, (diskpos_t) { header.AlternateLBA, 0 }) != 0) {
            return -1;
        }
    }

    return 0;
}

int GPT_disk_verify(disk_t disk) {
    if(gpt_verify_header(disk, (diskpos_t) { 1, 0 }) != 0) {
        return -1;
    }

    return 0;
}
