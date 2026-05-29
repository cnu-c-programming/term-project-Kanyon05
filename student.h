#ifndef STUDENT_H
#define STUDENT_H

typedef struct Student {
    int id;
    char name[32];
    struct Student* next;
} Student;

#endif