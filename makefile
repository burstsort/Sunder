#Output name
TARGET = bin/sunder.exe

#Search directories
IDIR = -I./inc -I./extlibs/glfw/include -I./extlibs/gl3w/include -I./extlibs/linmath.h
LDIR = -L./extlibs/glfw/lib -L./extlibs/gl3w/lib

#Project Files
SRC = src/main.cpp src/Vertex.cpp src/EarClipping.cpp src/Geometry.cpp src/Mesh.cpp src/Texture.cpp src/Shader.cpp src/Fragment.cpp src/Model.cpp src/Camera.cpp src/Knife.cpp src/Scene.cpp

#Flags
CC = g++
CFLAGS = -static-libgcc -static-libstdc++ -Wall -static -g
LFLAGS = -lgl3w -lglfw3 -lopengl32 -lgdi32

rule:
	@echo Building...
	@$(CC) $(SRC) $(IDIR) $(LDIR) $(CFLAGS) $(LFLAGS) -o $(TARGET)
	@echo Done!