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
#include <sys/mount.h>
#include <stdlib.h>
#include <string.h>
#include "initrd.h"
#include "crypting.h"
#include "error.h"

int main(int argc __attribute__((unused)), char *argv[])
{
    char root_partition[MAX_DEV_PATH];
    char fs_types[MAX_DEV_PATH];
    int err = 0;
    int i = 0;

    mount_virtual_fs();

    get_root_dev(root_partition, fs_types);
    
    if (is_LUKS(root_partition) == CRYPT_ISLUKS_TRUE) {
        for (i = 0; i < CRYPT_PASSWORD_TRIES; i++) {
            char *password = NULL;
            password = getpass("type passphrase for root device:");
            err = decrypt_device(root_partition,
                ROOT_MAPPED_NAME, password, (int)strlen(password));
            free(password);
            if (err != CRYPT_WRONG_PASS) { 
                memset((void*)root_partition, 0, MAX_DEV_PATH);
                memcpy((void*)root_partition, ROOT_MAPPED_PATH, strlen(ROOT_MAPPED_PATH));
                break;
            }
        }
        if (err == CRYPT_WRONG_PASS) {
            system_reboot("YOU TRIED MORE THAN %d TIMES\n", CRYPT_PASSWORD_TRIES);
        }
    }

    mount_root(root_partition, fs_types);

    clean_initrd();

    if (chdir("/root") < 0) {
        panic("ERROR CHANGING DIR TO %s\n", "/root");
    }

    mount(".", "/", NULL, MS_MOVE, NULL);
    if (chroot(".")) {
        panic("ERROR CHANGING ROOT TO %s\n", "/root");
    }
    if (chdir("/")) {
        panic("ERROR CHANGING DIR TO %s\n", "/");
    }
    execv("/sbin/init", argv);
}
