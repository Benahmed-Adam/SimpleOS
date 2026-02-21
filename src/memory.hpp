#pragma once

#include <stddef.h>
#include "structs.hpp"

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