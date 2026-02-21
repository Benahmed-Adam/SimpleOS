#pragma once

#include <stdint.h>
#include "isr.hpp"

#define STACK_SIZE 4096

typedef int (*Program)(int argc, char *argv[]);

enum State {
    RUNNING = 0,
    BLOCKED,
    FINISHED,
};

struct Process {
    uint32_t id;
    uintptr_t esp;
    uint8_t* stack;
    uintptr_t entry_point;
    State state;
    int argc;
    char **argv;
    const char* name;
    uint32_t pid = 0;
    Process* next = nullptr;

    void init(Program prog, int argc, char *argv[], const char* name);
    void save(registers_t* r);
    uintptr_t load();
};

void process_wrapper();

namespace Scheduler {
    void add_process(Program prog, const char* name = "", int argc = 0, char *argv[] = nullptr);
    uintptr_t schedule(registers_t* regs);
    void kill_current_process();
    bool is_process_running();
    const Process* get_current_process();
    Process* get_process_list();
    void remove_process(Process* proc);
}