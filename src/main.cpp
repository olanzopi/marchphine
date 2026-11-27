#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <shader.h>

#include <fstream>

#include "gameloop.h"
#include "logger.h"

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    gMouse.x = static_cast<int>(xpos);
    gMouse.y = static_cast<int>(ypos);
}

int main() {
    log_debug("hi!");

    if(!glfwInit()) {
        log_fatal("glfw init error");
        return 1;
    }

    GLFWwindow *window = glfwCreateWindow(1200, 800, "marchphine", NULL, NULL);
    glfwMakeContextCurrent(window);

    //vsync 0=off, comment=on
    //glfwSwapInterval(0);

    if (!gladLoadGL()) {
        log_fatal("opengl init error");
        return -1;
    }

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    if (glfwRawMouseMotionSupported())
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE); 

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    main_game(window);

    log_debug("goodbye!");
    
    glfwDestroyWindow(window);
    glfwTerminate();    

    return 0;
}