#include "Shell/shell.hpp"
#include "Audio/audio.hpp"
#include "Input/input.hpp"
#include "Memory/memory.hpp"
#include "Process/process.hpp"
#include "Time/time.hpp"
#include "VGA/vga.hpp"
#include "utils.hpp"

#define BUFFER_SIZE 1024
#define MAX_ARGS 16

VGA::Color stringToColor(const char* str)
{
    if (strcmp(str, "black") == 0)
        return VGA::BLACK;
    if (strcmp(str, "blue") == 0)
        return VGA::BLUE;
    if (strcmp(str, "green") == 0)
        return VGA::GREEN;
    if (strcmp(str, "cyan") == 0)
        return VGA::CYAN;
    if (strcmp(str, "red") == 0)
        return VGA::RED;
    if (strcmp(str, "magenta") == 0)
        return VGA::MAGENTA;
    if (strcmp(str, "brown") == 0)
        return VGA::BROWN;
    if (strcmp(str, "lgrey") == 0)
        return VGA::LIGHT_GREY;
    if (strcmp(str, "dgrey") == 0)
        return VGA::DARK_GREY;
    if (strcmp(str, "lblue") == 0)
        return VGA::LIGHT_BLUE;
    if (strcmp(str, "lgreen") == 0)
        return VGA::LIGHT_GREEN;
    if (strcmp(str, "lcyan") == 0)
        return VGA::LIGHT_CYAN;
    if (strcmp(str, "lred") == 0)
        return VGA::LIGHT_RED;
    if (strcmp(str, "lmagenta") == 0)
        return VGA::LIGHT_MAGENTA;
    if (strcmp(str, "yellow") == 0)
        return VGA::YELLOW;
    if (strcmp(str, "white") == 0)
        return VGA::WHITE;

    return (VGA::Color)255;
}

bool cmd_clear(char**)
{
    VGA::clear();
    return true;
}

bool cmd_hello(char**)
{
    VGA::println("Greeting !");
    return true;
}

bool cmd_info(char**)
{
    VGA::println("System info : blablabla");
    return true;
}

bool cmd_memory(char** args)
{
    if (args[1] == nullptr) {
        VGA::println("Bad usage! A parameter is required");
        return false;
    }

    char* arg = args[1];
    if (strcmp(arg, "abmem") == 0) {
        print_memory_blocks();
    } else if (strcmp(arg, "amem") == 0) {
        VGA::println("Total available memory: ", get_avalaible_memory_in_mb(), " MB");
    } else if (strcmp(arg, "tmem") == 0) {
        VGA::println("Total memory: ", get_total_memory_in_mb(), " MB");
    } else {
        VGA::println("Bad usage! A valid parameter is required");
        return false;
    }

    return true;
}

bool cmd_color(char** args)
{
    if (args[1] == nullptr || args[2] == nullptr) {
        VGA::println("Bad usage! color <foreground> <background>");
        VGA::println("Ex: color yellow blue");
        return false;
    }

    VGA::Color fg = stringToColor(args[1]);
    VGA::Color bg = stringToColor(args[2]);

    if (fg == 255 || bg == 255) {
        VGA::println("Invalid color.");
        return false;
    }

    if (fg == bg) {
        VGA::println("Foreground and background must be different.");
        return false;
    }

    VGA::set_color(fg, bg);
    VGA::refresh();

    return true;
}

bool cmd_echo(char** args)
{
    for (size_t i = 1; args[i] != nullptr; i++) {
        VGA::print(args[i]);
        if (args[i + 1])
            VGA::print(" ");
    }
    VGA::println();
    return true;
}

bool cmd_banner(char**)
{
    VGA::println(" _______  _______  _______  _______  _______  _______ ");
    VGA::println("|       ||       ||       ||       ||       ||       |");
    VGA::println("|   _   ||  _____||_     _||    ___||  _____||_     _|");
    VGA::println("|  | |  || |_____   |   |  |   |___ | |_____   |   |  ");
    VGA::println("|  |_|  ||_____  |  |   |  |    ___||_____  |  |   |  ");
    VGA::println("|       | _____| |  |   |  |   |___  _____| |  |   |  ");
    VGA::println("|_______||_______|  |___|  |_______||_______|  |___|  ");

    return true;
}

bool cmd_music(char**)
{
    play_music();
    return true;
}

bool cmd_reboot(char**)
{
    outb(0xCF9, 0x06);
    return true;
}

bool cmd_shutdown(char**)
{
    outw(0x604, 0x2000);
    return true;
}

bool cmd_uptime(char**)
{
    VGA::println(uptime(), " seconds");
    return true;
}

