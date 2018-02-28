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
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <sys/reboot.h>
#include <stdlib.h>
#include "error.h"

void system_reboot(const char *format, ...)
{
    va_list args;
    if (format != NULL) { 
        va_start(args, format);
        vfprintf(stderr, format, args);
        va_end(args);
    }

    puts("press enter to reboot");
    getchar();
    reboot(LINUX_REBOOT_CMD_RESTART);
}

void panic(const char *format, ...)
{
    va_list args;
    if (format != NULL) { 
        va_start(args, format);
        vfprintf(stderr, format, args);
        va_end(args);
    } else {
        fprintf(stderr, "%d:%s\n", errno, strerror(errno));
    }

    puts("a kernel panic will be raised");
    puts("press enter to continue");
    getchar();
    exit(1);
}
