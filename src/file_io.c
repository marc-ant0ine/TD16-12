#include "file_io.h"
#include "class.h"
#include "core.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Class* load_class_from_file(const char *filepath) {
    FILE *file = fopen(filepath, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: cannot open file %s\n", filepath);
        return NULL;
    }
    
    Class *class = create_class();
    if (class == NULL) {
        fclose(file);
        return NULL;
    }
    
    char ligne[1024];
    enum { INIT, STUDENTS, COURSES, GRADES } state = INIT;
    
    while (fgets(ligne, sizeof(ligne), file) != NULL) {
        if (is_empty_line(ligne)) {
            continue;
        }
        
        remove_newline(ligne);
        
        if (strstr(ligne, "ETUDIANTS") != NULL) {
            state = STUDENTS;
            continue;
        } else if (strstr(ligne, "MATIERES") != NULL) {
            state = COURSES;
            continue;
        } else if (strstr(ligne, "NOTES") != NULL) {
            state = GRADES;
            continue;
        }
        
        switch (state) {
            case STUDENTS: {
                Student *student = parse_student_line(ligne);
                if (student != NULL) {
                    if (add_student_to_class(class, student) != 0) {
                        fprintf(stderr, "Error: failed to add student\n");
                        free_student(student);
                    }
                }
                break;
            }
            
            case COURSES: {
                Course *course = parse_course_line(ligne);
                if (course != NULL) {
                    Course **new_courses = (Course**)realloc(class->courses,
                                                            (class->courses_count + 1) * sizeof(Course*));
                    if (new_courses == NULL) {
                        free_course(course);
                        break;
                    }
                    class->courses = new_courses;
                    class->courses[class->courses_count] = course;
                    class->courses_count++;
                }
                break;
            }
            
            case GRADES: {
                parse_grade_line(ligne, class->students, class->students_count,
                                 class->courses, class->courses_count);
                break;
            }
            
            default:
                break;
        }
    }
    
    fclose(file);
    
    printf("Loaded: %d students, %d courses\n", class->students_count, class->courses_count);
    
    return class;
}

int save_string(FILE *file, const char *str) {
    if (str == NULL) {
        int len = 0;
        if (fwrite(&len, sizeof(int), 1, file) != 1) return -1;
        return 0;
    }
    
    int len = strlen(str) + 1;
    if (fwrite(&len, sizeof(int), 1, file) != 1) return -1;
    if (fwrite(str, sizeof(char), len, file) != len) return -1;
    
    return 0;
}

char* load_string(FILE *file) {
    int len;
    if (fread(&len, sizeof(int), 1, file) != 1) return NULL;
    
    if (len == 0) return NULL;
    
    char *str = (char*)malloc(len * sizeof(char));
    if (str == NULL) return NULL;
    
    if (fread(str, sizeof(char), len, file) != len) {
        free(str);
        return NULL;
    }
    
    return str;
}

int save_grades(FILE *file, Grades *grades) {
    if (grades == NULL) return -1;
    
    if (fwrite(&grades->size, sizeof(int), 1, file) != 1) return -1;
    
    if (grades->size > 0 && grades->data != NULL) {
        if (fwrite(grades->data, sizeof(double), grades->size, file) != grades->size) {
            return -1;
        }
    }
    
    return 0;
}

int load_grades(FILE *file, Grades *grades) {
    if (grades == NULL) return -1;
    
    if (fread(&grades->size, sizeof(int), 1, file) != 1) return -1;
    
    if (grades->size > 0) {
        grades->data = (double*)malloc(grades->size * sizeof(double));
        if (grades->data == NULL) return -1;
        
        if (fread(grades->data, sizeof(double), grades->size, file) != grades->size) {
            free(grades->data);
            return -1;
        }
    } else {
        grades->data = NULL;
    }
    
    return 0;
}

int save_course(FILE *file, Course *course) {
    if (course == NULL) return -1;
    
    if (save_string(file, course->name) != 0) return -1;
    if (fwrite(&course->coefficient, sizeof(double), 1, file) != 1) return -1;
    if (fwrite(&course->average, sizeof(double), 1, file) != 1) return -1;
    if (save_grades(file, &course->notes) != 0) return -1;
    
    return 0;
}

Course* load_course(FILE *file) {
    Course *course = (Course*)malloc(sizeof(Course));
    if (course == NULL) return NULL;
    
    course->name = load_string(file);
    if (course->name == NULL) {
        free(course);
        return NULL;
    }
    
    if (fread(&course->coefficient, sizeof(double), 1, file) != 1) {
        free(course->name);
        free(course);
        return NULL;
    }
    
    if (fread(&course->average, sizeof(double), 1, file) != 1) {
        free(course->name);
        free(course);
        return NULL;
    }
    
    if (load_grades(file, &course->notes) != 0) {
        free(course->name);
        free(course);
        return NULL;
    }
    
    return course;
}

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

