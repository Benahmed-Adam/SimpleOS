#include "shell.hpp"
#include "vga.hpp"
#include "input.hpp"
#include "utils.hpp"
#include "memory.hpp"
#include "audio.hpp"
#include "time.hpp"
#include "process.hpp"

#define BUFFER_SIZE 1024
#define MAX_ARGS 16 

VGA::Color stringToColor(const char* str) {
    if (strcmp(str, "black") == 0)    return VGA::BLACK;
    if (strcmp(str, "blue") == 0)     return VGA::BLUE;
    if (strcmp(str, "green") == 0)    return VGA::GREEN;
    if (strcmp(str, "cyan") == 0)     return VGA::CYAN;
    if (strcmp(str, "red") == 0)      return VGA::RED;
    if (strcmp(str, "magenta") == 0)  return VGA::MAGENTA;
    if (strcmp(str, "brown") == 0)    return VGA::BROWN;
    if (strcmp(str, "lgrey") == 0)    return VGA::LIGHT_GREY;
    if (strcmp(str, "dgrey") == 0)    return VGA::DARK_GREY;
    if (strcmp(str, "lblue") == 0)    return VGA::LIGHT_BLUE;
    if (strcmp(str, "lgreen") == 0)   return VGA::LIGHT_GREEN;
    if (strcmp(str, "lcyan") == 0)    return VGA::LIGHT_CYAN;
    if (strcmp(str, "lred") == 0)     return VGA::LIGHT_RED;
    if (strcmp(str, "lmagenta") == 0) return VGA::LIGHT_MAGENTA;
    if (strcmp(str, "yellow") == 0)   return VGA::YELLOW;
    if (strcmp(str, "white") == 0)    return VGA::WHITE;
    
    return (VGA::Color)255;
}

bool cmd_clear(char**) {
    VGA::clear();
    return true;
}

bool cmd_hello(char**) {
    VGA::println("Salutation !");
    return true;
}

bool cmd_info(char**) {
    VGA::println("Infos systeme : blablabla");
    return true;
}

bool cmd_memory(char** args) {
    if (args[1] == nullptr) {
        VGA::println("Mauvais usage ! Un parametre est requis");
        return false;
    }

    char* arg = args[1];
    if (strcmp(arg, "abmem") == 0) {
        print_memory_blocks();
    } else if (strcmp(arg, "amem") == 0) {
        VGA::println("Total memoire disponoble : ", get_avalaible_memory_in_mb(), " MB");
    } else if (strcmp(arg, "tmem") == 0) {
        VGA::println("Total memoire : ", get_total_memory_in_mb(), " MB");
    } else {
        VGA::println("Mauvais usage ! Un parametre valide est requis");
        return false;
    }

    return true;
}

bool cmd_color(char** args) {
    if (args[1] == nullptr || args[2] == nullptr) {
        VGA::println("Mauvais usage! color <foreground> <background>");
        VGA::println("Ex: color yellow blue");
        return false;
    }

    VGA::Color fg = stringToColor(args[1]);
    VGA::Color bg = stringToColor(args[2]);

    if (fg == 255 || bg == 255) {
        VGA::println("Couleur invalide.");
        return false;
    }

    if (fg == bg) {
        VGA::println("La police et le fond doivent etre differents.");
        return false;
    }

    VGA::set_color(fg, bg);
    VGA::refresh();

    return true;
}

bool cmd_echo(char** args) {
    for (size_t i = 1; args[i] != nullptr; i++) {
        VGA::print(args[i]);
        if (args[i+1]) VGA::print(" ");
    }
    VGA::println();
    return true;
}

bool cmd_banner(char**) {
    VGA::println(" _______  _______  _______  _______  _______  _______ ");
    VGA::println("|       ||       ||       ||       ||       ||       |");
    VGA::println("|   _   ||  _____||_     _||    ___||  _____||_     _|");
    VGA::println("|  | |  || |_____   |   |  |   |___ | |_____   |   |  ");
    VGA::println("|  |_|  ||_____  |  |   |  |    ___||_____  |  |   |  ");
    VGA::println("|       | _____| |  |   |  |   |___  _____| |  |   |  ");
    VGA::println("|_______||_______|  |___|  |_______||_______|  |___|  ");
    VGA::println("\n---------------------- Par Moi ---------------------");

    return true;
}

bool cmd_music(char**) {
    play_music();
    return true;
}

bool cmd_reboot(char**) {
    outb(0xCF9, 0x06);
    return true;
}

