#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "command.h"
#include "file_io.h"

static const char* g_csv_path = "students.csv";

void set_csv_path(const char* path) {
    if (path != NULL) {
        g_csv_path = path;
    }
}

ShellResult handle_help(char* args, Student** head);

ShellResult handle_exit(char* args, Student** head) {
    (void) args; (void) head;
    printf("Goodbye.\n");
    return SHELL_EXIT;
}

ShellResult handle_clear(char* args, Student** head) {
    (void) args; (void)head;
    printf("\033[2J\033[H");
    return SHELL_OK;
}

ShellResult handle_list(char* args, Student** head) {
    (void) args;
    list_students(*head);
    return SHELL_OK;
}

ShellResult handle_stats(char* args, Student** head) {
    (void) args;
    print_stats(*head);
    return SHELL_OK;
}

ShellResult handle_find(char* args, Student** head) {
    if (args == NULL) {
        printf("Error: missing argument.\n");
        return SHELL_ERR_MISSING_ARGUMENT;
    }
    int id = atoi(args);
    Student* student = find_student(*head, id);
    if (student != NULL) {
        printf("ID: %d\nName: %s\nScore: %d\n", student->id, student->name, student->score);
    } else {
        printf("Error: student not found.\n");
    }
    return SHELL_OK;
}

ShellResult handle_reload(char* args, Student** head) {
    (void) args;
    free_all_students(head);
    int count = load_students_from_csv(g_csv_path, head);
    if (count >= 0) {
        printf("Reloaded %d students from %s.\n", count, g_csv_path);
    }
    return SHELL_OK;
}

ShellResult handle_save(char* args, Student** head) {
    (void) args;
    int count = save_students_to_csv(g_csv_path, *head);
    if (count >= 0) {
        printf("Saved %d students to %s.\n", count, g_csv_path);
    }
    return SHELL_OK;
}

ShellResult handle_add(char* args, Student** head){
    if (args == NULL) {
        printf("Error: missing arguments.\n");
        return SHELL_ERR_MISSING_ARGUMENT;
    }

    char* id_str = strtok(args, " \t");
    char* name_str = strtok(NULL, " \t");
    char* score_str = strtok(NULL, " \t");

    if (id_str == NULL || name_str == NULL || score_str == NULL) {
        printf("Error: missing arguments.\n");
        return SHELL_ERR_MISSING_ARGUMENT;
    }

    int id = atoi(id_str);
    int score = atoi(score_str);

    if (score<0 || score>100) {
        printf("Error: invalid score.\n");
        return SHELL_ERR_INVALID_ARGUMENT;
    }
    if (find_student(*head, id) != NULL) {
        printf("Error: duplicate ID.\n");
        return SHELL_ERR_DUPLICATE_STUDENT;
    }

    add_student(head, id, name_str, score);
    printf("Student added.\n");
    return SHELL_OK;
}

ShellResult handle_delete(char* args, Student** head) {
    if (args == NULL) {
        printf("Error: missing arguments.\n");
        return SHELL_ERR_MISSING_ARGUMENT;
    }
    int id = atoi(args);
    delete_student(head, id);
    return SHELL_OK;
}

ShellResult handle_update(char* args, Student** head){
    if (args == NULL) {
        printf("Error: missing arguments.\n");
        return SHELL_ERR_MISSING_ARGUMENT;
    }
    char* id_str = strtok(args, " \t");
    char* score_str = strtok (NULL, " \t");

    if (id_str == NULL || score_str == NULL) {
        printf("Error: missing arguments. \n");
        return SHELL_ERR_MISSING_ARGUMENT;
    }

    int id = atoi(id_str);
    int score = atoi(score_str);

    if (score < 0 || score > 100) {
        printf("Error: invalid score.\n");
        return SHELL_ERR_INVALID_SCORE;
    }

    update_student(*head, id, score);
    return SHELL_OK;
}

#ifdef ADMIN_MODE

Command commands[] = {
    {"svae", handle_save, "save", "Save students to CSV"},
    {"reload", handle_reload, "reload", "Reload students from CSV"},
    {"add", handle_add, "add <id> <name> <score>", "Add a student"},
    {"delete", handle_delete, "delete <id>", "Delete a student"},
    {"update", handle_update, "update <id> <score>", "Update student score"},
    {"find", handle_find, "find <id>", "Find student"},
    {"list", handle_list, "list", "List all students"},
    {"stats", handle_stats, "stats", "Show statistics"},
    {"help", handle_help, "help", "Show help"},
    {"clear", handle_clear, "clear", "Clear screen"},
    {"exit", handle_exit, "exit", "Exit shell"}
};

#elif defined(CLIENT_MODE)

Command commands[] = {
    {"reload", handle_reload, "reload", "Reload students from CSV"}, [cite: 302, 309]
    {"find", handle_find, "find <id>", "Find student"}, [cite: 310, 311, 312, 313]
    {"list", handle_list, "list", "List students"}, [cite: 314, 315, 316, 317]
    {"stats", handle_stats, "stats", "Show statistics"}, [cite: 318, 319, 320]
    {"help", handle_help, "help", "Show help"}, [cite: 321, 322, 323, 324]
    {"clear", handle_clear, "clear", "Clear screen"}, [cite: 326, 327]
    {"exit", handle_exit, "exit", "Exit shell"} [cite: 326, 328]
};

#endif

#define NUM_COMMANDS (sizeof(commands)/sizeof(Command))

ShellResult handle_help(char* args, Student** head) {
    (void)args; (void)head;
    printf("Commnads:\n");
    for (size_t i = 0; i < NUM_COMMANDS; i++) {
        printf("  %-25s %s\n", commands[i].usage, commands[i].description);
    }
    return SHELL_OK;
}

ShellResult execute_command(char* input, Student** head) {
    input[strcspn(input, "\r\n")] = '\0';
    if (strlen(input) == 0) return SHELL_OK;

    char* cmd_name = strtok(input, " \t");
    char* args = strtok(NULL, "");

    if (cmd_name == NULL) return SHELL_OK;

    for (size_t i = 0; i < NUM_COMMANDS; i++) {
        if (strcmp(cmd_name, commands[i].name) == 0) {
            return commands[i].handler(args, head);
        }
    }

    printf("Unknown command or permission denied.\n");
    return SHELL_ERR_UNKNOWN_COMMAND;
}