#include "exo3.h"
#include "student.h"
#include "course.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// ===== FONCTION inverser_ordre =====

void* inverser_ordre(const void* data, size_t element_size, size_t count) {
    // Vérification des paramètres
    if (data == NULL || count == 0) {
        return NULL;
    }
    
    // Vérifier que la taille est valide (2, 4 ou 8 octets)
    if (element_size != 2 && element_size != 4 && element_size != 8) {
        fprintf(stderr, "Erreur : Taille d'élément invalide : %zu octets\n", element_size);
        fprintf(stderr, "  Tailles acceptées : 2, 4 ou 8 octets\n");
        return NULL;
    }
    
    // Allouer de la mémoire pour la copie
    size_t total_size = element_size * count;
    void* result = malloc(total_size);
    if (result == NULL) {
        fprintf(stderr, "Erreur d'allocation mémoire\n");
        return NULL;
    }
    
    // Copier les données originales (pour les laisser intactes)
    memcpy(result, data, total_size);
    
    // Inverser l'ordre des octets pour chaque élément
    uint8_t* byte_ptr = (uint8_t*)result;
    
    for (size_t i = 0; i < count; i++) {
        uint8_t* element_start = byte_ptr + (i * element_size);
        
        // Inverser les octets de cet élément
        for (size_t j = 0; j < element_size / 2; j++) {
            uint8_t temp = element_start[j];
            element_start[j] = element_start[element_size - 1 - j];
            element_start[element_size - 1 - j] = temp;
        }
    }
    
    return result;
}

// ===== FONCTION afficher_octets_hex =====

void afficher_octets_hex(const void* data, size_t size, const char* label) {
    if (data == NULL || size == 0) {
        printf("%s : (données NULL ou vide)\n", label);
        return;
    }
    
    const uint8_t* bytes = (const uint8_t*)data;
    
    printf("%s : ", label);
    
    for (size_t i = 0; i < size; i++) {
        printf("%02X", bytes[i]);
        
        // Ajouter un espace tous les octets pour lisibilité
        if (i < size - 1) {
            printf(" ");
            
            // Nouvelle ligne tous les 16 octets
            if ((i + 1) % 16 == 0) {
                printf("\n                    ");
            }
        }
    }
    
    printf("\n");
}

//  FONCTION comparer_memoire 

int comparer_memoire(const void* ptr1, const void* ptr2, size_t size) {
    if (ptr1 == NULL || ptr2 == NULL) {
        return -1;
    }
    
    const uint8_t* p1 = (const uint8_t*)ptr1;
    const uint8_t* p2 = (const uint8_t*)ptr2;
    
    for (size_t i = 0; i < size; i++) {
        if (p1[i] != p2[i]) {
            return (int)(i + 1);  // Retourne la position de la première différence
        }
    }
    
    return 0;  // Identique
}

// ===== FONCTIONS INTERNES POUR LA SAUVEGARDE =====


static int ecrire_chaine(FILE* file, const char* str) {
    if (str == NULL) {
        size_t len = 0;
        return fwrite(&len, sizeof(size_t), 1, file) == 1 ? 0 : -1;
    }
    
    size_t len = strlen(str);
    if (fwrite(&len, sizeof(size_t), 1, file) != 1) {
        return -1;
    }
    
    if (len > 0 && fwrite(str, sizeof(char), len, file) != len) {
        return -1;
    }
    
    return 0;
}


static int ecrire_entier_inverse(FILE* file, const void* value, size_t size) {
    void* inversé = inverser_ordre(value, size, 1);
    if (inversé == NULL) {
        return -1;
    }
    
    int result = fwrite(inversé, size, 1, file) == 1 ? 0 : -1;
    free(inversé);
    
    return result;
}


static int ecrire_double_inverse(FILE* file, double value) {
    return ecrire_entier_inverse(file, &value, sizeof(double));
}

//  FONCTION sauvegarder_etudiant_binaire 

int save_student(FILE *file, Student *student) {
    if (student == NULL) return -1;
    
    if (fwrite(&student->id, sizeof(int), 1, file) != 1) return -1;
    if (fwrite(&student->age, sizeof(int), 1, file) != 1) return -1;
    if (fwrite(&student->overall_average, sizeof(double), 1, file) != 1) return -1;
    if (save_string(file, student->first_name) != 0) return -1;
    if (save_string(file, student->last_name) != 0) return -1;
    if (fwrite(&student->courses_count, sizeof(int), 1, file) != 1) return -1;
    
    for (int i = 0; i < student->courses_count; i++) {
        if (save_course(file, student->courses[i]) != 0) return -1;
    }
    
    return 0;
}

Student* load_student(FILE *file) {
    Student *student = (Student*)malloc(sizeof(Student));
    if (student == NULL) return NULL;
    
    if (fread(&student->id, sizeof(int), 1, file) != 1) {
        free(student);
        return NULL;
    }
    
    if (fread(&student->age, sizeof(int), 1, file) != 1) {
        free(student);
        return NULL;
    }
    
    if (fread(&student->overall_average, sizeof(double), 1, file) != 1) {
        free(student);
        return NULL;
    }
    
    student->first_name = load_string(file);
    if (student->first_name == NULL) {
        free(student);
        return NULL;
    }
    
    student->last_name = load_string(file);
    if (student->last_name == NULL) {
        free(student->first_name);
        free(student);
        return NULL;
    }
    
    if (fread(&student->courses_count, sizeof(int), 1, file) != 1) {
        free(student->first_name);
        free(student->last_name);
        free(student);
        return NULL;
    }
    
    if (student->courses_count > 0) {
        student->courses = (Course**)malloc(student->courses_count * sizeof(Course*));
        if (student->courses == NULL) {
            free(student->first_name);
            free(student->last_name);
            free(student);
            return NULL;
        }
        
        for (int i = 0; i < student->courses_count; i++) {
            student->courses[i] = load_course(file);
            if (student->courses[i] == NULL) {
                for (int j = 0; j < i; j++) {
                    free_course(student->courses[j]);
                }
                free(student->courses);
                free(student->first_name);
                free(student->last_name);
                free(student);
                return NULL;
            }
        }
    } else {
        student->courses = NULL;
    }
    
    return student;
}
    
    FILE* verify_file = fopen(filename, "rb");
    if (verify_file != NULL) {
        fseek(verify_file, 0, SEEK_END);
        long file_size = ftell(verify_file);
        fseek(verify_file, 0, SEEK_SET);
        
        uint8_t* file_data = malloc(file_size);
        if (file_data != NULL) {
            fread(file_data, 1, file_size, verify_file);
            afficher_octets_hex(file_data, file_size, "Contenu du fichier");
            free(file_data);
        }
        
        printf("   Taille du fichier : %ld octets\n", file_size);
        fclose(verify_file);
    }
    
    return 0;
}
