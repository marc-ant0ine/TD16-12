#include "exo1.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

typedef void (*Recalcul)(Student*, double, double);

bool miseAJourNotes(Class* promotion, Recalcul fonctionRecalcul) {
    if (promotion == NULL || fonctionRecalcul == NULL || promotion->students_count == 0) {
        return false;
    }
    
    // Rechercher les moyennes générales extrêmes
    double min_moyenne = 20.0;  // Initialiser à la valeur maximale possible
    double max_moyenne = 0.0;   // Initialiser à la valeur minimale possible
    
    for (int i = 0; i < promotion->students_count; i++) {
        Student *etudiant = promotion->students[i];
        if (etudiant != NULL) {
            double moyenne = etudiant->overall_average;
            
            // Ignorer les étudiants sans moyenne (moyenne = 0.0)
            if (moyenne > 0.0) {
                if (moyenne < min_moyenne) {
                    min_moyenne = moyenne;
                }
                if (moyenne > max_moyenne) {
                    max_moyenne = moyenne;
                }
            }
        }
    }
    
    // Vérifier si on a trouvé des moyennes valides
    if (min_moyenne > max_moyenne) {
        // Cas où aucun étudiant n'a de moyenne > 0.0
        fprintf(stderr, "Erreur : Aucune moyenne valide trouvée\n");
        return false;
    }
    
    printf("Intervalle des moyennes originales : [%.2f, %.2f]\n", min_moyenne, max_moyenne);
    
    // Modifier la moyenne générale de chaque étudiant
    for (int i = 0; i < promotion->students_count; i++) {
        Student *etudiant = promotion->students[i];
        if (etudiant != NULL && etudiant->overall_average > 0.0) {
            fonctionRecalcul(etudiant, min_moyenne, max_moyenne);
        }
    }
    
    return true;
}

void normalisation(Student* etudiant, double min_moyenne, double max_moyenne) {
    if (etudiant == NULL) {
        fprintf(stderr, "Erreur : Élève NULL dans normalisation\n");
        exit(EXIT_FAILURE);
    }
    
    // Vérifier la validité des paramètres
    if (min_moyenne < 0 || max_moyenne > 20 || min_moyenne > max_moyenne) {
        fprintf(stderr, "Erreur : Intervalle invalide [%.2f, %.2f]\n", min_moyenne, max_moyenne);
        exit(EXIT_FAILURE);
    }
    
    // Vérifier si l'étudiant a une moyenne
    if (etudiant->overall_average == 0.0) {
        // Si l'étudiant n'a pas de moyenne, on ne fait rien
        return;
    }
    
    // Vérifier que la moyenne est dans l'intervalle [min_moyenne, max_moyenne]
    if (etudiant->overall_average < min_moyenne || etudiant->overall_average > max_moyenne) {
        fprintf(stderr, "Erreur : Moyenne %.2f hors de l'intervalle [%.2f, %.2f]\n", 
                etudiant->overall_average, min_moyenne, max_moyenne);
        exit(EXIT_FAILURE);
    }
    
    // Sauvegarder l'ancienne moyenne pour affichage
    double ancienne_moyenne = etudiant->overall_average;
    
    // Appliquer la normalisation : ramener de [min_moyenne, max_moyenne] à [0, 20]
    if (max_moyenne - min_moyenne == 0) {
        // Cas particulier : toutes les moyennes sont identiques
        // On met toutes les moyennes à 10 (milieu de l'intervalle)
        etudiant->overall_average = 10.0;
    } else {
        
        etudiant->overall_average = (etudiant->overall_average - min_moyenne) * 20.0 / (max_moyenne - min_moyenne);
        
        // Assurer que la moyenne reste dans [0, 20] 
        if (etudiant->overall_average < 0.0) etudiant->overall_average = 0.0;
        if (etudiant->overall_average > 20.0) etudiant->overall_average = 20.0;
    }
    
    printf("Étudiant %s %s : %.2f -> %.2f\n", 
           etudiant->first_name, etudiant->last_name, 
           ancienne_moyenne, etudiant->overall_average);
}

void afficher_resume_moyennes(Class *promotion) {
    if (promotion == NULL) return;
    
    printf("\nRésumé des moyennes :\n");
    printf("%-5s | %-15s | %-15s | %10s\n", 
           "ID", "Prénom", "Nom", "Moyenne");
    printf("--------------------------------------------------------\n");
    
    for (int i = 0; i < promotion->students_count; i++) {
        Student *s = promotion->students[i];
        if (s != NULL) {
            printf("%-5d | %-15s | %-15s | %10.2f\n", 
                   s->id, s->first_name, s->last_name, s->overall_average);
        }
    }
}
