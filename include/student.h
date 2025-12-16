#ifndef STUDENT_H
#define STUDENT_H

#include "course.h"

typedef struct {
    int id;
    int age;
    int courses_count;
    double overall_average;
    char *first_name;
    char *last_name;
    Course **courses;
    unsigned long long validated_courses;  // Système binaire
} Student;

// Fonctions de gestion des étudiants
Student* create_student(int id, const char *first_name, const char *last_name, int age);
int add_grade_to_student(Student *student, const char *course_name, double coefficient, double grade);
void calculate_overall_average(Student *student);
Student* find_student(Student **students, int count, int id);
void free_student(Student *student);

#endif
