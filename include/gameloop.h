#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <shader.h>

void main_game(GLFWwindow* window);

struct MouseData {
    int x;
    int y;
};

extern MouseData gMouse;