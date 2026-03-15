#pragma once

#include <stdint.h>
#include <stddef.h>
#include "VGA/vga.hpp"

extern "C" void halt();

static inline void outb(uint16_t port, uint8_t val) {
    asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline void outw(uint16_t port, uint16_t val) {
    asm volatile ("outw %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

inline void *memset(void *p, int c, size_t count)
{
    for(unsigned int i = 0; i < count; i++)
    {
        ((char *)p)[i] = c;
    }
    return p;
}

inline void PANIC(const char *err) {
    //VGA::set_cursor(0, 1);
    VGA::set_color(VGA::RED, VGA::BLACK);
    VGA::print("PANIC: ", err);
    halt();
}

inline int strcmp(const char* s1, const char* s2)
{
    while(*s1 && (*s1 == *s2))
    {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

inline size_t strlen(const char *str)
{
    const char *s;

    for (s = str; *s; ++s);
    return (s - str);
}

inline int atoi(const char *strg)
{
    int res = 0;
    int i = 0;

    while (strg[i] != '\0')
    {
        res = res * 10 + (strg[i] - '0');
        i++;
    }

    return res;
}

inline void* memcpy(void* dest, const void* src, size_t count)
{
    char* dst = (char*)dest;
    const char* source = (const char*)src;
    for(size_t i = 0; i < count; i++)
    {
        dst[i] = source[i];
    }
    return dest;
}

inline char k_toupper(char c)
{
    if(c >= 'a' && c <= 'z')
    {
        return c - ('a' - 'A');
    }
    return c;
}