#include "sorting.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int compare_students_desc(const void *a, const void *b) {
    StudentRanking *sa = (StudentRanking*)a;
    StudentRanking *sb = (StudentRanking*)b;
    
    if (sb->average > sa->average) return 1;
    if (sb->average < sa->average) return -1;
    return 0;
}

int compare_students_by_name(const void *a, const void *b) {
    Student *sa = *(Student**)a;
    Student *sb = *(Student**)b;
    
    int cmp = strcmp(sa->last_name, sb->last_name);
    if (cmp != 0) return cmp;
    
    return strcmp(sa->first_name, sb->first_name);
}

int compare_students_by_age(const void *a, const void *b) {
    Student *sa = *(Student**)a;
    Student *sb = *(Student**)b;
    
    if (sa->age < sb->age) return -1;
    if (sa->age > sb->age) return 1;
    return 0;
}

int compare_students_by_id(const void *a, const void *b) {
    Student *sa = *(Student**)a;
    Student *sb = *(Student**)b;
    
    if (sa->id < sb->id) return -1;
    if (sa->id > sb->id) return 1;
    return 0;
}

int API_set_sorting_mode(CLASS_DATA* pClass, int mode) {
    if (pClass == NULL) {
        fprintf(stderr, "Error: pClass is NULL\n");
        return 0;
    }
    
    switch (mode) {
        case SORT_BY_AVERAGE_DESC:
            // Utilise la fonction existante pour le tri par moyenne
            pClass->current_mode = SORT_BY_AVERAGE_DESC;
            break;
            
        case SORT_BY_NAME_ASC:
            pClass->current_mode = SORT_BY_NAME_ASC;
            break;
            
        case SORT_BY_AGE_ASC:
            pClass->current_mode = SORT_BY_AGE_ASC;
            break;
            
        case SORT_BY_ID_ASC:
            pClass->current_mode = SORT_BY_ID_ASC;
            break;
            
        default:
            fprintf(stderr, "Error: Invalid sorting mode %d\n", mode);
            return 0;
    }
    
    return 1;
}

char** API_sort_students(CLASS_DATA* pClass) {
    if (pClass == NULL) {
        fprintf(stderr, "Error: pClass is NULL\n");
        return NULL;
    }
    
    if (pClass->students == NULL || pClass->students_count == 0) {
        fprintf(stderr, "Error: No students to sort\n");
        return NULL;
    }
    
    Student **sorted_students = (Student**)malloc(pClass->students_count * sizeof(Student*));
    if (sorted_students == NULL) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        return NULL;
    }
    
    for (int i = 0; i < pClass->students_count; i++) {
        sorted_students[i] = pClass->students[i];
    }
    
    switch (pClass->current_mode) {
        case SORT_BY_AVERAGE_DESC:
            // Utilise le système existant de StudentRanking pour le tri par moyenne
            {
                StudentRanking *rankings = (StudentRanking*)malloc(pClass->students_count * sizeof(StudentRanking));
                if (rankings == NULL) {
                    free(sorted_students);
                    return NULL;
                }
                
                for (int i = 0; i < pClass->students_count; i++) {
                    rankings[i].student = pClass->students[i];
                    rankings[i].average = pClass->students[i]->overall_average;
                }
                
                qsort(rankings, pClass->students_count, sizeof(StudentRanking), compare_students_desc);
                
                for (int i = 0; i < pClass->students_count; i++) {
                    sorted_students[i] = rankings[i].student;
                }
                
                free(rankings);
            }
            break;
            
        case SORT_BY_NAME_ASC:
            qsort(sorted_students, pClass->students_count, sizeof(Student*), compare_students_by_name);
            break;
            
        case SORT_BY_AGE_ASC:
            qsort(sorted_students, pClass->students_count, sizeof(Student*), compare_students_by_age);
            break;
            
        case SORT_BY_ID_ASC:
            qsort(sorted_students, pClass->students_count, sizeof(Student*), compare_students_by_id);
            break;
    }
    
    char **result = (char**)malloc(SIZE_TOP1 * sizeof(char*));
    if (result == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for result array\n");
        free(sorted_students);
        return NULL;
    }
    
    for (int i = 0; i < SIZE_TOP1; i++) {
        result[i] = NULL;
    }
    
    int num_to_include = (pClass->students_count < SIZE_TOP1) ? pClass->students_count : SIZE_TOP1;
    
    for (int i = 0; i < num_to_include; i++) {
        Student *s = sorted_students[i];
        
        char buffer[512];
        
        switch (pClass->current_mode) {
            case SORT_BY_AVERAGE_DESC:
                snprintf(buffer, sizeof(buffer), 
                        "#%d - %s %s (ID:%d) - Average: %.2f/20",
                        i + 1, s->first_name, s->last_name, s->id, s->overall_average);
                break;
                
            case SORT_BY_NAME_ASC:
                snprintf(buffer, sizeof(buffer),
                        "#%d - %s %s (ID:%d, Age:%d, Avg:%.2f)",
                        i + 1, s->last_name, s->first_name, s->id, s->age, s->overall_average);
                break;
                
            case SORT_BY_AGE_ASC:
                snprintf(buffer, sizeof(buffer),
                        "#%d - Age:%d - %s %s (ID:%d, Avg:%.2f)",
                        i + 1, s->age, s->first_name, s->last_name, s->id, s->overall_average);
                break;
                
            case SORT_BY_ID_ASC:
                snprintf(buffer, sizeof(buffer),
                        "#%d - ID:%d - %s %s (Age:%d, Avg:%.2f)",
                        i + 1, s->id, s->first_name, s->last_name, s->age, s->overall_average);
                break;
        }
        
        result[i] = (char*)malloc((strlen(buffer) + 1) * sizeof(char));
        if (result[i] == NULL) {
            fprintf(stderr, "Error: Memory allocation failed for string %d\n", i);
            for (int j = 0; j < i; j++) {
                free(result[j]);
            }
            free(result);
            free(sorted_students);
            return NULL;
        }
        
        strcpy(result[i], buffer);
    }
    
    free(sorted_students);
    return result;
}

void API_free_sort_result(char **result) {
    if (result == NULL) return;
    
    for (int i = 0; i < SIZE_TOP1; i++) {
        if (result[i] != NULL) {
            free(result[i]);
        }
    }
    free(result);
}
