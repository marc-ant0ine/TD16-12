#include "student.h"
#include <stdlib.h>
#include <string.h>

Student* create_student(int id, const char *first_name, const char *last_name, int age) {
    if (first_name == NULL || last_name == NULL) return NULL;
    
    Student *student = (Student*)malloc(sizeof(Student));
    if (student == NULL) return NULL;
    
    student->first_name = (char*)malloc((strlen(first_name) + 1) * sizeof(char));
    if (student->first_name == NULL) {
        free(student);
        return NULL;
    }
    strcpy(student->first_name, first_name);
    
    student->last_name = (char*)malloc((strlen(last_name) + 1) * sizeof(char));
    if (student->last_name == NULL) {
        free(student->first_name);
        free(student);
        return NULL;
    }
    strcpy(student->last_name, last_name);
    
    student->id = id;
    student->age = age;
    student->courses = NULL;
    student->courses_count = 0;
    student->overall_average = 0.0;
    student->validated_courses = 0;
    
    return student;
}

Course* find_or_create_course(Student *student, const char *course_name, double coefficient) {
    if (student == NULL || course_name == NULL) return NULL;
    
    for (int i = 0; i < student->courses_count; i++) {
        if (strcmp(student->courses[i]->name, course_name) == 0) {
            return student->courses[i];
        }
    }
    
    Course *new_course = create_course(course_name, coefficient);
    if (new_course == NULL) return NULL;
    
    Course **new_courses = (Course**)realloc(student->courses, 
                                             (student->courses_count + 1) * sizeof(Course*));
    if (new_courses == NULL) {
        free_course(new_course);
        return NULL;
    }
    
    student->courses = new_courses;
    student->courses[student->courses_count] = new_course;
    student->courses_count++;
    
    return new_course;
}

void calculate_overall_average(Student *student) {
    if (student == NULL) return;
    
    double sum_weighted = 0.0;
    double sum_coefficients = 0.0;
    
    for (int i = 0; i < student->courses_count; i++) {
        Course *course = student->courses[i];
        if (course->notes.size > 0) {
            sum_weighted += course->average * course->coefficient;
            sum_coefficients += course->coefficient;
        }
    }
    
    if (sum_coefficients > 0) {
        student->overall_average = sum_weighted / sum_coefficients;
    } else {
        student->overall_average = 0.0;
    }
}

int add_grade_to_student(Student *student, const char *course_name, 
                         double coefficient, double grade) {
    if (student == NULL) return -1;
    
    Course *course = find_or_create_course(student, course_name, coefficient);
    if (course == NULL) return -1;
    
    if (add_grade_to_course(course, grade) != 0) return -1;
    
    calculate_overall_average(student);
    
    return 0;
}

Student* find_student(Student **students, int count, int id) {
    for (int i = 0; i < count; i++) {
        if (students[i]->id == id) {
            return students[i];
        }
    }
    return NULL;
}

void free_student(Student *student) {
    if (student == NULL) return;
    
    if (student->first_name != NULL) {
        free(student->first_name);
        student->first_name = NULL;
    }
    
    if (student->last_name != NULL) {
        free(student->last_name);
        student->last_name = NULL;
    }
    
    if (student->courses != NULL) {
        for (int i = 0; i < student->courses_count; i++) {
            free_course(student->courses[i]);
        }
        free(student->courses);
        student->courses = NULL;
    }
    
    free(student);
}
