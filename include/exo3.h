#ifndef EXO3_H
#define EXO3_H

#include <stddef.h>

// Déclarations anticipées
typedef struct Student Student;
typedef struct Course Course;




void* inverser_ordre(const void* data, size_t element_size, size_t count);


void afficher_octets_hex(const void* data, size_t size, const char* label);


int sauvegarder_etudiant_binaire(const Student* student, const char* filename);


int comparer_memoire(const void* ptr1, const void* ptr2, size_t size);



#endif 