bool cmd_ps(char**)
{
    Process* p = Scheduler::get_process_list();

    while (p != nullptr) {
        VGA::println("PID: ", p->pid, " Name: ", p->name);
        p = p->next;
    }

    return true;
}

bool cmd_kill(char** args)
{
    if (args[1] == nullptr) {
        VGA::println("Bad usage! kill <pid>");
        return false;
    }

    Process* p = Scheduler::get_process_list();

    while (p != nullptr && p->pid != (uint32_t)atoi(args[1])) {
        p = p->next;
    }

    if (p != nullptr) {
        Scheduler::remove_process(p);
        VGA::println("Process: ", args[1], " has been killed");
    } else {
        VGA::println("Nonexistent process");
    }

    return true;
}

Command commandList[] = {
    { "help", "Display help or help for a command (help <cmd>)", cmd_help },
    { "clear", "Clear the screen", cmd_clear },
    { "hello", "Display a friendly message", cmd_hello },
    { "info", "Display pc info", cmd_info },
    { "memory", "Display memory info (memory <abmem | amem | tmem>)", cmd_memory },
    { "color", "Change foreground and background color\n Available colors: \n- black\n- blue\n- green\n- cyan\n- red\n- magenta\n- brown\n- lgrey\n- dgrey\n- lblue\n- lgreen\n- lcyan\n- lred\n- lmagenta\n- yellow\n- white", cmd_color },
    { "echo", "Display everything passed as parameter", cmd_echo },
    { "banner", "Display the os banner", cmd_banner },
    { "shutdown", "Turn off the pc", cmd_shutdown },
    { "reboot", "Reboot the pc", cmd_reboot },
    { "music", "Play music", cmd_music },
    { "uptime", "Pc uptime", cmd_uptime },
    { "ps", "Display all running processes", cmd_ps },
    { "kill", "Violently kill a process (kill <pid>)", cmd_kill }
};

const int numCommands = sizeof(commandList) / sizeof(Command);

bool cmd_help(char** args)
{
    if (args[1] != nullptr) {
        for (int i = 0; i < numCommands; i++) {
            if (strcmp(args[1], commandList[i].name) == 0) {
                VGA::println(commandList[i].name, " : ", commandList[i].description);
                return true;
            }
        }
        VGA::println("Unknown command for help.");
    } else {
        VGA::println("Available commands:");
        for (int i = 0; i < numCommands; i++) {
            VGA::println("- ", commandList[i].name);
        }
    }
    return true;
}

void executeCommand(char* cmd, char** args)
{
    if (strlen(cmd) == 0)
        return;

    for (int i = 0; i < numCommands; i++) {
        if (strcmp(cmd, commandList[i].name) == 0) {
            if (args[1] != nullptr && strcmp(args[1], "help") == 0) {
                VGA::println(commandList[i].description);
            } else {
                commandList[i].execute(args);
            }
            return;
        }
    }

    VGA::print("Unknown command: ");
    VGA::println(cmd);
}

void get_cmd_args(char buffer[BUFFER_SIZE], char*& cmd, char* args[MAX_ARGS])
{
    int argCount = 0;
    char* ptr = buffer;
    while (*ptr != '\0' && argCount < MAX_ARGS) {
        while (*ptr == ' ')
            ptr++;
        if (*ptr == '\0')
            break;

        if (argCount == 0)
            cmd = ptr;
        args[argCount++] = ptr;

        while (*ptr != ' ' && *ptr != '\0')
            ptr++;

        if (*ptr != '\0') {
            *ptr = '\0';
            ptr++;
        }
    }
}

int shell(int, char**)
{
    char buffer[BUFFER_SIZE];
    size_t idx = 0;

    VGA::print(">");

    while (true) {
        uint8_t scancode = PS2::getKeyPressed();

        if (scancode > 0) {
            if (scancode == PS2::KEY_ENTER) {
                VGA::println();
                buffer[idx] = '\0';
                char* args[MAX_ARGS] = { 0 };
                char* cmd = nullptr;

                get_cmd_args(buffer, cmd, args);

                if (cmd != nullptr) {
                    executeCommand(cmd, args);
                }

                idx = 0;
                VGA::print("> ");
            } else if (scancode == PS2::KEY_BACKSPACE) {
                if (idx > 0) {
                    idx--;
                    VGA::backspace();
                }
            } else {
                char key = PS2::toAscii(scancode);
                if (key > 0 && idx < BUFFER_SIZE) {
                    buffer[idx++] = key;
                    VGA::print(key);
                }
            }
        }
    }

    return 1;
}
