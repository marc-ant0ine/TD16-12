#ifndef CORE_H
#define CORE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Constantes
#define MAX_LINE_LENGTH 1024
#define MAX_NAME_LENGTH 256
#define PASSING_GRADE 10.0

// Types de base
typedef enum {
    FALSE = 0,
    TRUE = 1
} BOOL;

// Fonctions utilitaires
int is_empty_line(char *line);
void remove_newline(char *line);
int is_student_header(char *line);
int is_course_header(char *line);
int is_grade_header(char *line);

#endif
