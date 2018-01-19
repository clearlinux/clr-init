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

#ifndef __INITRD_H
#define __INITRD_H

#define MAX_CMDLINE_LEN 1024
#define MAX_DEV_PATH 512

#define CMD_LINE_PATH "/proc/cmdline"

// ROOT types
#define ROOT_UUID 1
#define ROOT_PARTUUID 2
#define ROOT_DEVNAME 3

#define ROOT_MAPPED_NAME "clr-root"
#define ROOT_MAPPED_PATH "/dev/mapper/"ROOT_MAPPED_NAME

/**
 * gets root device information from cmdline options
 */
void get_root_dev(char *root_dev, char *fs_type);

/**
 * Clean Initrd environment
 */
void clean_initrd();

/**
 * Mount virtual filesytems predefined
 */
void mount_virtual_fs();

/**
 * Mount root filesystem trying with every fs_types
 */
void mount_root(const char *root_partition, char *fs_types);

#endif
