#include "api.h"
#include "class.h"
#include "core.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Class* API_load_students(char *filePath) {
    if (filePath == NULL) {
        fprintf(stderr, "Error: filePath is NULL\n");
        return NULL;
    }
    
    FILE *file = fopen(filePath, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: cannot open file %s\n", filePath);
        return NULL;
    }
    
    Class *class = create_class();
    if (class == NULL) {
        fprintf(stderr, "Error: failed to create class structure\n");
        fclose(file);
        return NULL;
    }
    
    char ligne[1024];
    enum { INIT, STUDENTS, COURSES, GRADES } state = INIT;
    
    while (fgets(ligne, sizeof(ligne), file) != NULL) {
        if (is_empty_line(ligne)) {
            continue;
        }
        
        remove_newline(ligne);
        
        if (strstr(ligne, "ETUDIANTS") != NULL) {
            state = STUDENTS;
            continue;
        } else if (strstr(ligne, "MATIERES") != NULL) {
            state = COURSES;
            continue;
        } else if (strstr(ligne, "NOTES") != NULL) {
            state = GRADES;
            continue;
        }
        
        switch (state) {
            case STUDENTS: {
                Student *student = parse_student_line(ligne);
                if (student != NULL) {
                    if (add_student_to_class(class, student) != 0) {
                        fprintf(stderr, "Error: failed to add student to class\n");
                        free_student(student);
                        free_class(class);
                        fclose(file);
                        return NULL;
                    }
                }
                break;
            }
            
            case COURSES: {
                Course *course = parse_course_line(ligne);
                if (course != NULL) {
                    Course **new_courses = (Course**)realloc(class->courses,
                                                            (class->courses_count + 1) * sizeof(Course*));
                    if (new_courses == NULL) {
                        fprintf(stderr, "Error: memory allocation failed for courses\n");
                        free_course(course);
                        free_class(class);
                        fclose(file);
                        return NULL;
                    }
                    class->courses = new_courses;
                    class->courses[class->courses_count] = course;
                    class->courses_count++;
                }
                break;
            }
            
            case GRADES: {
                if (parse_grade_line(ligne, class->students, class->students_count,
                                     class->courses, class->courses_count) != 0) {
                    // On continue même en cas d'erreur
                }
                break;
            }
            
            default:
                break;
        }
    }
    
    fclose(file);
    
    if (class->students_count == 0 && class->courses_count == 0) {
        fprintf(stderr, "Warning: no data loaded from file\n");
        free_class(class);
        return NULL;
    }
    
    printf("Successfully loaded: %d students, %d courses\n", 
           class->students_count, class->courses_count);
    
    return class;
}

int API_save_to_binary_file(Class* pClass, char* filePath) {
    if (pClass == NULL) {
        fprintf(stderr, "Error: pClass is NULL\n");
        return -1;
    }
    
    if (filePath == NULL) {
        fprintf(stderr, "Error: filePath is NULL\n");
        return -1;
    }
    
    return save_class_to_binary(pClass, filePath);
}

