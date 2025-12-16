# Makefile 

# Compilateur et options
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
LDFLAGS = 

# Répertoires
SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj
BIN_DIR = bin

# Fichiers source principaux
MAIN_SRC = $(SRC_DIR)/main.c
LIBS_SRC = \
	$(SRC_DIR)/core.c \
	$(SRC_DIR)/grades.c \
	$(SRC_DIR)/course.c \
	$(SRC_DIR)/student.c \
	$(SRC_DIR)/class.c \
	$(SRC_DIR)/file_io.c \
	$(SRC_DIR)/api.c \
	$(SRC_DIR)/sorting.c \
	$(SRC_DIR)/binary_fields.c

# Fichiers d'en-tête correspondants
LIBS_HEADERS = \
	$(INC_DIR)/core.h \
	$(INC_DIR)/grades.h \
	$(INC_DIR)/course.h \
	$(INC_DIR)/student.h \
	$(INC_DIR)/class.h \
	$(INC_DIR)/file_io.h \
	$(INC_DIR)/api.h \
	$(INC_DIR)/sorting.h \
	$(INC_DIR)/binary_fields.h

# Fichiers objets
MAIN_OBJ = $(OBJ_DIR)/main.o
LIBS_OBJ = $(LIBS_SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Exécutable final
TARGET = $(BIN_DIR)/student_management_system

# Règles par défaut
.PHONY: all clean directories run test

all: directories $(TARGET)

# Créer les répertoires nécessaires
directories:
	@mkdir -p $(OBJ_DIR) $(BIN_DIR) $(SRC_DIR) $(INC_DIR)

# Lien final
$(TARGET): $(MAIN_OBJ) $(LIBS_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Compilation du main
$(OBJ_DIR)/main.o: $(MAIN_SRC) $(LIBS_HEADERS)
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $< -o $@

# Compilation des bibliothèques
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(INC_DIR)/%.h
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $< -o $@

# Exécution
run: all
	@echo "Exécution du programme..."
	@./$(TARGET) data/data.txt || echo "Spécifiez un fichier de données : make run FILE=data.txt"

# Test avec données d'exemple
test: all
	@echo "Test avec les données d'exemple..."
	@./$(TARGET) data/data.txt

# Nettoyage
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
	find . -name "*.o" -type f -delete
	find . -name "*.out" -type f -delete

# Nettoyage complet
distclean: clean
	rm -rf *~ core *.core

# Aide
help:
	@echo "Commandes disponibles:"
	@echo "  make all       - Compile le programme complet"
	@echo "  make clean     - Nettoie les fichiers objets et l'exécutable"
	@echo "  make run       - Compile et exécute (spécifiez FILE=...)"
	@echo "  make test      - Test avec données d'exemple"
	@echo "  make directories - Crée les répertoires nécessaires"
	@echo ""
	@echo "Exemples:"
	@echo "  make FILE=data/students.csv run"
	@echo "  make test"
