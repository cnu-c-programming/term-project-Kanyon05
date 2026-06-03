#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "student.h"

Student* find_student(Student* head, int id) {
    Student* current = head;

    while (current != NULL) {
        if (current->id == id) {
            return current;
        }
        current = current -> next;
    }
    return NULL;
}

void add_student(Student** head, int id, const char* name, int score) {
    Student* new_student = (Student*) malloc(sizeof(Student));
    if (new_student == NULL) {
        printf("Error: Memory allocation failed.\n");
        return;
    }

    new_student->id = id;
    new_student->score = score;
    new_student->next = NULL;

    strncpy(new_student->name, name, sizeof(new_student->name)-1);
    new_student->name[sizeof(new_student->name)-1]= '\0';

    if (*head == NULL){
        *head = new_student;
    } else {
        Student* current = *head;
        while(current->next != NULL) {
            current = current->next;
        }
        current->next = new_student;
    }
}

void delete_student(Student** head, int id) {
    Student* current = *head;
    Student* prev = NULL;

    while (current!=NULL && current->id != id) {
        prev = current;
        current = current->next;
    }

    if (current == NULL) {
        printf("Error: student not found.\n");
        return;
    }

    if (prev == NULL) {
        *head = current->next;
    } else {
        prev->next = current->next;
    }

    free(current);
    printf("Studnet deleted. \n");
}

void update_student(Student* head, int id, int score) {
    Student* target = find_student(head, id); 
    
    if (target == NULL) {
        printf("Error: student not found.\n");
        return;
    }
    
    target->score = score;
    printf("Student updated.\n");
}

void list_students(Student* head) {
    if (head == NULL) {
        printf("No students found.\n");
        return;
    }
    
    printf("ID\tName\tScore\n");
    Student* current = head;
    while (current != NULL) {
        printf("%d\t%s\t%d\n", current->id, current->name, current->score);
        current = current->next;
    }
}