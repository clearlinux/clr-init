/***
 Copyright © 2018 Intel Corporation
 
 Author: Josue David Hernandez Gutierrez <josue.d.hernandez.gutierrez@intel.com>
 
 This file is part of clr-initrd
 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published
 by the Free Software Foundation, either version 3 of the License,
 or (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program. If not, see <https://www.gnu.org/licenses/>.

***/

#ifndef __DEVICE_HANDLER_H
#define __DEVICE_HANDLER_H

#include <stdint.h>

#define DEV_PATH_LEN 512

//
// UUID string section numbers
// 
// 1        2    3    4    5
// XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX
//
#define POS_UUID_STR_SECTION_1 0
#define POS_UUID_STR_SECTION_2 9
#define POS_UUID_STR_SECTION_3 14
#define POS_UUID_STR_SECTION_4 19
#define POS_UUID_STR_SECTION_5 24

#define UUID_STR_LEN 36

#define BASE_16 16

// GPT
#define GPT_POSITION 512
#define GUID_ENTRY_POSITION 1024

#define GPT_SIG_ID "EFI PART"
#define GPT_SIG_ID_LEN 8

#define GPT_PARTITION_NAME_SIZE 72

struct uuid {
    uint32_t high_uuid;
    uint16_t middle_uuid1;
    uint16_t middle_uuid2;
    uint64_t low_uuid;
} __attribute__((packed));
typedef struct uuid uuid_t;

struct GPT_header {
    uint64_t signature;
    uint32_t revision;
    uint32_t header_size;
    uint32_t CRC32_zlib_header;
    uint32_t reserved; // must be zero
    uint64_t current_LBA;
    uint64_t backup_LBA;
    uint64_t first_usable_LBA;
    uint64_t lastu_usable_LBA;
    uuid_t   disk_uuid;
    uint64_t starting_LBA_partition_entries;
    uint32_t number_partition_entries;
    uint32_t size_single_parition_entry;
    uint32_t CRC32_zlib_partition_array_little_entian;
} __attribute__((packed));
typedef struct GPT_header GPT_header_t;

struct GUID_entry {
    uuid_t  partition_type;
    uuid_t  partuuid;
    uint64_t firs_LBA; //little endian
    uint64_t last_LBA;
    uint64_t flags;
    char    partition_name[GPT_PARTITION_NAME_SIZE];
} __attribute__((packed));
typedef struct GUID_entry GUID_entry_t;

/**
 * set dev_path pointed string with dev path associated with PARTUUID
 */
void find_partUUID(const char *c_partuuid, char *dev_path);

#endif
