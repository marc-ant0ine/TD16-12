#ifndef FILE_IO_H
#define FILE_IO_H

#include "class.h"

// Chargement depuis fichier texte
Class* load_class_from_file(const char *filepath);

// Sauvegarde/chargement binaire
int save_class_to_binary(Class *class, const char *filepath);
Class* load_class_from_binary(const char *filepath);

// Fonctions internes
int save_string(FILE *file, const char *str);
char* load_string(FILE *file);
int save_grades(FILE *file, Grades *grades);
int load_grades(FILE *file, Grades *grades);
int save_course(FILE *file, Course *course);
Course* load_course(FILE *file);
int save_student(FILE *file, Student *student);
Student* load_student(FILE *file);

#endif
