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

#include <stdio.h>
#include <string.h>
#include <libcryptsetup.h>
#include "error.h"
#include "crypting.h"

int decrypt_device(const char *device_path, const char *mapped_name, const char *pass, int pass_size)
{
    struct crypt_device *dev = NULL;
    int ret = 1;

    if (crypt_init(&dev, device_path) < 0) {
        panic(NULL);
    }

    if (crypt_load(dev, CRYPT_LUKS1, NULL) < 0) {
        crypt_free(dev);
        panic(NULL);
    }

    // checking for passphrase
    if (crypt_activate_by_passphrase(
        dev, NULL, CRYPT_ANY_SLOT,
        pass, pass_size, 0)) {
        crypt_free(dev);
        return CRYPT_WRONG_PASS;
    }

    if (crypt_activate_by_passphrase(
        dev, mapped_name, CRYPT_ANY_SLOT,
        pass, pass_size, 0)) {
        crypt_free(dev);
        panic(NULL);
    }

    crypt_free(dev);
    return ret;
}

int is_LUKS(const char *device_path)
{
    FILE *fp = NULL;
    LUKS_header_t header;
    int ret = CRYPT_ISLUKS_FALSE;
    int amount_elem = 1;

    fp = fopen(device_path, "r");
    if (fp == NULL) {
        panic(NULL);
    }
    
    if (fread((void*)&header, sizeof(header), amount_elem, fp) < amount_elem) {
        fclose(fp);
        panic("ERROR READING FROM %s OR EOF\n", device_path);
    }
    
    if (strncmp((const char*)&header.magic,
        CRYPT_HEADER_MAGIC, CRYPT_HEADER_MAGIC_SIZE) == 0) {
        fclose(fp);
        return CRYPT_ISLUKS_TRUE;
    }

    fclose(fp);
    return ret;
}

