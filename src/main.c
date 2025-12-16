#include <stdio.h>
#include <stdlib.h>
#include "api.h"
#include "sorting.h"
#include "binary_fields.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <data_file> [options]\n", argv[0]);
        fprintf(stderr, "Options:\n");
        fprintf(stderr, "  --save <binary_file>    Save to binary file\n");
        fprintf(stderr, "  --load <binary_file>    Load from binary file\n");
        fprintf(stderr, "  --poles                 Display poles statistics\n");
        return 1;
    }
    
    Class *class = NULL;
    
    // Vérifier les options
    int display_poles = 0;
    char *binary_file = NULL;
    int save_mode = 0;
    
    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "--poles") == 0) {
            display_poles = 1;
        } else if (strcmp(argv[i], "--save") == 0 && i + 1 < argc) {
            save_mode = 1;
            binary_file = argv[++i];
        } else if (strcmp(argv[i], "--load") == 0 && i + 1 < argc) {
            save_mode = 0;
            binary_file = argv[++i];
        }
    }
    
    // Chargement des données
    if (binary_file && !save_mode) {
        printf("Loading from binary file: %s\n", binary_file);
        class = load_class_from_binary(binary_file);
    } else {
        printf("Loading from text file: %s\n", argv[1]);
        class = API_load_students(argv[1]);
    }
    
    if (class == NULL) {
        fprintf(stderr, "Error: Failed to load data\n");
        return 1;
    }
    
    // Sauvegarde si demandé
    if (binary_file && save_mode) {
        printf("\nSaving to binary file: %s\n", binary_file);
        if (API_save_to_binary_file(class, binary_file) != 0) {
            fprintf(stderr, "Warning: Failed to save binary file\n");
        }
    }
    
    // Affichage selon les options
    if (display_poles) {
        printf("\n=== DISPLAYING POLES STATISTICS ===\n");
        API_display_results_per_field(class);
    } else {
        API_display(class);
        
        // Démonstration du tri
        printf("\n=== DEMONSTRATION OF SORTING ===\n");
        
        // Top 10 par moyenne
        char **top10 = API_get_best_students(class);
        if (top10) {
            printf("\nTop 10 students by average:\n");
            for (int i = 0; i < SIZE_TOP1 && top10[i] != NULL; i++) {
                printf("  %s\n", top10[i]);
            }
            API_free_best_students(top10);
        }
    }
    
    // Libération mémoire
    API_unload(class);
    
    return 0;
}
