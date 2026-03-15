#include "process.hpp"
#include "lock.hpp"
#include "memory.hpp"
#include "utils.hpp"
#include "vga.hpp"

void Process::init(Program prog, int argc, char* argv[], const char* name)
{
    this->entry_point = (uintptr_t)prog;
    this->argc = argc;
    this->argv = argv;
    this->name = name;
    uintptr_t stack_top = (uintptr_t)this->stack + STACK_SIZE;

    stack_top -= sizeof(registers_t);
    this->esp = stack_top;

    registers_t* regs = (registers_t*)this->esp;
    memset(regs, 0, sizeof(registers_t));

    regs->eip = (uint32_t)process_wrapper;

    regs->cs = 0x08;
    regs->eflags = 0x0202;
    regs->ds = 0x10;
    regs->es = 0x10;
    regs->fs = 0x10;
    regs->gs = 0x10;
}

void Process::save(registers_t* r)
{
    this->esp = (uintptr_t)r;
}

uintptr_t Process::load()
{
    return this->esp;
}

namespace Scheduler {
Process* head = nullptr;
Process* tail = nullptr;
Process* current_process = nullptr;
Process* process_to_delete = nullptr;
int process_count = 0;
int pid = 0;
Spinlock removeLock;

void add_process(Program prog, const char* name, int argc, char* argv[])
{
    uint8_t* new_stack = new uint8_t[STACK_SIZE];
    if (new_stack == nullptr) {
        PANIC("Plus de place dans la memoire pour la stack dun nouveau proceessus");
    }

    Process* new_proc = new Process();
    if (new_proc == nullptr) {
        delete[] new_stack;
        PANIC("Plus de placee dans la mémoire pour un nouveeau proceessus");
    }

    new_proc->id = process_count;
    new_proc->stack = new_stack;
    new_proc->state = State::RUNNING;
    new_proc->pid = ++pid;
    new_proc->next = nullptr;
    new_proc->init(prog, argc, argv, name);

    if (head == nullptr) {
        head = new_proc;
        tail = new_proc;
    } else {
        tail->next = new_proc;
        tail = new_proc;
    }
    process_count++;
}

void remove_process(Process* proc)
{
    LockGuard g(removeLock);

    if (proc == nullptr)
        return;

    if (proc->stack != nullptr) {
        delete[] proc->stack;
        proc->stack = nullptr;
    }

    if (head == proc) {
        head = proc->next;
        if (tail == proc) {
            tail = nullptr;
        }
    } else {
        Process* prev = head;
        while (prev != nullptr && prev->next != proc) {
            prev = prev->next;
        }
        if (prev != nullptr) {
            prev->next = proc->next;
            if (tail == proc) {
                tail = prev;
            }
        }
    }

    delete proc;
    process_count--;
}

uintptr_t schedule(registers_t* regs)
{
    if (process_to_delete != nullptr) {
        Process* to_delete = process_to_delete;
        process_to_delete = nullptr;

        if (current_process == to_delete) {
            current_process = to_delete->next ? to_delete->next : head;
        }
        remove_process(to_delete);
    }

    if (current_process != nullptr) {
        if (current_process->state == State::FINISHED) {
            process_to_delete = current_process;
        } else {
            current_process->save(regs);
        }
    }

    if (process_count == 0) {
        PANIC("Plus de processus ?????? Comment ??");
    }

    if (current_process == nullptr) {
        current_process = head;
    } else {
        current_process = current_process->next ? current_process->next : head;
    }

    while (current_process->state == State::FINISHED) {
        process_to_delete = current_process;
        Process* next = current_process->next ? current_process->next : head;
        if (process_to_delete != nullptr) {
            Process* to_delete = process_to_delete;
            process_to_delete = nullptr;

            if (current_process == to_delete) {
                current_process = to_delete->next ? to_delete->next : head;
            }
            remove_process(to_delete);
        }
        current_process = next;

        if (process_count == 0) {
            PANIC("Plus de processus ?????? Comment ??");
        }
    }

    return current_process->load();
}

void kill_current_process()
{
    if (current_process != nullptr) {
        current_process->state = State::FINISHED;
    }
}

bool is_process_running()
{
    return current_process != nullptr;
}

const Process* get_current_process()
{
    return current_process;
}

Process* get_process_list()
{
    return head;
}
}

void process_wrapper()
{
    Process* proc = Scheduler::current_process;
    Program entry = (Program)proc->entry_point;
    entry(proc->argc, proc->argv);
    proc->state = State::FINISHED;

    asm volatile("int $0x20");
}
