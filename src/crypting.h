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

#ifndef __CRYPTING_H
#define __CRYPTING_H

#include <stdint.h>

#define CRYPT_PASSWORD_TRIES 3

#define CRYPT_WRONG_PASS 0

#define CRYPT_ISLUKS_TRUE 1
#define CRYPT_ISLUKS_FALSE 0

#define CRYPT_HEADER_MAGIC "LUKS\xBA\xBE"
#define CRYPT_HEADER_MAGIC_SIZE 6
#define CRYPT_HEADER_DIGESTSIZE 20
#define CRYPT_HEADER_SALTSIZE 32
#define CRYPT_HEADER_CIPHER_NAME_SIZE 32
#define CRYPT_HEADER_CIPHER_MODE_SIZE 32
#define CRYPT_HEADER_HASH_SPEC_SIZE 32
#define CRYPT_HEADER_UUID_SIZE 40

#define CRYPT_HEADER_AMOUNT_KEYSLOTS 8

struct LUKS_keyslot {
    uint32_t active;
    uint32_t iterations;
    uint8_t salt[CRYPT_HEADER_SALTSIZE];
    uint32_t key_material_offset;
    uint32_t stripes;
} __attribute__((packed));
typedef struct LUKS_keyslot LUKS_keyslot_t;

struct LUKS_header {
    uint8_t magic[CRYPT_HEADER_MAGIC_SIZE];
    uint16_t version;
    char cipher_name[CRYPT_HEADER_CIPHER_NAME_SIZE];
    char cipher_mode[CRYPT_HEADER_CIPHER_MODE_SIZE];
    char hash_spec[CRYPT_HEADER_HASH_SPEC_SIZE];
    uint32_t payload_offset;
    uint32_t key_bytes;
    uint8_t mk_digest[CRYPT_HEADER_DIGESTSIZE];
    uint8_t mk_digest_salt[CRYPT_HEADER_SALTSIZE];
    uint32_t mk_digest_iter;
    char uuid[CRYPT_HEADER_UUID_SIZE];
    LUKS_keyslot_t keyslots[CRYPT_HEADER_AMOUNT_KEYSLOTS];
} __attribute__((packed));
typedef struct LUKS_header LUKS_header_t;

/**
 * Return 1 if device has luks encryption and 0 if not
 */
int is_LUKS(const char *device_path);

/**
 * decrypt partition and create decrypted device with mapped_name in /dev/mapper/
 * returns 1 if password is ok and all is well, 0 if password is wrong
 */
int decrypt_device(const char *device_path, const char *mapped_name, const char *pass, int pass_size);

#endif
