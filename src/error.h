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

#ifndef __ERROR_H
#define __ERROR_H

// taken from /usr/include/linux/reboot.h
#define LINUX_REBOOT_CMD_RESTART 0x1234567
/**
 * Show formated string in stderr and start reboot process
 * if format pointer is different than NULL also show strerror
 * from errno.
 */
void panic(const char *format, ...);

/**
 * Reboot the system after show a message
 */
void system_reboot(const char *format, ...);
#endif
