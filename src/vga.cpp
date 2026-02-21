#include "vga.hpp"
#include "utils.hpp"
#include "lock.hpp"

const size_t WIDTH = 80;
const size_t HEIGHT = 25;

inline uint8_t make_color(VGA::Color fg, VGA::Color bg) {
    return fg | bg << 4;
}

inline uint16_t make_entry(unsigned char uc, uint8_t color) {
    return static_cast<uint16_t>(uc) | static_cast<uint16_t>(color) << 8;
}

namespace VGA {
    size_t row = 0;
    size_t column = 0;
    uint8_t color = 0;
    uint16_t* buffer = nullptr;
    Spinlock vgaLock;
    
    void initialize() {
        row = 0;
        column = 0;
        color = make_color(BLACK, WHITE);
        buffer = reinterpret_cast<uint16_t*>(0xB8000);
        clear();
        enable_cursor(0, 15);
    }

    void update_cursor(int x, int y) {
        uint16_t pos = y * 80 + x; 

        outb(0x3D4, 0x0F);
        outb(0x3D5, (uint8_t) (pos & 0xFF));
        outb(0x3D4, 0x0E);
        outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
    }

    void enable_cursor(uint8_t cursor_start, uint8_t cursor_end) {
        outb(0x3D4, 0x0A);
        outb(0x3D5, (inb(0x3D5) & 0xC0) | cursor_start);

        outb(0x3D4, 0x0B);
        outb(0x3D5, (inb(0x3D5) & 0xE0) | cursor_end);
    }

    void disable_cursor() {
        outb(0x3D4, 0x0A);
        outb(0x3D5, 0x20);
    }

    void set_cursor(int x, int y) {
        column = x;
        row = y;
    }

    void clear() {
        for (size_t y = 0; y < HEIGHT; y++) {
            for (size_t x = 0; x < WIDTH; x++) {
                put_entry_at(' ', color, x, y);
            }
        }
        column = 0;
        row = 0;
    }

    void set_color(Color fg, Color bg) {
        color = make_color(fg, bg);
    }

    void refresh() {
        size_t old_x = column;
        size_t old_y = row;
        set_cursor(0, 0);

        for (size_t y = 0; y < HEIGHT; y++) {
            for (size_t x = 0; x < WIDTH; x++) {
                put_entry_at(buffer[y * WIDTH + x], color, x, y);
            }
        }

        set_cursor(old_x, old_y);
    }

    void write(const char* data) {
        for (size_t i = 0; data[i] != '\0'; i++) {
            put_char(data[i]);
        }
    }

    void write(char data) {
        put_char(data);
    }

    void write(int data) {
        if (data == 0) {
            put_char('0');
            return;
        }

        if (data < 0) {
            put_char('-');
            data = -data;
        }

        char buffer[21];
        int i = 0;

        while (data > 0) {
            buffer[i++] = (data % 10) + '0';
            data /= 10;
        }

        while (i > 0) {
            put_char(buffer[--i]);
        }
    }

    void write(unsigned int data) {
        if (data == 0) {
            put_char('0');
            return;
        }

        char buffer[21];
        int i = 0;

        while (data > 0) {
            buffer[i++] = (data % 10) + '0';
            data /= 10;
        }

        while (i > 0) {
            put_char(buffer[--i]);
        }
    }

    void write(const void* ptr) {
        uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);

        const char* digits = "0123456789ABCDEF";
        write("0x");
        
        int start_shift = (sizeof(uintptr_t) * 8) - 4;

        for (int i = start_shift; i >= 0; i -= 4) {
            put_char(digits[(addr >> i) & 0xF]);
        }
    }

    void write(double data) {
        if (data == 0) {
            put_char('0');
            return;
        }

        if (data < 0) {
            put_char('-');
            data = -data;
        }

        long int_part = (long)data;
        write((int)int_part);

        put_char('.');

        double diff = data - (double)int_part;
        
        const int precision = 2;
        for (int i = 0; i < precision; i++) {
            diff *= 10;
            int digit = (int)diff;
            put_char(digit + '0');
            diff -= digit;
        }
    }

    void put_char(char c) {
        LockGuard l(vgaLock);

        if (c == '\n') {
            next_line();
        } else {
            put_entry_at(c, color, column, row);
            if (++column == WIDTH) {
                next_line();
            }
        }
        update_cursor(column, row);
    }

    void put_entry_at(char c, uint8_t color, size_t x, size_t y) {
        const size_t index = y * WIDTH + x;
        buffer[index] = make_entry(c, color);
    }

    void next_line() {
        column = 0;
        if (++row == HEIGHT) {
            row = HEIGHT - 1;
            scroll();
        }
    }

    void scroll() {
        for (size_t i = WIDTH; i < HEIGHT * WIDTH; i++) {
            buffer[i - WIDTH] = buffer[i];
        }
        for (size_t i = (HEIGHT - 1) * WIDTH; i < HEIGHT * WIDTH; i++) {
            buffer[i] = make_entry(' ', color);
        }
    }

    void backspace() {
        column--;
        printAt(column, row, " "); 
        update_cursor(column, row); 
    }
}