/*
 multiboot.h - A stripped down version of the multiboot header

	This is included from grub-0.94

 Changes:
	Removed some #define's specific to the demo kernel.
	Changed the type names to agree with cos and be portable.
	
	Protected the damn thing from more than 1 inclusion!
*/
/* Copyright (C) 1999, 2001  Free Software Foundation, Inc.
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */
   
#ifndef _MULTIBOOT_H_
#define _MULTIBOOT_H_

/* Macros.  */

/* The magic number for the Multiboot header.  */
#define MULTIBOOT_HEADER_MAGIC		0x1BADB002

/* The magic number passed by a Multiboot-compliant boot loader.  */
#define MULTIBOOT_LOADER_MAGIC		0x2BADB002

/* Types.  */

/* The Multiboot header.  */
typedef struct multiboot_header
{
  u32_t magic;
  u32_t flags;
  u32_t checksum;
  u32_t header_addr;
  u32_t load_addr;
  u32_t load_end_addr;
  u32_t bss_end_addr;
  u32_t entry_addr;
} multiboot_header_t;

/* The symbol table for a.out.  */
typedef struct aout_symbol_table
{
  u32_t tabsize;
  u32_t strsize;
  u32_t addr;
  u32_t reserved;
} aout_symbol_table_t;

/* The section header table for ELF.  */
typedef struct elf_section_header_table
{
  u32_t num;
  u32_t size;
  u32_t addr;
  u32_t shndx;
} elf_section_header_table_t;

/* The Multiboot information.  */
typedef struct multiboot_info
{
  u32_t flags;
  u32_t mem_lower;
  u32_t mem_upper;
  u32_t boot_device;
  u32_t cmdline;
  u32_t mods_count;
  u32_t mods_addr;
  union
  {
    aout_symbol_table_t aout_sym;
    elf_section_header_table_t elf_sec;
  } u;
  u32_t mmap_length;
  u32_t mmap_addr;
} multiboot_info_t;

/* The module structure.  */
typedef struct module
{
  u32_t mod_start;
  u32_t mod_end;
  u32_t string;
  u32_t reserved;
} module_t;

/* The memory map. Be careful that the offset 0 is base_addr_low
   but no size.  */
typedef struct memory_map
{
  u32_t size;
  u32_t base_addr_low;
  u32_t base_addr_high;
  u32_t length_low;
  u32_t length_high;
  u32_t type;
} memory_map_t;

#endif // !_MULTIBOOT_H_
