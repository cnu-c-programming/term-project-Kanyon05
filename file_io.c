#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file_io.h"
#include "student.h"

int load_students_from_csv(const char* filename, Student** head) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) return 0;
    
    char line[256];
    int count = 0;

    if (fgets(line, sizeof(line), file) != NULL) {
        if(strstr(line, "id") == NULL || strstr(line, "name") == NULL || strstr(line, "score")==NULL) {
            printf("Error: Invalid CSV header.\n");
            fclose(file);
            return -1;
        }
    } else {
        fclose(file);
        return 0;
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        line[strcspn(line, "\r\n")] = 0;
        if (strlen(line) == 0) continue;

        char* token = strtok(line, ",");
        if (token == NULL) continue;
        int id = atoi(token);

        token = strtok(NULL, ",");
        if (token == NULL) continue;
        char name[32];
        while (*token == ' ') token++;
        strncpy(name, token, sizeof(name) - 1);
        name[sizeof(name) - 1] = '\0';

        token = strtok(NULL, ",");
        if(token == NULL) continue;
        int score = atoi(token);

        add_student(head, id, name, score);
        count++;
    }

    fclose(file);
    return count;
}

int save_students_to_csv(const char* filename, Student* head) {
    FILE* file = fopen(filename,"w");
    if(file == NULL) {
        printf("Error: Failed to open file for writing.\n");
        return -1;
    }

    fprintf(file, "id,name,score\n");

    int count = 0;
    Student* current = head;

    while (current != NULL) {
        fprintf(file, "%d,%s,%d\n", current->id, current->name, current->score);
        current = current->next;
        count++; 
    }

    fclose(file);
    return count;
}