#ifndef SORTING_H
#define SORTING_H

#include "class.h"

// Modes de tri
typedef enum {
    SORT_BY_AVERAGE_DESC = 0,
    SORT_BY_NAME_ASC = 1,
    SORT_BY_AGE_ASC = 2,
    SORT_BY_ID_ASC = 3
} SortMode;

// Structure pour le classement
typedef struct {
    Student *student;
    double average;
} StudentRanking;

// Fonctions de comparaison
int compare_students_desc(const void *a, const void *b);
int compare_students_by_name(const void *a, const void *b);
int compare_students_by_age(const void *a, const void *b);
int compare_students_by_id(const void *a, const void *b);

// API de tri
int API_set_sorting_mode(CLASS_DATA* pClass, int mode);
char** API_sort_students(CLASS_DATA* pClass);
void API_free_sort_result(char **result);

#endif
