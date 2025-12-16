#include "binary_fields.h"
#include <string.h>
#include <ctype.h>
#include <stdio.h>

CourseIndex get_course_index(const char *course_name) {
    if (course_name == NULL) return COURSE_COUNT;
    
    char lower_name[256];
    strncpy(lower_name, course_name, 255);
    lower_name[255] = '\0';
    for (int i = 0; lower_name[i]; i++) {
        lower_name[i] = tolower(lower_name[i]);
    }
    
    if (strstr(lower_name, "math")) return MATHS;
    if (strstr(lower_name, "francais") || strstr(lower_name, "français")) return FRANCAIS;
    if (strstr(lower_name, "histoire")) return HISTOIRE;
    if (strstr(lower_name, "geographie") || strstr(lower_name, "géographie")) return GEOGRAPHIE;
    if (strstr(lower_name, "chimie")) return CHIMIE;
    if (strstr(lower_name, "physique")) return PHYSIQUE;
    if (strstr(lower_name, "anglais")) return ANGLAIS;
    if (strstr(lower_name, "espagnol")) return ESPAGNOL;
    if (strstr(lower_name, "allemand")) return ALLEMAND;
    if (strstr(lower_name, "sport") || strstr(lower_name, "eps")) return SPORT;
    if (strstr(lower_name, "philosophie") || strstr(lower_name, "philo")) return PHILOSOPHIE;
    if (strstr(lower_name, "biologie") || strstr(lower_name, "bio")) return BIOLOGIE;
    if (strstr(lower_name, "informatique") || strstr(lower_name, "info")) return INFORMATIQUE;
    if (strstr(lower_name, "economie") || strstr(lower_name, "économie") || strstr(lower_name, "eco")) return ECONOMIE;
    if (strstr(lower_name, "art")) return ART;
    if (strstr(lower_name, "musique")) return MUSIQUE;
    
    return COURSE_COUNT;
}

const char* get_course_name_from_index(CourseIndex index) {
    switch(index) {
        case MATHS: return "Mathématiques";
        case FRANCAIS: return "Français";
        case HISTOIRE: return "Histoire";
        case GEOGRAPHIE: return "Géographie";
        case CHIMIE: return "Chimie";
        case PHYSIQUE: return "Physique";
        case ANGLAIS: return "Anglais";
        case ESPAGNOL: return "Espagnol";
        case ALLEMAND: return "Allemand";
        case SPORT: return "Sport";
        case PHILOSOPHIE: return "Philosophie";
        case BIOLOGIE: return "Biologie";
        case INFORMATIQUE: return "Informatique";
        case ECONOMIE: return "Économie";
        case ART: return "Art";
        case MUSIQUE: return "Musique";
        default: return "Inconnue";
    }
}

void set_course_validation(Student *student, CourseIndex course_index, int is_validated) {
    if (student == NULL || course_index >= 64) return;
    
    if (is_validated) {
        student->validated_courses |= (1ULL << course_index);
    } else {
        student->validated_courses &= ~(1ULL << course_index);
    }
}

int is_course_validated(const Student *student, CourseIndex course_index) {
    if (student == NULL || course_index >= 64) return 0;
    
    return (student->validated_courses & (1ULL << course_index)) != 0;
}

int count_validated_courses(const Student *student) {
    if (student == NULL) return 0;
    
    int count = 0;
    unsigned long long mask = student->validated_courses;
    
    while (mask) {
        count += mask & 1;
        mask >>= 1;
    }
    
    return count;
}

void update_all_validations(Student *student) {
    if (student == NULL) return;
    
    student->validated_courses = 0;
    
    for (int i = 0; i < student->courses_count; i++) {
        Course *course = student->courses[i];
        if (course->notes.size > 0) {
            int is_validated = (course->average >= 10.0) ? 1 : 0;
            set_course_validation(student, course->index, is_validated);
        }
    }
}

int count_validated_by_pole(const Student *student, unsigned long long pole_mask) {
    if (student == NULL) return 0;
    
    unsigned long long validated_in_pole = student->validated_courses & pole_mask;
    
    int count = 0;
    while (validated_in_pole) {
        count += validated_in_pole & 1;
        validated_in_pole >>= 1;
    }
    
    return count;
}

int count_courses_in_pole(const Student *student, unsigned long long pole_mask) {
    if (student == NULL) return 0;
    
    int count = 0;
    
    for (int i = 0; i < student->courses_count; i++) {
        Course *course = student->courses[i];
        if (pole_mask & (1ULL << course->index)) {
            count++;
        }
    }
    
    return count;
}

static int check_mask_validated(const Student *student, unsigned long long mask) {
    if (student == NULL) return 0;
    
    return ((student->validated_courses & mask) == mask);
}

