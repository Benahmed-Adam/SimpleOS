#pragma once

#include <stdint.h>

class Spinlock {
public:
    Spinlock();
    uint32_t acquire();
    void release(uint32_t flags);
};

class LockGuard {
private:
    Spinlock& _lock;
    uint32_t _flags;

public:
    explicit LockGuard(Spinlock& l);
    ~LockGuard();

    LockGuard(const LockGuard&) = delete;
    LockGuard& operator=(const LockGuard&) = delete;
};
