#ifndef STUDENT_H
#define STUDENT_H

typedef struct Student {
    int id;
    char name[32];
    int score;
    struct Student* next;
} Student;

void add_student(Student** head, int id, const char* name, int score);
void delete_student(Student** head, int id);
void update_student (Student* head, int id, int score);
Student* find_student(Student* head, int id);

void list_students(Student* head);
void print_stats(Student* head);

void free_all_students(Student** head);

#endif

