#include "qglobal.h"

#ifndef BOOTIMAGE_H
#define BOOTIMAGE_H

/* tools/mkbootimg/bootimg.h
**
** Copyright 2007, The Android Open Source Project
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/
typedef struct boot_img_hdr boot_img_hdr;
#define BOOT_MAGIC "ANDROID!"
#define BOOT_MAGIC_SIZE 8
#define BOOT_NAME_SIZE 16
#define BOOT_ARGS_SIZE 512
#define BOOT_EXTRA_ARGS_SIZE 1024


#define DEFAULT_PAGE_SIZE      2048
#define DEFAULT_BASE_ADDRESS   0x00000000
#define DEFAULT_KERNEL_OFFSET  0x00008000
#define DEFAULT_RAMDISK_OFFSET 0x02000000
#define DEFAULT_SECOND_OFFSET  0x00f00000
#define DEFAULT_TAGS_OFFSET    0x01E00000



struct boot_img_hdr
{

    quint8 magic[BOOT_MAGIC_SIZE];

    quint32 kernel_size; /* size in bytes */

    quint32 kernel_addr; /* physical load addr */

    quint32 ramdisk_size; /* size in bytes */

    quint32 ramdisk_addr; /* physical load addr */

    quint32 second_size; /* size in bytes */

    quint32 second_addr; /* physical load addr */

    quint32 tags_addr; /* physical addr for kernel tags */

    quint32 page_size; /* flash page size we assume */

    quint32 dt_size; /* device tree in bytes */

    quint32 unused; /* future expansion: should be 0 */

    quint8 name[BOOT_NAME_SIZE]; /* asciiz product name */

    quint8 cmdline[BOOT_ARGS_SIZE];

    quint32 id[8]; /* timestamp / checksum / sha1 / etc */

    /* Supplemental command line data; kept here to maintain
    * binary compatibility with older versions of mkbootimg */
    quint8 extra_cmdline[BOOT_EXTRA_ARGS_SIZE];

};

/*
** +-----------------+
** | boot header     | 1 page
** +-----------------+
** | kernel          | n pages
** +-----------------+
** | ramdisk         | m pages
** +-----------------+
** | second stage    | o pages
** +-----------------+
**
** n = (kernel_size + page_size - 1) / page_size
** m = (ramdisk_size + page_size - 1) / page_size
** o = (second_size + page_size - 1) / page_size
**
** 0. all entities are page_size aligned in flash
** 1. kernel and ramdisk are required (size != 0)
** 2. second is optional (second_size == 0 -> no second)
** 3. load each element (kernel, ramdisk, second) at
**    the specified physical address (kernel_addr, etc)
** 4. prepare tags at tag_addr.  kernel_args[] is
**    appended to the kernel commandline in the tags.
** 5. r0 = 0, r1 = MACHINE_TYPE, r2 = tags_addr
** 6. if second_size != 0: jump to second_addr
**    else: jump to kernel_addr
*/




#endif // BOOTIMAGE_H
