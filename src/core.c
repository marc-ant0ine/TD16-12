#include "core.h"

int is_empty_line(char *line) {
    if (line == NULL) return 1;
    while (*line) {
        if (!isspace((unsigned char)*line)) return 0;
        line++;
    }
    return 1;
}

void remove_newline(char *line) {
    if (line == NULL) return;
    size_t len = strlen(line);
    if (len > 0 && line[len-1] == '\n') {
        line[len-1] = '\0';
    }
    if (len > 1 && line[len-2] == '\r') {
        line[len-2] = '\0';
    }
}

int is_student_header(char *line) {
    return (strstr(line, "numero") != NULL && 
            strstr(line, "prenom") != NULL && 
            strstr(line, "nom") != NULL);
}

int is_course_header(char *line) {
    return (strstr(line, "nom") != NULL && 
            strstr(line, "coef") != NULL &&
            strstr(line, "prenom") == NULL);
}

int is_grade_header(char *line) {
    return (strstr(line, "id") != NULL && 
            strstr(line, "nom") != NULL && 
            strstr(line, "note") != NULL);
}
