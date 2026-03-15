#include "ISR/isr.hpp"
#include "IDT/idt.hpp"
#include "Process/process.hpp"
#include "VGA/vga.hpp"
#include "utils.hpp"
#include <stdint.h>

#define MASTER_COMMAND 0x20
#define MASTER_DATA 0x21
#define SLAVE_COMMAND 0xA0
#define SLAVE_DATA 0xA1

#define PIC_RESET 0x20

isr_handler_t interrupt_handlers[256];

extern "C" {
void register_interrupt_handler(uint8_t interrupt, isr_handler_t handler)
{
    interrupt_handlers[interrupt] = handler;
}

uintptr_t isr_handler(registers_t* regs)
{
    if (interrupt_handlers[regs->int_no]) {
        return interrupt_handlers[regs->int_no](regs);
    } else {
        if (Scheduler::is_process_running()) {
            VGA::set_color(VGA::RED, VGA::BLACK);
            VGA::println("[EXCEPTION] Dans : ", Scheduler::get_current_process()->name);
            VGA::println("Code : ", regs->int_no);

            switch (regs->int_no) {
            case DIVISION_BY_ZERO:
                VGA::println("Division par zero !");
                break;
            case INVALID_OPCODE_EXCEPTION:
                VGA::println("Instruction invalide");
                break;
            case GENERAL_PROTECTION_FAULT:
                VGA::println("General Protection Fault. Code: ", regs->err_code);
                break;
            case PAGE_FAULT:
                VGA::println("PAGE FAULT. Code d'erreur: ", regs->err_code);
                VGA::println("Adresse: ", regs->eip);
                break;
            case STACK_FAULT:
                VGA::println("Faute de pile. Code: ", regs->err_code);
                break;
            case DOUBLE_FAULT:
                VGA::println("DOUBLE FAULT !");
                break;
            default:
                VGA::print("Exception non geree : ", regs->int_no);
                break;
            }

            VGA::println("Tuer le processus...");
            VGA::set_color(VGA::BLACK, VGA::WHITE);

            Scheduler::kill_current_process();
            return Scheduler::schedule(regs);
        } else {
            VGA::set_color(VGA::RED, VGA::BLACK);
            VGA::print("\n\n===== KERNEL PANIC =====\n");
            VGA::print("Exception ");
            VGA::print(regs->int_no);
            VGA::print(" a EIP: ");
            VGA::println(regs->eip);
            VGA::println("Code d'erreur: ", regs->err_code);

            switch (regs->int_no) {
            case DIVISION_BY_ZERO:
                PANIC("Division par zero dans le kernel !");
                break;
            case INVALID_OPCODE_EXCEPTION:
                PANIC("Instruction invalide dans le kernel !");
                break;
            case GENERAL_PROTECTION_FAULT:
                PANIC("General Protection Fault dans le kernel !");
                break;
            case PAGE_FAULT:
                PANIC("Page Fault dans le kernel !");
                break;
            case DOUBLE_FAULT:
                PANIC("Double Fault !");
                break;
            default:
                PANIC("Exception fatale dans le kernel !");
                break;
            }
        }
    }
    return (uintptr_t)regs;
}

uintptr_t irq_handler(registers_t* regs)
{
    if (regs->int_no >= 40) {
        outb(SLAVE_COMMAND, PIC_RESET);
    }

    outb(MASTER_COMMAND, PIC_RESET);

    if (interrupt_handlers[regs->int_no] != 0) {
        return interrupt_handlers[regs->int_no](regs);
    }

    return (uintptr_t)regs;
}
}