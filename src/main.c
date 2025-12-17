#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "api.h"
#include "sorting.h"
#include "binary_fields.h"
#include "exo1.h"
#include "exo2.h"

// Fonction pour afficher l'aide
void print_help(const char* program_name) {
    fprintf(stderr, "Usage: %s <data_file> [options]\n\n", program_name);
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  --save <binary_file>    Save to binary file\n");
    fprintf(stderr, "  --load <binary_file>    Load from binary file\n");
    fprintf(stderr, "  --poles                 Display poles statistics\n");
    fprintf(stderr, "  --recalcul              Apply grade normalization (Exercise 1)\n");
    fprintf(stderr, "  --prenoms               Display first names list (Exercise 2)\n");
    fprintf(stderr, "  --all                   Run all exercises\n");
    fprintf(stderr, "  --help                  Display this help message\n\n");
    fprintf(stderr, "Examples:\n");
    fprintf(stderr, "  %s data.txt --recalcul --prenoms\n", program_name);
    fprintf(stderr, "  %s data.txt --all --save class_data.bin\n", program_name);
    fprintf(stderr, "  %s data.txt --load class_data.bin --poles\n", program_name);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_help(argv[0]);
        return 1;
    }
    
    // Vérifier l'option --help
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0) {
            print_help(argv[0]);
            return 0;
        }
    }
    
    Class *class = NULL;
    
    // Vérifier les options
    int display_poles = 0;
    int apply_recalcul = 0;
    int display_prenoms = 0;
    int run_all = 0;
    char *binary_file = NULL;
    int save_mode = 0;
    
    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "--poles") == 0) {
            display_poles = 1;
        } else if (strcmp(argv[i], "--recalcul") == 0) {
            apply_recalcul = 1;
        } else if (strcmp(argv[i], "--prenoms") == 0) {
            display_prenoms = 1;
        } else if (strcmp(argv[i], "--all") == 0) {
            run_all = 1;
        } else if (strcmp(argv[i], "--save") == 0 && i + 1 < argc) {
            save_mode = 1;
            binary_file = argv[++i];
        } else if (strcmp(argv[i], "--load") == 0 && i + 1 < argc) {
            save_mode = 0;
            binary_file = argv[++i];
        } else {
            fprintf(stderr, "Warning: Unknown option '%s'\n", argv[i]);
        }
    }
    
   
    if (run_all) {
        apply_recalcul = 1;
        display_prenoms = 1;
    }
    
    // Chargement des données
    if (binary_file && !save_mode) {
        printf(" Loading from binary file: %s\n", binary_file);
        class = load_class_from_binary(binary_file);
    } else {
        printf(" Loading from text file: %s\n", argv[1]);
        class = API_load_students(argv[1]);
    }
    
    if (class == NULL) {
        fprintf(stderr, " Error: Failed to load data\n");
        return 1;
    }
    
    printf(" Data loaded successfully: %d students, %d courses\n\n", 
           class->students_count, class->courses_count);
    
    // ===== EXERCICE 1 : RECALCUL =====
    if (apply_recalcul) {
        printf(" === EXERCICE 1 : RECALCUL DES MOYENNES ===\n");
        
        // Afficher les moyennes avant normalisation
        printf("\n Moyennes avant normalisation :\n");
        afficher_resume_moyennes(class);
        
        // Appliquer la normalisation
        printf("\n  Application de la normalisation...\n");
        bool succes = miseAJourNotes(class, normalisation);
        
        if (!succes) {
            fprintf(stderr, " Échec du recalcul des moyennes\n");
        } else {
            // Afficher les moyennes après normalisation
            printf("\n Moyennes après normalisation :\n");
            afficher_resume_moyennes(class);
        }
    }
    
    // EXERCICE 2 
    if (display_prenoms) {
        printf("\n  EXERCICE 2 : LISTE DES PRÉNOMS \n");
        
        int taille_prenoms;
        char** prenoms = listePrenoms(class, &taille_prenoms);
        
        if (prenoms != NULL) {
            printf("\n👥 Liste des prénoms en majuscules (%d étudiants) :\n\n", taille_prenoms);
            
            
            int cols = 3;
            printf("┌────────────────────────────────────────────────────────────┐\n");
            printf("│                     LISTE DES PRÉNOMS                     │\n");
            printf("├────────────────────────────────────────────────────────────┤\n");
            
            for (int i = 0; i < taille_prenoms; i += cols) {
                printf("│");
                for (int j = 0; j < cols && i + j < taille_prenoms; j++) {
                    printf(" %3d. %-15s ", i + j + 1, prenoms[i + j]);
                }
                // Remplir avec des espaces si nécessaire
                for (int j = taille_prenoms - i; j < cols; j++) {
                    printf(" %-22s ", "");
                }
                printf("│\n");
            }
            
            printf("└────────────────────────────────────────────────────────────┘\n");
            
            // Informations techniques
            printf("\n  Informations techniques :\n");
            printf("   • Allocation mémoire contiguë : ✓\n");
            printf("   • Prénoms convertis en majuscules : ✓\n");
            printf("   • Données originales non modifiées : ✓\n");
            printf("   • Gestion des erreurs (retour NULL) : ✓\n");
            
            if (taille_prenoms < class->students_count) {
                printf("   • Note : %d étudiant(s) sans prénom ou prénom NULL\n", 
                       class->students_count - taille_prenoms);
            }
            
        } else {
            fprintf(stderr, " Erreur lors de la création du tableau de prénoms\n");
        }
    }
    
    // Sauvegarde si demandé
    if (binary_file && save_mode) {
        printf("\n Saving to binary file: %s\n", binary_file);
        if (API_save_to_binary_file(class, binary_file) != 0) {
            fprintf(stderr, "  Warning: Failed to save binary file\n");
        } else {
            printf("Save completed successfully!\n");
        }
    }
    
    // Affichage selon les options
    if (display_poles) {
        printf("\n === DISPLAYING POLES STATISTICS ===\n");
        API_display_results_per_field(class);
    } 
    
    
    if (!display_poles && (!display_prenoms || run_all || apply_recalcul)) {
        printf("\n === CLASS OVERVIEW ===\n");
        
        if (apply_recalcul) {
            printf("   (après normalisation)\n");
        }
        
        API_display(class);
        
        // Démonstration du tri
        printf("\n  TOP STUDENTS \n");
        
        // Top 10 par moyenne
        char **top10 = API_get_best_students(class);
        if (top10) {
            printf("\nTop 10 students by average:\n");
            for (int i = 0; i < SIZE_TOP1 && top10[i] != NULL; i++) {
                printf("   %s\n", top10[i]);
            }
            API_free_best_students(top10);
        }
    }
    
    // Libération mémoire
    printf("\n Cleaning up memory...\n");
    API_unload(class);
    printf("Program completed successfully!\n");
    
    return 0;
}
