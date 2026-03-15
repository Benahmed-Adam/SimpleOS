#include "Time/time.hpp"
#include "ISR/isr.hpp"
#include "Process/process.hpp"
#include "VGA/vga.hpp"
#include "utils.hpp"

volatile uint32_t ticks;
uint32_t frequency;

uintptr_t timer_handler(registers_t* regs)
{
    ticks++;
    return Scheduler::schedule(regs);
}

void init_timer(uint32_t frequency)
{
    ::frequency = frequency;
    uint32_t divisor = 1193182 / frequency;

    outb(0x43, 0x36);

    uint8_t low = (uint8_t)(divisor & 0xFF);
    uint8_t high = (uint8_t)((divisor >> 8) & 0xFF);

    outb(0x40, low);
    outb(0x40, high);

    VGA::println("PIT initialized at ", frequency, " Hz");

    register_interrupt_handler(IRQ0, timer_handler);
}

void sleep(uint32_t ms)
{
    uint32_t ticks_to_wait = (ms * frequency) / 1000;

    if (ms > 0 && ticks_to_wait == 0)
        ticks_to_wait = 1;

    uint32_t end = ticks + ticks_to_wait;

    while (ticks < end) {
        asm volatile("hlt");
    }
}

uint32_t uptime()
{
    return ticks / frequency;
}