void API_display(Class* pClass) {
    if (pClass == NULL) {
        fprintf(stderr, "Error: Class pointer is NULL\n");
        return;
    }
    
    printf("\n================================================================================\n");
    printf("                    STUDENT MANAGEMENT SYSTEM v1.0                             \n");
    printf("================================================================================\n\n");
    
    // SECTION : MATIERES
    printf("=== AVAILABLE COURSES ===\n");
    printf("Total number of courses: %d\n\n", pClass->courses_count);
    
    if (pClass->courses_count > 0) {
        printf("%-5s | %-30s | %-12s\n", "No.", "Course Name", "Coefficient");
        printf("------+--------------------------------+--------------\n");
        
        for (int i = 0; i < pClass->courses_count; i++) {
            if (pClass->courses[i] != NULL) {
                printf("%-5d | %-30s | %-12.2f\n", 
                       i + 1, 
                       pClass->courses[i]->name, 
                       pClass->courses[i]->coefficient);
            }
        }
        printf("\n");
    } else {
        printf("No courses registered in the system.\n\n");
    }
    
    // SECTION : ETUDIANTS
    printf("=== STUDENTS AND THEIR GRADES ===\n");
    printf("Total number of students: %d\n\n", pClass->students_count);
    
    if (pClass->students_count == 0) {
        printf("No students registered in the system.\n\n");
        return;
    }
    
    for (int i = 0; i < pClass->students_count; i++) {
        Student *student = pClass->students[i];
        if (student == NULL) continue;
        
        printf("--------------------------------------------------------------------------------\n");
        printf("Student #%d: %s %s (ID: %d, Age: %d)\n", 
               i + 1,
               student->first_name, 
               student->last_name, 
               student->id,
               student->age);
        printf("--------------------------------------------------------------------------------\n");
        
        if (student->courses_count == 0) {
            printf("  No courses enrolled.\n");
        } else {
            printf("  %-30s | %-10s | %-15s\n", 
                   "Course Name", "Average", "Coefficient");
            printf("  --------------------------------+------------+-----------------\n");
            
            for (int j = 0; j < student->courses_count; j++) {
                Course *course = student->courses[j];
                if (course != NULL && course->notes.size > 0) {
                    printf("  %-30s | %10.2f | %15.2f\n", 
                           course->name, 
                           course->average,
                           course->coefficient);
                } else if (course != NULL) {
                    printf("  %-30s | %10s | %15.2f\n", 
                           course->name, 
                           "N/A",
                           course->coefficient);
                }
            }
            
            printf("  --------------------------------+------------+-----------------\n");
            
            if (student->overall_average > 0) {
                printf("  %-30s | %10.2f |\n", 
                       "OVERALL AVERAGE", 
                       student->overall_average);
            } else {
                printf("  %-30s | %10s |\n", 
                       "OVERALL AVERAGE", 
                       "N/A");
            }
        }
        printf("\n");
    }
    
    // STATISTIQUES GLOBALES
    printf("=== GLOBAL STATISTICS ===\n");
    
    double class_average_sum = 0.0;
    int students_with_average = 0;
    
    for (int i = 0; i < pClass->students_count; i++) {
        if (pClass->students[i]->courses_count > 0 && 
            pClass->students[i]->overall_average > 0) {
            class_average_sum += pClass->students[i]->overall_average;
            students_with_average++;
        }
    }
    
    if (students_with_average > 0) {
        double class_average = class_average_sum / students_with_average;
        printf("Class Overall Average: %.2f/20\n", class_average);
        printf("Students with grades: %d/%d\n", 
               students_with_average, 
               pClass->students_count);
    } else {
        printf("No student averages available yet.\n");
    }
    
    printf("\n");
}

void API_unload(Class* pClass) {
    if (pClass == NULL) {
        return;
    }
    
    free_class(pClass);
}

int compare_students_desc(const void *a, const void *b) {
    StudentRanking *sa = (StudentRanking*)a;
    StudentRanking *sb = (StudentRanking*)b;
    
    if (sb->average > sa->average) return 1;
    if (sb->average < sa->average) return -1;
    return 0;
}

