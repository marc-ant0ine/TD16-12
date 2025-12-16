#ifndef BINARY_FIELDS_H
#define BINARY_FIELDS_H

#include "student.h"
#include "course.h"

// Énumération des matières
typedef enum {
    MATHS = 0,
    FRANCAIS,
    HISTOIRE,
    GEOGRAPHIE,
    CHIMIE,
    PHYSIQUE,
    ANGLAIS,
    ESPAGNOL,
    ALLEMAND,
    SPORT,
    PHILOSOPHIE,
    BIOLOGIE,
    INFORMATIQUE,
    ECONOMIE,
    ART,
    MUSIQUE,
    COURSE_COUNT
} CourseIndex;

// Masques binaires pour les pôles
#define SCIENCES_MASK ((1ULL << MATHS) | (1ULL << CHIMIE) | (1ULL << PHYSIQUE) | \
                      (1ULL << BIOLOGIE) | (1ULL << INFORMATIQUE))
#define HUMANITIES_MASK ((1ULL << FRANCAIS) | (1ULL << HISTOIRE) | \
                        (1ULL << GEOGRAPHIE) | (1ULL << PHILOSOPHIE))
#define LANGUAGES_MASK ((1ULL << ANGLAIS) | (1ULL << ESPAGNOL) | (1ULL << ALLEMAND))
#define ARTS_SPORTS_MASK ((1ULL << ART) | (1ULL << MUSIQUE) | (1ULL << SPORT))
#define ECONOMICS_MASK ((1ULL << ECONOMIE))

// Fonctions de mapping
CourseIndex get_course_index(const char *course_name);
const char* get_course_name_from_index(CourseIndex index);

// Gestion des validations binaires
void set_course_validation(Student *student, CourseIndex course_index, int is_validated);
int is_course_validated(const Student *student, CourseIndex course_index);
int count_validated_courses(const Student *student);
void update_all_validations(Student *student);

// Système de pôles
int count_validated_by_pole(const Student *student, unsigned long long pole_mask);
int count_courses_in_pole(const Student *student, unsigned long long pole_mask);
int is_pole_fully_validated(const Student *student, unsigned long long pole_mask);
double calculate_pole_average(const Student *student, unsigned long long pole_mask);

// Affichage des résultats par pôle
void display_poles_statistics(Class *class);
void API_display_results_per_field(Class* pClass);

#endif