int is_pole_fully_validated(const Student *student, unsigned long long pole_mask) {
    if (student == NULL) return 0;
    
    unsigned long long student_courses_in_pole = 0;
    for (int i = 0; i < student->courses_count; i++) {
        Course *course = student->courses[i];
        if (pole_mask & (1ULL << course->index)) {
            student_courses_in_pole |= (1ULL << course->index);
        }
    }
    
    if (student_courses_in_pole == 0) return 0;
    
    return check_mask_validated(student, student_courses_in_pole);
}

double calculate_pole_average(const Student *student, unsigned long long pole_mask) {
    if (student == NULL) return 0.0;
    
    double sum_weighted = 0.0;
    double sum_coefficients = 0.0;
    
    for (int i = 0; i < student->courses_count; i++) {
        Course *course = student->courses[i];
        if ((pole_mask & (1ULL << course->index)) && course->notes.size > 0) {
            sum_weighted += course->average * course->coefficient;
            sum_coefficients += course->coefficient;
        }
    }
    
    if (sum_coefficients > 0) {
        return sum_weighted / sum_coefficients;
    }
    
    return 0.0;
}

void display_poles_statistics(Class *class) {
    if (class == NULL || class->students_count == 0) return;
    
    printf("\n=== CLASS STATISTICS BY POLES ===\n");
    
    struct {
        unsigned long long mask;
        const char *name;
    } poles[] = {
        {SCIENCES_MASK, "Sciences"},
        {HUMANITIES_MASK, "Humanités"},
        {LANGUAGES_MASK, "Langues"},
        {ARTS_SPORTS_MASK, "Arts et Sport"},
        {ECONOMICS_MASK, "Économie"}
    };
    
    for (int p = 0; p < 5; p++) {
        printf("\n  Pôle: %s\n", poles[p].name);
        printf("  %s\n", "-----------------------------------");
        
        int students_in_pole = 0;
        double sum_pole_averages = 0.0;
        int fully_validated_count = 0;
        
        for (int i = 0; i < class->students_count; i++) {
            Student *student = class->students[i];
            int courses_in_pole = count_courses_in_pole(student, poles[p].mask);
            
            if (courses_in_pole > 0) {
                students_in_pole++;
                double pole_avg = calculate_pole_average(student, poles[p].mask);
                sum_pole_averages += pole_avg;
                
                if (is_pole_fully_validated(student, poles[p].mask)) {
                    fully_validated_count++;
                }
            }
        }
        
        if (students_in_pole > 0) {
            printf("  Students enrolled: %d\n", students_in_pole);
            printf("  Average grade: %.2f/20\n", sum_pole_averages / students_in_pole);
            printf("  Students with pole fully validated: %d/%d (%.1f%%)\n", 
                   fully_validated_count, 
                   students_in_pole,
                   (fully_validated_count * 100.0) / students_in_pole);
        } else {
            printf("  No students enrolled in this pole\n");
        }
    }
    
    printf("\n==================================\n");
}

#define YEAR_MASK (SCIENCES_MASK | HUMANITIES_MASK | LANGUAGES_MASK | \
                   ARTS_SPORTS_MASK | ECONOMICS_MASK)

int is_year_validated(const Student *student) {
    if (student == NULL || student->courses_count == 0) return 0;
    
    unsigned long long student_courses_mask = 0;
    for (int i = 0; i < student->courses_count; i++) {
        student_courses_mask |= (1ULL << student->courses[i]->index);
    }
    
    return check_mask_validated(student, student_courses_mask);
}

