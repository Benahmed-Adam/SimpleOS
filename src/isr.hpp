#pragma once

#include <stdint.h>

struct registers_t {
    // Segments poussés manuellement (LIFO : le dernier poussé est le premier ici)
    uint32_t ds;       // Poussé en dernier
    uint32_t es;
    uint32_t fs;
    uint32_t gs;       // Poussé en premier

    // Registres poussés par pusha (Ordre inverse de pusha)
    uint32_t edi, esi, ebp, esp_unused, ebx, edx, ecx, eax;

    // Poussés par le macro isr_noerr / isr_err
    uint32_t int_no, err_code;

    // Poussés automatiquement par le processeur lors de l'interruption
    uint32_t eip, cs, eflags, useresp, ss;
};

extern "C" {
    typedef uintptr_t (*isr_handler_t)(registers_t*);
    void register_interrupt_handler(uint8_t interrupt, isr_handler_t handler);
    uintptr_t isr_handler(registers_t* regs);
    uintptr_t irq_handler(registers_t* regs);
}

#define IRQ0 32
#define IRQ1 33
#define IRQ2 34
#define IRQ3 35
#define IRQ4 36
#define IRQ5 37
#define IRQ6 38
#define IRQ7 39
#define IRQ8 40
#define IRQ9 41
#define IRQ10 42
#define IRQ11 43
#define IRQ12 44
#define IRQ13 45
#define IRQ14 46
#define IRQ15 47

enum IsrCode : uint32_t {
    DIVISION_BY_ZERO = 1,
    DEBUG_EXCEPTION,
    NON_MASKABLE_INTERRUPT,
    BREAKPOINT_EXCEPTION,
    INTO_DETECTED_OVERFLOW,
    OUT_OF_BOUNDS_EXCEPTION,
    INVALID_OPCODE_EXCEPTION,
    NO_COPROCESSOR_EXCEPTION,
    DOUBLE_FAULT,
    COPROCESSOR_SEGMENT_OVERRUN,
    BAD_TSS,
    SEGMENT_NOT_PRESENT,
    STACK_FAULT,
    GENERAL_PROTECTION_FAULT,
    PAGE_FAULT,
    UNKNOWN_INTERRUPT_EXCEPTION,
    COPROCESSOR_FAULT,
    ALIGNMENT_CHECK_EXCEPTION,
    MACHINE_CHECK_EXCEPTION,
};