char** API_get_best_students(CLASS_DATA* pClass) {
    if (pClass == NULL) {
        return NULL;
    }
    
    char** result = (char**)calloc(SIZE_TOP1, sizeof(char*));
    if (result == NULL) {
        return NULL;
    }
    
    int valid_students = 0;
    for (int i = 0; i < pClass->students_count; i++) {
        if (pClass->students[i]->courses_count > 0 && 
            pClass->students[i]->overall_average > 0) {
            valid_students++;
        }
    }
    
    if (valid_students == 0) {
        return result;
    }
    
    StudentRanking *rankings = (StudentRanking*)malloc(valid_students * sizeof(StudentRanking));
    if (rankings == NULL) {
        free(result);
        return NULL;
    }
    
    int idx = 0;
    for (int i = 0; i < pClass->students_count; i++) {
        if (pClass->students[i]->courses_count > 0 && 
            pClass->students[i]->overall_average > 0) {
            rankings[idx].student = pClass->students[i];
            rankings[idx].average = pClass->students[i]->overall_average;
            idx++;
        }
    }
    
    qsort(rankings, valid_students, sizeof(StudentRanking), compare_students_desc);
    
    int num_to_process = (valid_students < SIZE_TOP1) ? valid_students : SIZE_TOP1;
    
    for (int i = 0; i < num_to_process; i++) {
        Student *student = rankings[i].student;
        
        char buffer[512];
        snprintf(buffer, sizeof(buffer), 
                 "%d. %s %s (ID: %d) - Moyenne: %.2f/20",
                 i + 1,
                 student->first_name,
                 student->last_name,
                 student->id,
                 student->overall_average);
        
        result[i] = (char*)malloc((strlen(buffer) + 1) * sizeof(char));
        if (result[i] == NULL) {
            for (int j = 0; j < i; j++) {
                free(result[j]);
            }
            free(result);
            free(rankings);
            return NULL;
        }
        
        strcpy(result[i], buffer);
    }
    
    free(rankings);
    return result;
}

void API_free_best_students(char** best_students) {
    if (best_students == NULL) {
        return;
    }
    
    for (int i = 0; i < SIZE_TOP1; i++) {
        if (best_students[i] != NULL) {
            free(best_students[i]);
        }
    }
    
    free(best_students);
}

char** API_get_best_students_in_course(CLASS_DATA* pClass, char* course) {
    if (pClass == NULL || course == NULL) {
        return NULL;
    }
    
    char** result = (char**)calloc(SIZE_TOP2, sizeof(char*));
    if (result == NULL) {
        return NULL;
    }
    
    int enrolled_students = 0;
    for (int i = 0; i < pClass->students_count; i++) {
        Student *student = pClass->students[i];
        for (int j = 0; j < student->courses_count; j++) {
            if (strcmp(student->courses[j]->name, course) == 0 && 
                student->courses[j]->notes.size > 0) {
                enrolled_students++;
                break;
            }
        }
    }
    
    if (enrolled_students == 0) {
        return result;
    }
    
    StudentRanking *rankings = (StudentRanking*)malloc(enrolled_students * sizeof(StudentRanking));
    if (rankings == NULL) {
        free(result);
        return NULL;
    }
    
    int idx = 0;
    for (int i = 0; i < pClass->students_count; i++) {
        Student *student = pClass->students[i];
        for (int j = 0; j < student->courses_count; j++) {
            if (strcmp(student->courses[j]->name, course) == 0 && 
                student->courses[j]->notes.size > 0) {
                rankings[idx].student = student;
                rankings[idx].average = student->courses[j]->average;
                idx++;
                break;
            }
        }
    }
    
    qsort(rankings, enrolled_students, sizeof(StudentRanking), compare_students_desc);
    
    int num_to_process = (enrolled_students < SIZE_TOP2) ? enrolled_students : SIZE_TOP2;
    
    for (int i = 0; i < num_to_process; i++) {
        Student *student = rankings[i].student;
        double course_average = rankings[i].average;
        
        char buffer[512];
        snprintf(buffer, sizeof(buffer), 
                 "%d. %s %s (ID: %d) - Moyenne en %s: %.2f/20",
                 i + 1,
                 student->first_name,
                 student->last_name,
                 student->id,
                 course,
                 course_average);
        
        result[i] = (char*)malloc((strlen(buffer) + 1) * sizeof(char));
        if (result[i] == NULL) {
            for (int j = 0; j < i; j++) {
                free(result[j]);
            }
            free(result);
            free(rankings);
            return NULL;
        }
        
        strcpy(result[i], buffer);
    }
    
    free(rankings);
    return result;
}

void API_free_best_students_in_course(char** best_students) {
    if (best_students == NULL) {
        return;
    }
    
    for (int i = 0; i < SIZE_TOP2; i++) {
        if (best_students[i] != NULL) {
            free(best_students[i]);
        }
    }
    
    free(best_students);
}
