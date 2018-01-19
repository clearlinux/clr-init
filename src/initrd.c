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

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/mount.h>
#include "error.h"
#include "device_handler.h"
#include "initrd.h"

struct virtual_fs {
    const char *target;
    const char *filesystem;
    unsigned long mountflags;
    const void *data;
};
typedef struct virtual_fs virtual_fs_t;

static virtual_fs_t vfs[] = {
    { "/proc",    "proc",       MS_NODEV | MS_NOEXEC | MS_NOSUID,   NULL},
    { "/dev",     "devtmpfs",   0,                                  "size=10240k,mode=0755"}
};

static int get_cmdline_param(const char *cmdline, const char *param, char *value)
{
    char *value_end = NULL;
    int value_len = 0;
    char *param_start = strstr(cmdline, param);
    if (param_start == NULL) {
        return 0;
    }
    
    param_start += strlen(param) + 1;
    value_end = strpbrk(param_start, " \t");
    if (value_end == NULL) {
        value_end = param_start + strlen(param_start);
    }
    
    value_len = value_end - param_start;
    memset(value, 0, value_len + 1);
    memcpy((void*)value, (void*)param_start, value_len);
   
    return value_len + 1;
}

static size_t get_root_type(char *cmdline, char *devname)
{
    int ret = 0;
    char root_param[MAX_CMDLINE_LEN];
    
    if ((ret = get_cmdline_param(cmdline, "root", root_param)) == 0) {
        panic("ERROR GETTING ROOT FROM CMDLINE\n");
    }

    if (get_cmdline_param(root_param, "PARTUUID", devname) != 0) {
        return ROOT_PARTUUID;
    }

    if (get_cmdline_param(root_param, "UUID", devname) != 0) {
        return ROOT_UUID;
    }

    memcpy(devname, root_param, ret);
    return ROOT_DEVNAME;
}

void get_root_dev(char *root_dev, char *fs_type)
{
    FILE *fp_cmdline = NULL;
    char cmdline[MAX_CMDLINE_LEN];
    char root_value[MAX_CMDLINE_LEN];
    int ret = 0;

    fp_cmdline = fopen(CMD_LINE_PATH, "r");
    if (fp_cmdline == NULL) {
        panic(NULL);
    }
    
    ret = fread((void*) cmdline, sizeof(char) * MAX_CMDLINE_LEN, 1, fp_cmdline);
    if(!feof(fp_cmdline) && ret != 1){
        fclose(fp_cmdline);
        panic("ERROR READING FROM %s OR EOF\n", CMD_LINE_PATH);
    }

    if (get_cmdline_param(cmdline, "rootfstype", fs_type) == 0) {
        fclose(fp_cmdline);
        panic("ERROR GETTING ROOTFS_TYPE FROM CMDLINE\n");
    }

    switch (get_root_type(cmdline, root_value)) {
        case ROOT_UUID:
            fclose(fp_cmdline);
            panic("ROOT=UUID IS NOT IMPLEMENTED YET\n");
        break;
        case ROOT_PARTUUID:
            find_partUUID(root_value, root_dev);
        break;
        case ROOT_DEVNAME:
            memcpy(root_dev, root_value, MAX_CMDLINE_LEN);
        break;
        default:
            panic("ERROR GETTING ROOT FROM CMDLINE\n");
        break;
    }

    fclose(fp_cmdline);
}

void clean_initrd()
{
    rmdir("/dev");
    rmdir("/proc");
    unlink("/init");
}

void mount_virtual_fs()
{
    int i = 0;

    for (i = 0; i < sizeof(vfs)/sizeof(virtual_fs_t); i++) {
        if (mount(NULL, vfs[i].target, vfs[i].filesystem, vfs[i].mountflags, vfs[i].data)) {
            panic("ERROR MOUNTING %s\n", vfs[i].filesystem);
        }
    }

}

void mount_root(const char *root_partition, char *fs_types)
{
    char *fs_type = NULL;
    fs_type = strtok(fs_types, ",");
    while (fs_type != NULL) {
        if (mount(root_partition, "/root", fs_type, MS_NOATIME|MS_NODIRATIME, NULL) == 0) {
            break;
        }
        fs_type = strtok(NULL, ",");
    }

    if (fs_type == NULL) {
        panic("ERROR MOUNTING %s\n", "/root");
    }
}
