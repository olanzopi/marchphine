#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <shader.h>
#include <gameloop.h>
#include <vertices.h>
#include <indices.h>
#include <shaderdata.h>

#define GLM_FORCE_SSE2
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/type_ptr.hpp>

void draw_object(
    GLuint MVP_Uniform, 
    GLuint Model_Uniform,
    glm::vec3 const& cam_pos,
    glm::vec2 const& Rot, float fov,
    glm::vec3 model_pos,
    glm::vec3 model_rot,
    GLint vao)  
    {
    fov = fov * (M_PI/180);
    glm::mat4 Projection = glm::perspective(fov, 1200.0f / 800.0f, 0.1f, 100.f);

    glm::vec3 forward;
    forward.x = cos(Rot.y) * sin(Rot.x);
    forward.y = sin(Rot.y);
    forward.z = cos(Rot.y) * cos(Rot.x);

    glm::mat4 View = glm::lookAt(cam_pos, cam_pos + forward, glm::vec3(0,1,0));

    glm::mat4 Model = glm::translate(glm::mat4(1.0f), model_pos);
    Model = glm::rotate(Model, glm::radians(model_rot.x), glm::vec3(1, 0, 0));
    Model = glm::rotate(Model, glm::radians(model_rot.y), glm::vec3(0, 1, 0));
    Model = glm::rotate(Model, glm::radians(model_rot.z), glm::vec3(0, 0, 1));

    glm::mat4 MVP = Projection * View * Model;
    glUniformMatrix4fv(MVP_Uniform, 1, GL_FALSE, glm::value_ptr(MVP));
    glUniformMatrix4fv(Model_Uniform, 1, GL_FALSE, glm::value_ptr(Model));

    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(indices[0]), GL_UNSIGNED_INT, nullptr);
}

MouseData gMouse = {0, 0};

void main_game(GLFWwindow* window) {

    //cube

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint buffer = 0;
    glGenBuffers(1, &buffer);

    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*9, (void*)0); // x y z

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float)*9, (void*)(sizeof(float)*3)); // r g b

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float)*9, (void*)(sizeof(float)*6)); // normals

    //indencis

    GLuint indexbuffer = 0;
    glGenBuffers(1, &indexbuffer);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    Shader shader;
    shader.loadShaderProgramFromData(vertexShaderSource, fragmentShaderSource);
    shader.bind();

    GLint move = shader.getUniformLocation("MVP");
    GLint Model = shader.getUniformLocation("Model");

    float posX = 0.0f, posY = 0.0f, posZ = 0.0f;
    float rotX = 0.0f, rotY = 0.0f;
    float moveSpeed = 0.01f;
    float sensitivity = 0.1f;
    float fov = 90.0f;

    bool mouse_enabled = true;
    bool tab_pressed = false;

    double prevX = 0.0f, prevY = 0.0f;

    double previousTime = glfwGetTime();
    double start_time = previousTime;
    float fps = 0;
    float frame_count = 0;

    while (!glfwWindowShouldClose(window)) {

        frame_count++;
        double currentTime = glfwGetTime();

        if(currentTime - start_time >= 1) {
            fps = frame_count;
            frame_count = 0;
            start_time = currentTime;
        }
        previousTime = currentTime;

        glClearColor(0.4,0.4,0.4,0.4);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Input | Position

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) { posX += sin(rotX*(M_PI/180)) * moveSpeed; posZ += cos(rotX*(M_PI/180)) * moveSpeed; }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) { posX -= sin(rotX*(M_PI/180)) * moveSpeed; posZ -= cos(rotX*(M_PI/180)) * moveSpeed; }

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) { posX += sin((rotX-90)*(M_PI/180)) * moveSpeed; posZ += cos((rotX-90)*(M_PI/180)) * moveSpeed; }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) { posX += sin((rotX+90)*(M_PI/180)) * moveSpeed; posZ += cos((rotX+90)*(M_PI/180)) * moveSpeed; }

        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) { posY += moveSpeed; }
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) { posY -= moveSpeed; }

        // FOV

        if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) { fov -= 1.0f; }
        if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) { fov += 1.0f; }

        fov = glm::clamp(fov, 1.0f, 179.0f);

        if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) { 
            if(!tab_pressed) {
                mouse_enabled = !mouse_enabled;
                if (!mouse_enabled) {
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                } else {
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                }
            }
            tab_pressed = true;
        } else { tab_pressed = false; }

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            return;
        }

        // Input | Rotation

        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) { rotX += sensitivity*5; }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) { rotX -= sensitivity*5; }

        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) { rotY += sensitivity*5; }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) { rotY -= sensitivity*5; }

        // Rotation clamping
        if (mouse_enabled) {
            double delX = gMouse.x - prevX; double delY = gMouse.y - prevY;
            rotX -= (delX * sensitivity);
            rotY -= delY * sensitivity;
            prevX = gMouse.x; prevY = gMouse.y;
        }
            
        if (rotX >= 360) { rotX-=360; }
        if (rotX <= 0) { rotX=360+rotX; }
        rotY = glm::clamp(rotY, -89.0f, 89.0f);

        draw_object(
            move,                                           // uniform location
            Model,
            glm::vec3(posX, posY, posZ),                    // camera location
            glm::vec2(rotX*(M_PI/180),rotY*(M_PI/180)),     // camera rotation
            fov,                                            // fov
            glm::vec3(0,0,0),                               // model_pos
            glm::vec3(0, 0, 0),                             // model_rot
            vao
        );

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}
