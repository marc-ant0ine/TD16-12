#ifndef EXO1_H
#define EXO1_H

#include <stdbool.h>

// Déclaration des types (si non déjà déclarés ailleurs)
typedef struct Student Student;
typedef struct Class Class;

// Définition du pointeur de procédure Recalcul
typedef void (*Recalcul)(Student*, double, double);


bool miseAJourNotes(Class* promotion, Recalcul fonctionRecalcul);


void normalisation(Student* etudiant, double min_moyenne, double max_moyenne);

void afficher_resume_moyennes(Class *promotion);

#endif 
