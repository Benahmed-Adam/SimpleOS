#include "Input/input.hpp"
#include <stdint.h>

void push_key(char c)
{
    uint32_t next = (PS2::head + 1) % PS2::KEYBOARD_BUFFER_SIZE;
    if (next != PS2::tail) {
        PS2::key_buffer[PS2::head] = c;
        PS2::head = next;
    }
}

uintptr_t handler_input(registers_t* regs)
{
    push_key(inb(0x60));
    return (uintptr_t)regs;
}

namespace PS2 {
char key_buffer[KEYBOARD_BUFFER_SIZE];
uint32_t head = 0;
uint32_t tail = 0;

const char kbd_map[] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' ', 0
};

const char kbd_map_shift[] = {
    0, 27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\"', '~',
    0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
    '*', 0, ' ', 0
};

bool shift_pressed = false;

char toAscii(uint8_t scancode)
{
    if (scancode == KEY_L_SHIFT || scancode == KEY_R_SHIFT) {
        shift_pressed = true;
        return 0;
    }
    if (scancode == (KEY_L_SHIFT | KEY_RELEASED) || scancode == (KEY_R_SHIFT | KEY_RELEASED)) {
        shift_pressed = false;
        return 0;
    }

    if (scancode & KEY_RELEASED) {
        return 0;
    }

    if (scancode >= sizeof(kbd_map)) {
        return 0;
    }

    return shift_pressed ? kbd_map_shift[scancode] : kbd_map[scancode];
}

char getKeyPressed()
{
    if (head == tail) {
        return 0;
    }
    char c = key_buffer[tail];
    tail = (tail + 1) % KEYBOARD_BUFFER_SIZE;
    return c;
}

void init_keyboard()
{
    VGA::println("Keyboard initialization");

    outb(0x64, 0xFF);
    uint8_t status = inb(0x64);
    VGA::println("Status received after reset: ", status);

    status = inb(0x64);
    if (status & (1 << 0)) {
        VGA::println("Output buffer full");
    } else {
        VGA::println("Output buffer empty");
    }

    if (status & (1 << 1)) {
        VGA::println("Input buffer full");
    } else {
        VGA::println("Input buffer empty");
    }

    if (status & (1 << 2)) {
        VGA::println("System flag present");
    } else {
        VGA::println("System flag absent");
    }

    if (status & (1 << 3)) {
        VGA::println("PS/2 device");
    } else {
        VGA::println("PS/2 controller");
    }

    if (status & (1 << 6)) {
        VGA::println("Timeout");
    } else {
        VGA::println("No timeout");
    }

    if (status & (1 << 7)) {
        VGA::println("Parity error");
    } else {
        VGA::println("No parity error");
    }

    outb(0x64, 0xAA);
    uint8_t result = inb(0x60);

    if (result == 0x55) {
        VGA::println("Tests passed");
    } else if (result == 0xFC) {
        VGA::println("Test failed");
    } else {
        VGA::println("Keyboard responded with unexpected code: ", result);
    }

    outb(0x64, 0x20);
    result = inb(0x60);
    VGA::println("Config byte: ", result);

    register_interrupt_handler(IRQ1, handler_input);

    VGA::println("Keyboard OK");
}
}