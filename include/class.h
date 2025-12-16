#ifndef CLASS_H
#define CLASS_H

#include "student.h"
#include "course.h"

typedef struct {
    Student **students;
    int students_count;
    Course **courses;
    int courses_count;
} Class;

// Fonctions de gestion de la classe
Class* create_class(void);
int add_student_to_class(Class *class, Student *student);
void free_class(Class *class);

// Parser
Student* parse_student_line(char *line);
Course* parse_course_line(char *line);
int parse_grade_line(char *line, Student **students, int students_count, 
                     Course **courses, int courses_count);

#endif
