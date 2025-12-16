#ifndef COURSE_H
#define COURSE_H

#include "grades.h"

typedef struct {
    char *name;
    double coefficient;
    double average;
    Grades notes;
    int index;  // Pour le système binaire
} Course;

// Fonctions de gestion des matières
Course* create_course(const char *name, double coefficient);
int add_grade_to_course(Course *course, double grade);
void free_course(Course *course);

#endif
