#include "lock.hpp"

Spinlock::Spinlock() {}

void Spinlock::acquire() {
    asm volatile("cli" : : : "memory");
}

void Spinlock::release() {
    asm volatile("sti" : : : "memory");
}

LockGuard::LockGuard(Spinlock& l) : _lock(l) {
    _lock.acquire();
}

LockGuard::~LockGuard() {
    _lock.release();
}