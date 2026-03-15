#include "gdt.hpp"
#include "idt.hpp"
#include "input.hpp"
#include "memory.hpp"
#include "pic.hpp"
#include "process.hpp"
#include "shell.hpp"
#include "structs.hpp"
#include "time.hpp"
#include "vga.hpp"
#include <stdint.h>

int idle_task(int, char**)
{
    while (true) {
        asm volatile("hlt");
    }
    return 0;
}

extern "C" void kmain(multiboot_info* info, uint32_t magic)
{
    if (magic != 0x2BADB002) {
        VGA::println("nuuh uh");
        return;
    }
    VGA::initialize();
    init_gdt();
    memory_init(info);
    remap_pic();
    init_idt();
    PS2::init_keyboard();
    init_timer(250);

    Scheduler::add_process(idle_task, "idle");
    Scheduler::add_process(shell, "shell");

    while (true) {
        asm volatile("hlt");
    }
}