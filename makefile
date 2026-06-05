# ============================================================
#  Makefile - Proyecto Juego PvP Metroidvania (Windows/SFML)
#  Requiere SFML instalado en C:/SFML
#  Ajusta SFML_DIR si lo tienes en otra ruta
# ============================================================

CXX      = g++
CXXFLAGS = -std=c++17 -Wall -O2
SFML_DIR = C:/SFML

INCLUDES = -Iinclude -I$(SFML_DIR)/include
LIBS     = -L$(SFML_DIR)/lib \
           -lsfml-graphics -lsfml-window -lsfml-system \
           -lopengl32 -lwinmm -lgdi32

SRC_DIR  = src
OBJ_DIR  = obj
BIN_DIR  = bin

SOURCES  = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS  = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SOURCES))
TARGET   = $(BIN_DIR)/JuegoProyecto.exe

all: dirs $(TARGET)

dirs:
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	@if not exist $(BIN_DIR) mkdir $(BIN_DIR)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	@if exist $(OBJ_DIR) rmdir /s /q $(OBJ_DIR)
	@if exist $(TARGET)  del /q $(TARGET)

run: all
	$(TARGET)

.PHONY: all dirs clean run
