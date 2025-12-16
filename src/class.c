#include "class.h"
#include "core.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

Class* create_class(void) {
    Class *class = (Class*)malloc(sizeof(Class));
    if (class == NULL) return NULL;
    
    class->students = NULL;
    class->students_count = 0;
    class->courses = NULL;
    class->courses_count = 0;
    
    return class;
}

int add_student_to_class(Class *class, Student *student) {
    if (class == NULL || student == NULL) return -1;
    
    Student **new_students = (Student**)realloc(class->students, 
                                                (class->students_count + 1) * sizeof(Student*));
    if (new_students == NULL) return -1;
    
    class->students = new_students;
    class->students[class->students_count] = student;
    class->students_count++;
    
    return 0;
}

void free_class(Class *class) {
    if (class == NULL) return;
    
    if (class->students != NULL) {
        for (int i = 0; i < class->students_count; i++) {
            free_student(class->students[i]);
        }
        free(class->students);
        class->students = NULL;
    }
    
    if (class->courses != NULL) {
        for (int i = 0; i < class->courses_count; i++) {
            free_course(class->courses[i]);
        }
        free(class->courses);
        class->courses = NULL;
    }
    
    free(class);
}

Student* parse_student_line(char *line) {
    if (line == NULL || is_empty_line(line)) {
        return NULL;
    }
    
    remove_newline(line);
    
    if (is_student_header(line)) {
        return NULL;
    }
    
    int id, age;
    char first_name_temp[256];
    char last_name_temp[256];
    
    int nb_fields = sscanf(line, "%d;%255[^;];%255[^;];%d",
                           &id, first_name_temp, last_name_temp, &age);
    
    if (nb_fields != 4) {
        fprintf(stderr, "Error: invalid student line format: %s\n", line);
        return NULL;
    }
    
    return create_student(id, first_name_temp, last_name_temp, age);
}

Course* parse_course_line(char *line) {
    if (line == NULL || is_empty_line(line)) {
        return NULL;
    }
    
    remove_newline(line);
    
    if (is_course_header(line)) {
        return NULL;
    }
    
    char name_temp[256];
    double coefficient;
    
    int nb_fields = sscanf(line, "%255[^;];%lf", name_temp, &coefficient);
    
    if (nb_fields != 2) {
        fprintf(stderr, "Error: invalid course line format: %s\n", line);
        return NULL;
    }
    
    return create_course(name_temp, coefficient);
}

int parse_grade_line(char *line, Student **students, int students_count, 
                     Course **courses, int courses_count) {
    if (line == NULL || is_empty_line(line)) {
        return 0;
    }
    
    remove_newline(line);
    
    if (is_grade_header(line)) {
        return 0;
    }
    
    int student_id;
    char course_name[256];
    double grade;
    
    int nb_fields = sscanf(line, "%d;%255[^;];%lf", &student_id, course_name, &grade);
    
    if (nb_fields != 3) {
        fprintf(stderr, "Error: invalid grade line format: %s\n", line);
        return -1;
    }
    
    Student *student = find_student(students, students_count, student_id);
    if (student == NULL) {
        fprintf(stderr, "Warning: student with id %d not found\n", student_id);
        return -1;
    }
    
    double coefficient = 1.0;
    for (int i = 0; i < courses_count; i++) {
        if (strcmp(courses[i]->name, course_name) == 0) {
            coefficient = courses[i]->coefficient;
            break;
        }
    }
    
    return add_grade_to_student(student, course_name, coefficient, grade);
}
