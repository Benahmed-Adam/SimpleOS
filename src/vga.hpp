#pragma once

#include <stdint.h>
#include <stddef.h>

namespace VGA {
    extern size_t row;
    extern size_t column;
    extern uint8_t color;

    enum Color : uint8_t {
        BLACK = 0,
        BLUE = 1,
        GREEN = 2,
        CYAN = 3,
        RED = 4,
        MAGENTA = 5,
        BROWN = 6,
        LIGHT_GREY = 7,
        DARK_GREY = 8,
        LIGHT_BLUE = 9,
        LIGHT_GREEN = 10,
        LIGHT_CYAN = 11,
        LIGHT_RED = 12,
        LIGHT_MAGENTA = 13,
        YELLOW = 14,
        WHITE = 15,
    };

    void initialize();
    void enable_cursor(uint8_t cursor_start, uint8_t cursor_end);
    void update_cursor(int x, int y);
    void disable_cursor();
    void set_cursor(int x, int y);
    void clear();
    void set_color(Color fg, Color bg);
    void refresh();

    void write(const char* data);
    void write(char data);
    void write(int data);
    void write(unsigned int data);
    void write(const void* data); // afficher une adresse
    void write(double data);
    void put_char(char c);
    void next_line();
    void put_entry_at(char c, uint8_t color, size_t x, size_t y);
    void scroll();
    void backspace();

    inline void print() {}

    template<typename T, typename... Args>
    void print(T first, Args... args) {
        write(first);
        print(args...);
    }

    template<typename... Args>
    void printAt(size_t x, size_t y, Args... args) {
        size_t oldCol = column;
        size_t oldRow = row;

        column = x;
        row = y;

        print(args...);

        column = oldCol;
        row = oldRow;
    }

    template<typename... Args>
    void println(Args... args) {
        print(args...);
        put_char('\n');
    }
}