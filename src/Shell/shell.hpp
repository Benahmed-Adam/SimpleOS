#pragma once
#include "utils.hpp"

typedef bool (*CommandFunction)(char** args);

struct Command {
    const char* name;
    const char* description;
    CommandFunction execute;
};

bool cmd_help(char** args);
bool cmd_clear(char** args);
bool cmd_hello(char** args);
bool cmd_info(char** args);
bool cmd_uptime(char** args);
bool cmd_memory(char** args);
//bool cmd_ls(char** args);
//bool cmd_cat(char** args);
//bool cmd_touch(char** args);
bool cmd_color(char** args);
bool cmd_echo(char** args);
bool cmd_banner(char** args);
bool cmd_reboot(char** args);
bool cmd_shutdown(char** args);
bool cmd_ps(char** args);
bool cmd_kill(char** args);

void executeCommand(char* cmd, char** args);

int shell(int, char**);