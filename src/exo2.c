#include <exo2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>



// EXERCICE 2 : TABLEAU DE PRÉNOMS 


char** listePrenoms(Class* promotion, int* taille) {
    // Vérification des paramètres
    if (promotion == NULL || taille == NULL) {
        return NULL;
    }
    
    
    if (promotion->students_count == 0) {
        *taille = 0;
        char** result = (char**)malloc(sizeof(char*));
        if (result != NULL) {
            result[0] = NULL;
        }
        return result;
    }
    
    
    size_t taille_totale = 0;
    
    for (int i = 0; i < promotion->students_count; i++) {
        Student* etudiant = promotion->students[i];
        if (etudiant != NULL && etudiant->first_name != NULL) {
            
            taille_totale += strlen(etudiant->first_name) + 1;
        }
    }
    
    if (taille_totale == 0) {
        *taille = 0;
        char** result = (char**)malloc(sizeof(char*));
        if (result != NULL) {
            result[0] = NULL;
        }
        return result;
    }
    
   

    
    char** tableau = (char**)malloc(promotion->students_count * sizeof(char*) + 
                                    taille_totale * sizeof(char));
    
    if (tableau == NULL) {
        *taille = 0;
        return NULL;
    }
    
   
    char* zone_chaines = (char*)(tableau + promotion->students_count);
    char* current_pos = zone_chaines;
    int index_valide = 0;
    
   
    for (int i = 0; i < promotion->students_count; i++) {
        Student* etudiant = promotion->students[i];
        
        if (etudiant != NULL && etudiant->first_name != NULL) {
            // Pointer vers l'emplacement actuel dans la zone des chaînes
            tableau[index_valide] = current_pos;
            
            // Copier et convertir en majuscules
            char* source = etudiant->first_name;
            char* dest = current_pos;
            
            while (*source) {
                *dest = toupper((unsigned char)*source);
                source++;
                dest++;
            }
            *dest = '\0'; // Terminer la chaîne
            
            // Avancer le pointeur de la position courante
            current_pos = dest + 1;
            
            index_valide++;
        }
    }
    
    // Mettre à jour la taille réelle (nombre d'étudiants valides)
    *taille = index_valide;
    
    return tableau;
}






void afficher_info_memoire(char** tableau, int taille) {
    if (tableau == NULL) return;
    
    printf("\n=== INFORMATIONS SUR LA MÉMOIRE ===\n");
    
    // Adresse du tableau de pointeurs
    printf("Adresse du tableau de pointeurs: %p\n", (void*)tableau);
    
    // Adresse de chaque pointeur et de sa chaîne
    for (int i = 0; i < taille; i++) {
        printf("  Pointeur[%d]: %p -> Chaîne: %p ('%s')\n", 
               i, (void*)&tableau[i], (void*)tableau[i], tableau[i]);
    }
    
    // Vérifier si les chaînes sont contiguës
    if (taille > 1) {
        printf("\nVérification de la contiguïté:\n");
        for (int i = 0; i < taille - 1; i++) {
            char* fin_chaines = tableau[i] + strlen(tableau[i]) + 1;
            if (fin_chaines == tableau[i + 1]) {
                printf("  Chaîne[%d] et Chaîne[%d]: CONTIGUËS ✓\n", i, i + 1);
            } else {
                printf("  Chaîne[%d] et Chaîne[%d]: NON CONTIGUËS ✗\n", i, i + 1);
            }
        }
    }
    
    // Calculer la taille totale allouée
    size_t taille_pointeurs = taille * sizeof(char*);
    size_t taille_chaines = 0;
    for (int i = 0; i < taille; i++) {
        taille_chaines += strlen(tableau[i]) + 1;
    }
    
    printf("\nTaille allouée:\n");
    printf("  - Tableau de pointeurs: %zu octets\n", taille_pointeurs);
    printf("  - Zone des chaînes: %zu octets\n", taille_chaines);
    printf("  - Total: %zu octets\n", taille_pointeurs + taille_chaines);
}





    