int save_class_to_binary(Class *class, const char *filepath) {
    if (class == NULL || filepath == NULL) {
        fprintf(stderr, "Error: Invalid parameters for save\n");
        return -1;
    }
    
    FILE *file = fopen(filepath, "wb");
    if (file == NULL) {
        fprintf(stderr, "Error: Cannot create file %s\n", filepath);
        return -1;
    }
    
    const char signature[] = "CLASSDATA";
    if (fwrite(signature, sizeof(char), strlen(signature), file) != strlen(signature)) {
        fprintf(stderr, "Error: Failed to write signature\n");
        fclose(file);
        return -1;
    }
    
    if (fwrite(&class->students_count, sizeof(int), 1, file) != 1) {
        fprintf(stderr, "Error: Failed to write students count\n");
        fclose(file);
        return -1;
    }
    
    for (int i = 0; i < class->students_count; i++) {
        if (save_student(file, class->students[i]) != 0) {
            fprintf(stderr, "Error: Failed to save student %d\n", i);
            fclose(file);
            return -1;
        }
    }
    
    if (fwrite(&class->courses_count, sizeof(int), 1, file) != 1) {
        fprintf(stderr, "Error: Failed to write courses count\n");
        fclose(file);
        return -1;
    }
    
    for (int i = 0; i < class->courses_count; i++) {
        if (save_course(file, class->courses[i]) != 0) {
            fprintf(stderr, "Error: Failed to save course %d\n", i);
            fclose(file);
            return -1;
        }
    }
    
    fclose(file);
    printf("Class data successfully saved to %s\n", filepath);
    return 0;
}

Class* load_class_from_binary(const char *filepath) {
    if (filepath == NULL) {
        fprintf(stderr, "Error: Invalid filepath\n");
        return NULL;
    }
    
    FILE *file = fopen(filepath, "rb");
    if (file == NULL) {
        fprintf(stderr, "Error: Cannot open file %s\n", filepath);
        return NULL;
    }
    
    const char expected_signature[] = "CLASSDATA";
    char signature[10];
    if (fread(signature, sizeof(char), strlen(expected_signature), file) != strlen(expected_signature)) {
        fprintf(stderr, "Error: Failed to read signature\n");
        fclose(file);
        return NULL;
    }
    
    if (strncmp(signature, expected_signature, strlen(expected_signature)) != 0) {
        fprintf(stderr, "Error: Invalid file format\n");
        fclose(file);
        return NULL;
    }
    
    Class *class = (Class*)malloc(sizeof(Class));
    if (class == NULL) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        fclose(file);
        return NULL;
    }
    
    if (fread(&class->students_count, sizeof(int), 1, file) != 1) {
        fprintf(stderr, "Error: Failed to read students count\n");
        free(class);
        fclose(file);
        return NULL;
    }
    
    if (class->students_count > 0) {
        class->students = (Student**)malloc(class->students_count * sizeof(Student*));
        if (class->students == NULL) {
            fprintf(stderr, "Error: Memory allocation failed for students\n");
            free(class);
            fclose(file);
            return NULL;
        }
        
        for (int i = 0; i < class->students_count; i++) {
            class->students[i] = load_student(file);
            if (class->students[i] == NULL) {
                fprintf(stderr, "Error: Failed to load student %d\n", i);
                for (int j = 0; j < i; j++) {
                    free_student(class->students[j]);
                }
                free(class->students);
                free(class);
                fclose(file);
                return NULL;
            }
        }
    } else {
        class->students = NULL;
    }
    
    if (fread(&class->courses_count, sizeof(int), 1, file) != 1) {
        fprintf(stderr, "Error: Failed to read courses count\n");
        for (int i = 0; i < class->students_count; i++) {
            free_student(class->students[i]);
        }
        free(class->students);
        free(class);
        fclose(file);
        return NULL;
    }
    
    if (class->courses_count > 0) {
        class->courses = (Course**)malloc(class->courses_count * sizeof(Course*));
        if (class->courses == NULL) {
            fprintf(stderr, "Error: Memory allocation failed for courses\n");
            for (int i = 0; i < class->students_count; i++) {
                free_student(class->students[i]);
            }
            free(class->students);
            free(class);
            fclose(file);
            return NULL;
        }
        
        for (int i = 0; i < class->courses_count; i++) {
            class->courses[i] = load_course(file);
            if (class->courses[i] == NULL) {
                fprintf(stderr, "Error: Failed to load course %d\n", i);
                for (int j = 0; j < i; j++) {
                    free_course(class->courses[j]);
                }
                free(class->courses);
                for (int j = 0; j < class->students_count; j++) {
                    free_student(class->students[j]);
                }
                free(class->students);
                free(class);
                fclose(file);
                return NULL;
            }
        }
    } else {
        class->courses = NULL;
    }
    
    fclose(file);
    printf("Class data successfully loaded from %s\n", filepath);
    printf("Loaded: %d students, %d courses\n", class->students_count, class->courses_count);
    
    return class;
}
