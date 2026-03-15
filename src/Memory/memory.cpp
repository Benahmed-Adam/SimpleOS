#include "Memory/memory.hpp"
#include "VGA/vga.hpp"

struct Memory_block {
    bool isFree;
    size_t size;
    Memory_block* next;
    Memory_block* prev;
};

extern "C" uint32_t end;

Memory_block* memory_list = nullptr;
uint32_t mem_upper = 0;

bool memory_init(multiboot_info* info)
{
    if (!(info->flags & (1 << 6))) {
        VGA::println("Error: No mmap available.");
        return false;
    }

    uintptr_t kernel_end = (uintptr_t)&end;
    uint32_t mmap_ptr = info->mmap_addr;
    uint32_t mmap_end = info->mmap_addr + info->mmap_length;
    mem_upper = info->mem_upper;

    VGA::println("Analyzing memory...");

    while (mmap_ptr < mmap_end) {
        multiboot_mmap_entry* entry = (multiboot_mmap_entry*)mmap_ptr;

        if (entry->type == 1) {
            uintptr_t base = (uintptr_t)entry->addr;
            uintptr_t length = (uintptr_t)entry->len;

            if (base < kernel_end) {
                if (base + length > kernel_end) {
                    length -= (kernel_end - base);
                    base = kernel_end;
                } else {
                    length = 0;
                }
            }

            if (length > sizeof(Memory_block)) {
                memory_list = (Memory_block*)base;
                memory_list->isFree = true;
                memory_list->size = length - sizeof(Memory_block);
                memory_list->next = nullptr;
                memory_list->prev = nullptr;

                VGA::println("Start address: ", (void*)base);
                VGA::println("Size: ", (int)(length / 1024), " KB");

                break;
            }
        }

        mmap_ptr += entry->size + 4;
    }

    return true;
}

void* malloc(size_t size)
{
    Memory_block* curr = memory_list;

    while (curr != nullptr) {
        if (curr->isFree && curr->size >= size) {
            if (curr->size > size + sizeof(Memory_block) + 4) { // to split a piece if too big
                uintptr_t new_addr = (uintptr_t)curr + sizeof(Memory_block) + size;
                Memory_block* newBlock = (Memory_block*)new_addr;

                newBlock->size = curr->size - size - sizeof(Memory_block);
                newBlock->isFree = true;
                newBlock->next = curr->next;
                newBlock->prev = curr;

                if (newBlock->next != nullptr) {
                    newBlock->next->prev = newBlock;
                }

                curr->size = size;
                curr->next = newBlock;
            }

            curr->isFree = false;

            return (void*)((uintptr_t)curr + sizeof(Memory_block));
        }
        curr = curr->next;
    }

    return nullptr;
}

void free(void* ptr)
{
    if (ptr == nullptr)
        return;

    Memory_block* currPtr = (Memory_block*)((uintptr_t)ptr - sizeof(Memory_block));
    currPtr->isFree = true;

    if (currPtr->next != nullptr && currPtr->next->isFree) {
        currPtr->size += sizeof(Memory_block) + currPtr->next->size;
        currPtr->next = currPtr->next->next;
    }

    if (currPtr->prev != nullptr && currPtr->prev->isFree) {
        currPtr->prev->size += sizeof(Memory_block) + currPtr->size;
        currPtr->prev->next = currPtr->next;

        if (currPtr->next != nullptr) {
            currPtr->next->prev = currPtr->prev;
        }

        currPtr = currPtr->prev;
    }
}

float get_avalaible_memory_in_mb()
{
    float res = 0;
    const Memory_block* curr = memory_list;

    while (curr != nullptr) {
        if (curr->isFree) {
            res += curr->size;
        }
        curr = curr->next;
    }

    res /= (1024 * 1024);

    return res;
}

float get_total_memory_in_mb()
{
    return (mem_upper + 1024) / 1024;
}

void print_memory_blocks()
{
    const Memory_block* curr = memory_list;

    VGA::print("[ Start ] -> ");
    while (curr != nullptr) {
        VGA::print("[ ", (void*)curr, " | ", curr->size, " | ", curr->isFree ? "free" : "occupied", " ]  -> ");
        curr = curr->next;
    }

    VGA::println("[ End ]");
}

void* operator new(size_t size)
{
    return malloc(size);
}

void* operator new[](size_t size)
{
    return malloc(size);
}

void operator delete(void* ptr)
{
    return free(ptr);
}

void operator delete[](void* ptr)
{
    return free(ptr);
}

void operator delete(void* ptr, unsigned int)
{
    return free(ptr);
}

void operator delete[](void* ptr, unsigned int)
{
    return free(ptr);
}