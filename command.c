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
        printF("ID: %d\nName: %s\nScore: %d\n", student->id, student->name, student->score);
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
        printf("Reloaded %d students from %s.\n", count, g_csv_path)
    }
    return SHELL_OK;
}