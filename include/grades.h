#ifndef GRADES_H
#define GRADES_H

typedef struct {
    int size;
    double *data;
} Grades;

// Fonctions de gestion des notes
void init_grades(Grades *grades);
int add_grade(Grades *grades, double grade);
double calculate_grades_average(const Grades *grades);
void free_grades(Grades *grades);

#endif
