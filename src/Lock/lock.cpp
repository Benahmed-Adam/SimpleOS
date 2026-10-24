#include "Lock/lock.hpp"

Spinlock::Spinlock() { }

uint32_t Spinlock::acquire()
{
    uint32_t flags;
    asm volatile("pushf; pop %0; cli" : "=r"(flags) : : "memory");
    return flags;
}

void Spinlock::release(uint32_t flags)
{
    asm volatile("push %0; popf" : : "r"(flags) : "memory", "cc");
}

LockGuard::LockGuard(Spinlock& l)
    : _lock(l)
{
    _flags = _lock.acquire();
}

LockGuard::~LockGuard()
{
    _lock.release(_flags);
}
