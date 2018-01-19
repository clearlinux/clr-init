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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include "error.h"
#include "device_handler.h"

static void UUID_parser(const char *c_uuid, uuid_t *uuid)
{
    if (strlen(c_uuid) != UUID_STR_LEN) {
        panic("INCORRECT PARTUUID FORMAT FOR %s\n", c_uuid);
    }

    memset((void*)uuid, 0, sizeof(uuid_t));
    uint32_t tmp = strtol(&c_uuid[POS_UUID_STR_SECTION_1], NULL, BASE_16);
    uuid->high_uuid = tmp;

    uint16_t tmp2 = strtol(&c_uuid[POS_UUID_STR_SECTION_2], NULL, BASE_16);
    uuid->middle_uuid1 = tmp2;

    uint16_t tmp3 = strtol(&c_uuid[POS_UUID_STR_SECTION_3], NULL, BASE_16);
    uuid->middle_uuid2 = tmp3;

    uint16_t tmp4 = strtol(&c_uuid[POS_UUID_STR_SECTION_4], NULL, BASE_16);

    uint64_t tmp5 = strtol(&c_uuid[POS_UUID_STR_SECTION_5], NULL, BASE_16);
    uuid->low_uuid = htobe16(tmp4) | htobe64(tmp5);
}

static int get_GPTheader(GPT_header_t *header, char *device_path)
{
    FILE *fp = NULL;
    int ret = 0;
    int header_amount = 1;

    fp = fopen(device_path, "r");
    if (fp == NULL) {
        panic(NULL);
    }
    
    if (fseek(fp, GPT_POSITION, SEEK_SET) < 0) {
        fclose(fp);
        panic(NULL);
    }

    if ((ret = fread((void*)header, sizeof(GPT_header_t), header_amount, fp)) != header_amount) {
        fclose(fp);
        panic("ERROR READING FROM %s OR EOF\n", device_path);
    }

    fclose(fp);
    return ret;
}

static size_t get_devGUIDs(int size, GUID_entry_t *guid, char *device_path)
{
    FILE *fp = NULL;
    int i = 0;
    
    fp = fopen(device_path, "r");
    if (fp == NULL) {
        panic(NULL);
    }

    if (fseek(fp, GUID_ENTRY_POSITION, SEEK_SET) < 0) {
        fclose(fp);
        panic(NULL);
    }

    if (fread((void*)guid, sizeof(GUID_entry_t), size, fp) != size) {
        fclose(fp);
        panic("ERROR READING FROM %s OR EOF\n", device_path);
    }

    for (i = 0; i < size; i++) {
        if (guid[i].partition_type.high_uuid == 0)
            break;
    }

    fclose(fp);
    return (size_t)i;
}

void find_partUUID(const char *c_partuuid, char *dev_path)
{
    DIR *p_dir = NULL;
    struct dirent *p_dir_entry = NULL;
    uuid_t partuuid;

    UUID_parser(c_partuuid, &partuuid);

    memset(dev_path, 0, DEV_PATH_LEN);

    p_dir = opendir("/dev");
    if (p_dir == NULL) {
        panic(NULL);
    }

    while ((p_dir_entry = readdir(p_dir)) != NULL) {
        
        if (p_dir_entry->d_type == DT_BLK) {
            
            char full_name[DEV_PATH_LEN];
            snprintf(full_name, DEV_PATH_LEN, "%s/%s", "/dev", p_dir_entry->d_name);
            GPT_header_t tmp_header;
        
            memset(&tmp_header, 0, sizeof(GPT_header_t));
            get_GPTheader(&tmp_header, full_name);
            
            if (strncmp((const char*)&tmp_header.signature, GPT_SIG_ID, GPT_SIG_ID_LEN) == 0) {
                GUID_entry_t *entries = NULL;
                int partitions;
                int i;
                entries = malloc(
                    sizeof(GUID_entry_t) * tmp_header.number_partition_entries);
                memset(entries, 0, sizeof(GUID_entry_t) * tmp_header.number_partition_entries);

                partitions = get_devGUIDs(
                    tmp_header.number_partition_entries, entries,
                        full_name);

                for (i = 0; i < partitions; i++) {
                    if (memcmp(&entries[i].partuuid, &partuuid, sizeof(uuid_t)) == 0) {
                        snprintf(dev_path, DEV_PATH_LEN, "%s%d", full_name, i+1);
                        break;
                    }
                }

                free(entries);
                if (strlen(dev_path) > 0)
                    break;
            }
        }
    }
    closedir(p_dir);
}

//int find_UUID(char* dev_path)
//{
//
//    return 0;
//}

