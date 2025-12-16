#ifndef API_H
#define API_H

#include "class.h"

// Constantes API
#define SIZE_TOP1 10
#define SIZE_TOP2 3

// Alias pour la compatibilité
typedef Class CLASS_DATA;

// Fonctions API principales
Class* API_load_students(char *filePath);
int API_save_to_binary_file(Class* pClass, char* filePath);
void API_display(Class* pClass);
void API_unload(Class* pClass);

// Top étudiants
char** API_get_best_students(CLASS_DATA* pClass);
char** API_get_best_students_in_course(CLASS_DATA* pClass, char* course);

// Libération mémoire
void API_free_best_students(char** best_students);
void API_free_best_students_in_course(char** best_students);

#endif
