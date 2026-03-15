#pragma once

#include <cstdint>
#include <stddef.h>

struct __attribute__((packed)) multiboot_info {
    uint32_t flags; // Flags indicating which fields are valid
    uint32_t mem_lower; // Amount of lower memory (below 1 MB) in KB
    uint32_t mem_upper; // Amount of upper memory (above 1 MB) in KB
    uint32_t boot_device; // Boot device (BIOS disk device)
    uint32_t cmdline; // Pointer to the command line string
    uint32_t mods_count; // Number of modules loaded
    uint32_t mods_addr; // Pointer to the first module structure
    uint32_t syms[4]; // Symbol table information (deprecated)
    uint32_t mmap_length; // Size of the memory map
    uint32_t mmap_addr; // Pointer to the memory map
    uint32_t drives_length; // Size of the BIOS drive information
    uint32_t drives_addr; // Pointer to the BIOS drive information
    uint32_t config_table; // Pointer to the ROM configuration table
    uint32_t boot_loader_name; // Pointer to the bootloader name string
    uint32_t apm_table; // Pointer to the APM (Advanced Power Management) table
    uint32_t vbe_control_info; // VBE (VESA BIOS Extensions) control information
    uint32_t vbe_mode_info; // VBE mode information
    uint16_t vbe_mode; // VBE mode number
    uint16_t vbe_interface_seg; // VBE interface segment
    uint16_t vbe_interface_off; // VBE interface offset
    uint16_t vbe_interface_len; // VBE interface length
};

struct __attribute__((packed)) multiboot_mmap_entry {
    uint32_t size; // Size of this entry (often 20 or 24 bytes)
    uint64_t addr; // Start address of the RAM zone
    uint64_t len; // Length of the RAM zone
    uint32_t type; // 1 = Available, everything else = Do not touch!
};

bool memory_init(multiboot_info* info);
void* malloc(size_t size);
void free(void* ptr);

void print_memory_blocks();
float get_avalaible_memory_in_mb();
float get_total_memory_in_mb();

void* operator new(size_t size);
void* operator new[](size_t size);
void operator delete(void* ptr);
void operator delete[](void* ptr);