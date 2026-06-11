/*
 * main.c  –  Mini Student Shell
 *
 * TODO: Implement admin_shell and client_shell.
 *
 * Build:
 *   make admin   →  admin_shell  (compiled with -DADMIN_MODE)
 *   make client  →  client_shell (compiled with -DCLIENT_MODE)
 *
 * Usage:
 *   ./admin_shell [students.csv]
 *   ./admin_shell -f commands.txt [students.csv]
 *   ./client_shell [students.csv]
 *   ./client_shell -f commands.txt [students.csv]
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* TODO: Add your own header includes here */
#include "student.h"
#include "file_io.h"
#include "command.h"

#ifdef ADMIN_MODE
#define PROMPT "admin> "
#elif defined(CLIENT_MODE)
#define PROMPT "client> "
#else
#define PROMPT "shell> "
#endif

static Student* g_head = NULL;
/* ---------------------------------------------------------------
 * TODO: Implement the interactive shell loop.
 *   - Print a prompt and read a line from stdin.
 *   - Parse the line into a command and arguments.
 *   - Dispatch to the appropriate handler function.
 *   - Loop until the user types "exit" or EOF.
 * --------------------------------------------------------------- */
void run_shell(const char *csv_path) {
    /* TODO */
    char input[256];
    (void)csv_path;

    while(1) {
        printf("%s", PROMPT);
        if (fgets(input, sizeof(input), stdin) == NULL) break;

        ShellResult res = execute_command(input, &g_head);
        if (res == SHELL_EXIT) break;
    }
}

/* ---------------------------------------------------------------
 * TODO: Implement batch mode – read commands from a file.
 *   - Open cmd_file for reading.
 *   - Execute each line as a command (same logic as run_shell).
 *   - Close the file when done.
 * --------------------------------------------------------------- */
void run_command_file(const char *cmd_file, const char *csv_path) {
    /* TODO */
    FILE* file = fopen(cmd_file, "r");
    if (file == NULL) {
        printf("Error: Cannot open command file.\n");
        run_shell(csv_path);
        return;
    }

    char line[256];
    int line_num = 0;

    while (fgets(line, sizeof(line), file) != NULL) {
        char original_line[256];
        strncpy(original_line, line, sizeof(original_line));
        original_line[strcspn(original_line, "\r\n")] = '\0';

        char* trimmed = line;
        while (*trimmed == ' ' || *trimmed == '\t') trimmed++;
        if (strlen(trimmed) == 0 || *trimmed == '\n' || *trimmed == '\r') continue;
        if (*trimmed == '#') continue;

        line_num++;

        printf("[command file:%d] %s\n", line_num, original_line);

        ShellResult res = execute_command(line, &g_head);

        if (res == SHELL_EXIT) {
            break;
        } else if (res != SHELL_OK && res != SHELL_ERR_UNKNOWN_COMMAND) {
            printf("Skipped line %d.\n", line_num);
        }
    }
    fclose(file);
}

int main(int argc, char *argv[]) {
    const char *csv_path  = "students.csv"; /* default CSV file */
    const char *cmd_file  = NULL;           /* -f <file> argument */
    int csv_file_provided = 0;

    /* TODO: Parse command-line arguments.
     *   Supported flags:
     *     -f <file>   run commands from <file> instead of stdin
     *   Remaining positional argument (if any): path to students CSV.
     *
     *   Example parsing skeleton:
     *
     *   for (int i = 1; i < argc; i++) {
     *       if (strcmp(argv[i], "-f") == 0 && i + 1 < argc) {
     *           cmd_file = argv[++i];
     *       } else {
     *           csv_path = argv[i];
     *       }
     *   }
     */
    
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-f") == 0 && i + 1 < argc) {
            cmd_file = argv[++i];
        }
        else {
            csv_path = argv[i];
            csv_file_provided = 1;
        }
    }

    if (argc<2 || !csv_file_provided) {
#ifdef ADMIN_MODE
        printf("Usage: ./admin_shell <csv_file> [-f command_file]\n");
#else
        printf("Usage: ./client_shell <csv_file> [-f command_file]\n");
#endif
        return 1;
    }

    set_csv_path(csv_path);

    int count = load_students_from_csv(csv_path, &g_head);
    if (count >= 0) {
#ifdef ADMIN_MODE
        printf("[Admin Program]\n");
#elif defined(CLIENT_MODE)
        printf("[Client Program]\n");
#endif
        printf("Loaded %d students from %s.\n", count, csv_path);
    }

#ifdef ADMIN_MODE
    /* Admin shell: supports add, delete, update, save, load, sort, list, find, help, exit */
    if (cmd_file) {
        run_command_file(cmd_file, csv_path);
    } else {
        run_shell(csv_path);
    }

#elif defined(CLIENT_MODE)
    /* Client shell: supports find, list, help, exit  (read-only) */
    if (cmd_file) {
        run_command_file(cmd_file, csv_path);
    } else {
        run_shell(csv_path);
    }

#else
#error "Define either -DADMIN_MODE or -DCLIENT_MODE when compiling."
#endif

    free_all_students(&g_head);
    return 0;
}