void API_display_results_per_field(Class* pClass) {
    if (pClass == NULL || pClass->students_count == 0) {
        printf("No students to display\n");
        return;
    }
    
    printf("\n╔════════════════════════════════════════════════════════════════════╗\n");
    printf("║           RÉSULTATS DES ÉTUDIANTS PAR PÔLES                       ║\n");
    printf("╚════════════════════════════════════════════════════════════════════╝\n");
    
    struct {
        unsigned long long mask;
        const char *name;
        const char *short_name;
    } poles[] = {
        {SCIENCES_MASK,     "Sciences",         "SCI"},
        {HUMANITIES_MASK,   "Humanités",        "HUM"},
        {LANGUAGES_MASK,    "Langues",          "LAN"},
        {ARTS_SPORTS_MASK,  "Arts et Sport",    "ART"},
        {ECONOMICS_MASK,    "Économie",         "ECO"}
    };
    const int nb_poles = 5;
    
    for (int i = 0; i < pClass->students_count; i++) {
        Student *student = pClass->students[i];
        
        printf("\n┌────────────────────────────────────────────────────────────────────┐\n");
        printf("│ Étudiant: %-20s %-20s (ID: %4d) │\n", 
               student->first_name, student->last_name, student->id);
        printf("├────────────────────────────────────────────────────────────────────┤\n");
        
        if (student->courses_count == 0) {
            printf("│ Aucune matière inscrite                                            │\n");
            printf("└────────────────────────────────────────────────────────────────────┘\n");
            continue;
        }
        
        printf("│ Moyenne Générale: %5.2f/20                                        │\n", 
               student->overall_average);
        printf("├────────────────────────────────────────────────────────────────────┤\n");
        
        int total_poles_student_has = 0;
        int poles_validated = 0;
        
        printf("│ %-20s │ %8s │ %8s │ %10s │\n", 
               "PÔLE", "Moyenne", "Matières", "STATUT");
        printf("├────────────────────────────────────────────────────────────────────┤\n");
        
        for (int p = 0; p < nb_poles; p++) {
            int courses_in_pole = count_courses_in_pole(student, poles[p].mask);
            
            if (courses_in_pole == 0) {
                continue;
            }
            
            total_poles_student_has++;
            
            double pole_average = calculate_pole_average(student, poles[p].mask);
            int validated_in_pole = count_validated_by_pole(student, poles[p].mask);
            int pole_fully_validated = is_pole_fully_validated(student, poles[p].mask);
            
            if (pole_fully_validated) {
                poles_validated++;
            }
            
            printf("│ %-20s │ %6.2f/20 │  %2d/%-2d   │ ", 
                   poles[p].name, pole_average, validated_in_pole, courses_in_pole);
            
            if (pole_fully_validated) {
                printf("✓ VALIDÉ   │\n");
            } else {
                printf("✗ NON VALIDÉ│\n");
            }
        }
        
        printf("├────────────────────────────────────────────────────────────────────┤\n");
        
        int year_validated = is_year_validated(student);
        
        printf("│ VALIDATION DE L'ANNÉE:                                             │\n");
        printf("│   Pôles validés: %d/%d                                              │\n", 
               poles_validated, total_poles_student_has);
        printf("│   Matières validées: %d/%d                                          │\n", 
               count_validated_courses(student), student->courses_count);
        printf("│                                                                    │\n");
        
        if (year_validated) {
            printf("│   ╔════════════════════════════════════════════════════════╗     │\n");
            printf("│   ║  ✓✓✓ ANNÉE COMPLÈTEMENT VALIDÉE ✓✓✓                  ║     │\n");
            printf("│   ╚════════════════════════════════════════════════════════╝     │\n");
        } else {
            printf("│   ╔════════════════════════════════════════════════════════╗     │\n");
            printf("│   ║  ✗ ANNÉE NON VALIDÉE                                  ║     │\n");
            printf("│   ╚════════════════════════════════════════════════════════╝     │\n");
        }
        
        printf("└────────────────────────────────────────────────────────────────────┘\n");
    }
    
    printf("\n╔════════════════════════════════════════════════════════════════════╗\n");
    printf("║                  STATISTIQUES GLOBALES DE LA CLASSE                ║\n");
    printf("╚════════════════════════════════════════════════════════════════════╝\n");
    
    int total_students_with_year_validated = 0;
    
    for (int i = 0; i < pClass->students_count; i++) {
        if (is_year_validated(pClass->students[i])) {
            total_students_with_year_validated++;
        }
    }
    
    printf("\n  Étudiants ayant validé leur année: %d/%d (%.1f%%)\n",
           total_students_with_year_validated,
           pClass->students_count,
           pClass->students_count > 0 ? 
               (total_students_with_year_validated * 100.0) / pClass->students_count : 0.0);
    
    struct {
        unsigned long long mask;
        const char *name;
    } poles_stats[] = {
        {SCIENCES_MASK,     "Sciences"},
        {HUMANITIES_MASK,   "Humanités"},
        {LANGUAGES_MASK,    "Langues"},
        {ARTS_SPORTS_MASK,  "Arts et Sport"},
        {ECONOMICS_MASK,    "Économie"}
    };
    
    for (int p = 0; p < 5; p++) {
        int students_with_pole = 0;
        int students_validated_pole = 0;
        
        for (int i = 0; i < pClass->students_count; i++) {
            Student *student = pClass->students[i];
            if (count_courses_in_pole(student, poles_stats[p].mask) > 0) {
                students_with_pole++;
                if (is_pole_fully_validated(student, poles_stats[p].mask)) {
                    students_validated_pole++;
                }
            }
        }
        
        if (students_with_pole > 0) {
            printf("    • %-15s : %2d/%2d étudiants (%.1f%%)\n",
                   poles_stats[p].name,
                   students_validated_pole,
                   students_with_pole,
                   (students_validated_pole * 100.0) / students_with_pole);
        }
    }
    
    printf("\n════════════════════════════════════════════════════════════════════\n");
}
