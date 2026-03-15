#pragma once

class Spinlock {
public:
    Spinlock();
    void acquire();
    void release();
};

class LockGuard {
private:
    Spinlock& _lock;

public:
    explicit LockGuard(Spinlock& l);
    ~LockGuard();

    LockGuard(const LockGuard&) = delete;
    LockGuard& operator=(const LockGuard&) = delete;
};