#ifndef EXO2_H
#define EXO2_H

#include <stddef.h>


typedef struct Student Student;
typedef struct Class Class;


char** listePrenoms(Class* promotion, int* taille);


void afficher_info_memoire(char** tableau, int taille);

#endif 
