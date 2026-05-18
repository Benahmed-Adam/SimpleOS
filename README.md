# SimpleOS - A Simple x86 Kernel

A simple 32-bit operating system kernel written in C++ for x86 architecture. This project is an educational operating system that demonstrates fundamental OS concepts including memory management, interrupt handling, process scheduling, and a simple shell.

## Features

- **Multiboot Compliant** - Boot via GRUB2
- **Global Descriptor Table (GDT)** - Segmentation setup for protected mode
- **Interrupt Descriptor Table (IDT)** - Interrupt and exception handling
- **Programmable Interrupt Controller (PIC)** - IRQ management and remapping
- **VGA Text Mode** - Display output with color support
- **PS/2 Keyboard Input** - Basic keyboard driver
- **Memory Management** - Basic physical memory allocation via Multiboot info
- **Process Scheduler** - Simple cooperative multitasking with an idle task
- **Timer Interrupts** - System timer for scheduling
- **Interactive Shell** - Command-line interface with built-in commands

## Building

### Prerequisites

- GCC cross-compiler for i686: `i686-linux-gnu-g++`
- NASM assembler
- GRUB tools: `grub-mkrescue`
- QEMU (for testing): `qemu-system-i386`

On Ubuntu/Debian:
```bash
sudo apt-get install gcc-i686-linux-gnu nasm grub-common grub-pc-bin qemu-system-x86
```

### Build Commands

```bash
# Build the kernel and create ISO
make all

# Run in QEMU
make run

# Clean everything
make clean
```

## Shell Commands

The built-in shell supports the following commands:

| Command    | Description                      |
|------------|----------------------------------|
| `help`     | Display available commands       |
| `clear`    | Clear the screen                 |
| `hello`    | Print a greeting message         |
| `info`     | Show system information          |
| `uptime`   | Display system uptime            |
| `memory`   | Show memory usage                |
| `color`    | Change text color                |
| `echo`     | Print text to console            |
| `banner`   | Display ASCII art banner         |
| `reboot`   | Reboot the system                |
| `shutdown` | Shutdown the system              |
| `ps`       | List running processes           |
| `kill`     | Terminate a process             |

## Inspiration

This project was inspired by and built with reference to:

- [knusbaum/kernel](https://github.com/knusbaum/kernel/) - A reference for simple kernel structure and design
- [OSDev Wiki](https://wiki.osdev.org) - The comprehensive resource for OS development tutorials and documentation
