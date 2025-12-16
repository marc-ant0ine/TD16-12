#include "course.h"
#include <stdlib.h>
#include <string.h>

Course* create_course(const char *name, double coefficient) {
    if (name == NULL) return NULL;
    
    Course *course = (Course*)malloc(sizeof(Course));
    if (course == NULL) return NULL;
    
    course->name = (char*)malloc((strlen(name) + 1) * sizeof(char));
    if (course->name == NULL) {
        free(course);
        return NULL;
    }
    strcpy(course->name, name);
    
    course->coefficient = coefficient;
    course->average = 0.0;
    course->index = -1; // Sera défini par binary_fields si utilisé
    init_grades(&course->notes);
    
    return course;
}

int add_grade_to_course(Course *course, double grade) {
    if (course == NULL) return -1;
    
    if (add_grade(&course->notes, grade) != 0) return -1;
    
    course->average = calculate_grades_average(&course->notes);
    
    return 0;
}

void free_course(Course *course) {
    if (course == NULL) return;
    
    if (course->name != NULL) {
        free(course->name);
        course->name = NULL;
    }
    
    free_grades(&course->notes);
    free(course);
}
