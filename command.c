#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "command.h"
#include "file_io.h"

static const char* g_csv_path = "students.csv";
static int g_unsaved_changes = 0;

void set_csv_path(const char* path) {
    if (path != NULL) {
        g_csv_path = path;
    }
}

int is_valid_number(const char* str) {
    if (str == NULL || *str == '\0') return 0;
    int i = 0;
    if (str[0] == '-') i = 1;
    if (str[i] == '\0') return 0;
    while (str[i] != '\0') {
        if (str[i] < '0' || str[i] > '9') return 0;
        i++;
    }
    return 1;
}

ShellResult handle_help(char* args, Student** head);

ShellResult handle_exit(char* args, Student** head) {
    (void) args; (void) head;

    if (g_unsaved_changes) {
        printf("Warning: You have unsaved changes. (Use 'save' to save them)\n");
    }

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
        return SHELL_OK;
    } else {
        printf("Error: student not found.\n");
        return SHELL_ERR_STUDENT_NOT_FOUND;
    }
}

ShellResult handle_reload(char* args, Student** head) {
    (void) args;
    free_all_students(head);
    int count = load_students_from_csv(g_csv_path, head);
    if (count >= 0) {
        printf("Reloaded %d students from %s.\n", count, g_csv_path);
    }
    g_unsaved_changes = 0;
    return SHELL_OK;
}

ShellResult handle_save(char* args, Student** head) {
    (void) args;
    int count = save_students_to_csv(g_csv_path, *head);
    if (count >= 0) {
        printf("Saved %d students to %s.\n", count, g_csv_path);
    }
    g_unsaved_changes = 0;
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

    if (id_str == NULL || name_str == NULL || score_str == NULL) {
        printf("Error: missing arguments.\n");
        return SHELL_ERR_MISSING_ARGUMENT;
    }

    if (!is_valid_number(id_str) || atoi(id_str) <= 0) {
        printf("Error: invalid argument.\n");
        return SHELL_ERR_INVALID_ARGUMENT;
    }

    if (!is_valid_number(score_str) || atoi(score_str) < 0 || atoi(score_str) > 100) {
        printf("Error: invalid score.\n");
        return SHELL_ERR_INVALID_SCORE;
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
    g_unsaved_changes = 1;
    return SHELL_OK;
}

ShellResult handle_delete(char* args, Student** head) {
    if (args == NULL) {
        printf("Error: missing arguments.\n");
        return SHELL_ERR_MISSING_ARGUMENT;
    }
    int id = atoi(args);
    if (find_student(*head, id) == NULL) {
        printf("Error: student not found.\n");
        return SHELL_ERR_STUDENT_NOT_FOUND;
    }
    delete_student(head, id);
    g_unsaved_changes = 1;
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

    if (!is_valid_number(id_str) || atoi(id_str) <= 0) {
        printf("Error: invalid argument.\n");
        return SHELL_ERR_INVALID_ARGUMENT;
    }

    if (!is_valid_number(score_str) || atoi(score_str) < 0 || atoi(score_str) > 100) {
        printf("Error: invalid score.\n");
        return SHELL_ERR_INVALID_SCORE;
    }

    int id = atoi(id_str);
    int score = atoi(score_str);

    if (score < 0 || score > 100) {
        printf("Error: invalid score.\n");
        return SHELL_ERR_INVALID_SCORE;
    }

    if (find_student(*head, id) == NULL) {
        printf("Error: student not found.\n");
        return SHELL_ERR_STUDENT_NOT_FOUND; 
    }
    update_student(*head, id, score);
    g_unsaved_changes = 1;
    return SHELL_OK;
}

ShellResult handle_sort(char* args, Student** head) {
    if (args == NULL) {
        printf("Error: missing arguments.\n");
        return SHELL_ERR_MISSING_ARGUMENT;
    }

    char* key = strtok(args, " \t");
    if (key == NULL) {
        printf("Error: missing sort key.\n");
        return SHELL_ERR_MISSING_ARGUMENT;
    }

    if (strcmp(key, "name") == 0) {
        sort_students(head, "name");
        printf("List sorted by name.\n");
    } else if (strcmp(key, "score") == 0) {
        sort_students(head, "score");
        printf("List sorted by score.\n");
    } else {
        printf("Error: invalid sort key.\n");
    }
    g_unsaved_changes = 1;
    return SHELL_OK;
}

#ifdef ADMIN_MODE

Command commands[] = {
    {"save", handle_save, "save", "Save students to CSV"},
    {"reload", handle_reload, "reload", "Reload students from CSV"},
    {"add", handle_add, "add <id> <name> <score>", "Add a student"},
    {"delete", handle_delete, "delete <id>", "Delete a student"},
    {"update", handle_update, "update <id> <score>", "Update student score"},
    {"find", handle_find, "find <id>", "Find student"},
    {"list", handle_list, "list", "List all students"},
    {"stats", handle_stats, "stats", "Show statistics"},
    {"help", handle_help, "help", "Show help"},
    {"clear", handle_clear, "clear", "Clear screen"},
    {"sort", handle_sort, "sort <name|score>", "Sort students"},
    {"exit", handle_exit, "exit", "Exit shell"}
};

#elif defined(CLIENT_MODE)

Command commands[] = {
    {"reload", handle_reload, "reload", "Reload students from CSV"},
    {"find", handle_find, "find <id>", "Find student"},
    {"list", handle_list, "list", "List students"},
    {"stats", handle_stats, "stats", "Show statistics"},
    {"help", handle_help, "help", "Show help"},
    {"clear", handle_clear, "clear", "Clear screen"},
    {"exit", handle_exit, "exit", "Exit shell"}
};

#endif

#define NUM_COMMANDS (sizeof(commands)/sizeof(Command))

ShellResult handle_help(char* args, Student** head) {
    (void)args; (void)head;
    printf("Commands:\n");
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