#include "grades.h"
#include <stdlib.h>

void init_grades(Grades *grades) {
    if (grades == NULL) return;
    grades->data = NULL;
    grades->size = 0;
}

int add_grade(Grades *grades, double grade) {
    if (grades == NULL) return -1;
    
    double *new_data = (double*)realloc(grades->data, (grades->size + 1) * sizeof(double));
    if (new_data == NULL) return -1;
    
    grades->data = new_data;
    grades->data[grades->size] = grade;
    grades->size++;
    
    return 0;
}

double calculate_grades_average(const Grades *grades) {
    if (grades == NULL || grades->size == 0) return 0.0;
    
    double sum = 0.0;
    for (int i = 0; i < grades->size; i++) {
        sum += grades->data[i];
    }
    
    return sum / grades->size;
}

void free_grades(Grades *grades) {
    if (grades == NULL) return;
    
    if (grades->data != NULL) {
        free(grades->data);
        grades->data = NULL;
    }
    grades->size = 0;
}