bool cmd_shutdown(char**) {
    outw(0x604, 0x2000);
    return true;
}

bool cmd_uptime(char**) {
    VGA::println(uptime()," secondes");
    return true;
}

bool cmd_ps(char**) {
    Process* p = Scheduler::get_process_list();

    while (p != nullptr) {
        VGA::println("PID : ", p->pid, " Nom : ", p->name);
        p = p->next;
    }

    return true;
}

bool cmd_kill(char** args) {
    if (args[1] == nullptr) {
        VGA::println("Mauvais usage! kill <pid>");
        return false;
    }

    Process* p = Scheduler::get_process_list();

    while (p != nullptr && p->pid != atoi(args[1])) {
        p = p->next;
    }

    if (p != nullptr) {
        Scheduler::remove_process(p);
        VGA::println("Le processus : ", args[1], " a bien ete elimine");
    } else {
        VGA::println("Processus inexistant");
    }

    return true;
}

Command commandList[] = {
    {"help",  "Affiche le aide ou le aide de une commande (help <cmd>)", cmd_help},
    {"clear", "Efface le ecran", cmd_clear},
    {"hello", "Affiche un message gentil", cmd_hello},
    {"info", "Affiche les infos du pc", cmd_info},
    {"memory","Affiche les infos de la memoire (memory <abmem | amem | tmem>)", cmd_memory},
    {"color", "Change la couleur des caracteres et de fond\n Couleurs disponible : \n- black\n- blue\n- green\n- cyan\n- red\n- magenta\n- brown\n- lgrey\n- dgrey\n- lblue\n- lgreen\n- lcyan\n- lred\n- lmagenta\n- yellow\n- white", cmd_color},
    {"echo", "Affiche tout ce qui est passe en parametre", cmd_echo},
    {"banner", "Affiche la banniere de l os", cmd_banner},
    {"shutdown", "Eteind le pc", cmd_shutdown},
    {"reboot", "Redemarre le pc", cmd_reboot},
    {"music", "Joue de la musique", cmd_music},
    {"uptime", "Uptime du pc", cmd_uptime},
    {"ps", "Affiche tout les processus en cours", cmd_ps},
    {"kill", "Assasine violement un processus (kill <pid>)", cmd_kill}
};

const int numCommands = sizeof(commandList) / sizeof(Command);

bool cmd_help(char** args) {
    if (args[1] != nullptr) {
        for (int i = 0; i < numCommands; i++) {
            if (strcmp(args[1], commandList[i].name) == 0) {
                VGA::println(commandList[i].name, " : ", commandList[i].description);
                return true;
            }
        }
        VGA::println("Commande inconnue pour le aide.");
    } else {
        VGA::println("Commandes disponibles :");
        for (int i = 0; i < numCommands; i++) {
            VGA::println("- ", commandList[i].name);
        }
    }
    return true;
}

void executeCommand(char* cmd, char** args) {
    if (strlen(cmd) == 0) return;

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

    VGA::print("Commande inconnue: ");
    VGA::println(cmd);
}

void get_cmd_args(char buffer[BUFFER_SIZE], char *&cmd, char *args[MAX_ARGS])
{
    int argCount = 0;
    char *ptr = buffer;
    while (*ptr != '\0' && argCount < MAX_ARGS)
    {
        while (*ptr == ' ')
            ptr++;
        if (*ptr == '\0')
            break;

        if (argCount == 0)
            cmd = ptr;
        args[argCount++] = ptr;

        while (*ptr != ' ' && *ptr != '\0')
            ptr++;

        if (*ptr != '\0')
        {
            *ptr = '\0';
            ptr++;
        }
    }
}

int shell(int, char**) {
    char buffer[BUFFER_SIZE];
    size_t idx = 0;

    VGA::print(">");

    while (true) {
        uint8_t scancode = PS2::getKeyPressed();

        if (scancode > 0) {
            if (scancode == PS2::KEY_ENTER) {
                VGA::println();
                buffer[idx] = '\0';
                char* args[MAX_ARGS] = {0};
                char* cmd = nullptr;

                get_cmd_args(buffer, cmd, args);

                if (cmd != nullptr) {
                    executeCommand(cmd, args);
                }
                
                idx = 0;
                VGA::print("> ");
            } 
            else if (scancode == PS2::KEY_BACKSPACE) {
                if (idx > 0) {
                    idx--;
                    VGA::backspace();
                }
            }
            else {
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
