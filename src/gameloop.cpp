#include <thread>
#include <chrono>

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

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

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

    const float quadVertices[] = {
        -1.0f,  1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,

        -1.0f,  1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        1.0f,  1.0f, 0.0f
    };

    GLuint quadVAO = 0, quadVBO = 0;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);

    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindVertexArray(0);

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
    GLint iTimeLoc = shader.getUniformLocation("iTime");
    GLint iResLoc  = shader.getUniformLocation("iResolution");

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    ImGui::StyleColorsDark();

    float posX = 0.0f, posY = 0.0f, posZ = 0.0f;
    float rotX = 0.0f, rotY = 0.0f;
    float moveSpeed = 2.0f;
    float sensitivity = 20.0f;
    float fov = 90.0f;

    bool mouse_enabled = true;
    bool tab_pressed = false;

    double prevX = 0.0f, prevY = 0.0f;

    float targetFPS = 240.0;
    double targetFrameTime = 1.0 / targetFPS;
    double lastTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        double targetFrameTime = 1.0 / targetFPS;

        double currentTime = glfwGetTime();
        float dt = static_cast<float>(currentTime - lastTime);
        lastTime = currentTime;

        glClearColor(0.4,0.4,0.4,0.4);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Input | Position

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) { posX += sin(rotX*(M_PI/180)) * moveSpeed * dt; posZ += cos(rotX*(M_PI/180)) * moveSpeed * dt; }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) { posX -= sin(rotX*(M_PI/180)) * moveSpeed * dt; posZ -= cos(rotX*(M_PI/180)) * moveSpeed * dt; }

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) { posX += sin((rotX-90)*(M_PI/180)) * moveSpeed * dt; posZ += cos((rotX-90)*(M_PI/180)) * moveSpeed * dt; }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) { posX += sin((rotX+90)*(M_PI/180)) * moveSpeed * dt; posZ += cos((rotX+90)*(M_PI/180)) * moveSpeed * dt; }

        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) { posY += moveSpeed * dt; }
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) { posY -= moveSpeed * dt; }

        // FOV

        if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) { fov -= 40.0f * dt; }
        if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) { fov += 40.0f * dt; }

        fov = glm::clamp(fov, 1.0f, 179.0f);

        if (glfwGetKey(window, GLFW_KEY_GRAVE_ACCENT) == GLFW_PRESS) { 
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

        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)  { rotX += sensitivity * 5 * dt; }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) { rotX -= sensitivity * 5 * dt; }

        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)   { rotY += sensitivity * 5 * dt; }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) { rotY -= sensitivity * 5 * dt; }

        // Rotation clamping
        if (mouse_enabled) {
            double delX = gMouse.x - prevX;
            double delY = gMouse.y - prevY;

            rotX -= delX * sensitivity * dt;
            rotY -= delY * sensitivity * dt;

            prevX = gMouse.x; prevY = gMouse.y;
        }
            
        if (rotX >= 360) { rotX -= 360; }
        if (rotX <= 0)   { rotX  = 360 + rotX; }
        rotY = glm::clamp(rotY, -89.9f, 89.9f);

        int fbWidth = 1200, fbHeight = 800;
        glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
        float aspect = (fbHeight > 0)
            ? static_cast<float>(fbWidth) / static_cast<float>(fbHeight)
            : (1200.0f / 800.0f);

        glm::mat4 Projection = glm::perspective(glm::radians(fov), aspect, 0.1f, 100.0f);

        float yaw = glm::radians(rotX);
        float pitch = glm::radians(rotY);
        glm::vec3 forward{
            cos(pitch) * sin(yaw),
            sin(pitch),
            cos(pitch) * cos(yaw)
        };
        glm::vec3 camPos{posX, posY, posZ};
        glm::mat4 View = glm::lookAt(camPos, camPos + forward, glm::vec3(0.0f, 1.0f, 0.0f));

        glm::mat4 modelMat = glm::mat4(1.0f);
        glm::mat4 mvpMat = Projection * View * modelMat;

        glUniform1f(iTimeLoc, (float)currentTime);
        glUniform3f(iResLoc, 1200.0f, 800.0f, 1.0f);

        shader.bind();
        glUniformMatrix4fv(move, 1, GL_FALSE, glm::value_ptr(mvpMat));
        glUniformMatrix4fv(Model, 1, GL_FALSE, glm::value_ptr(modelMat));

        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("meowlicious debug panel");
        ImGui::Text("FPS: %.2f", ImGui::GetIO().Framerate);
        ImGui::Text("FOV: %.2f", fov);
        ImGui::Text("Camera position: %.2f, %.2f, %.2f", posX, posY, posZ);
        ImGui::Text("Camera rotation: %.2f, %.2f", rotX, rotY);
        ImGui::SliderFloat("Target Framerate", &targetFPS, 1.0f, 860.0f, "%.0f");
        ImGui::SliderFloat("Move speed",  &moveSpeed,   0.0f, 100.0f);
        ImGui::SliderFloat("Sensitivity", &sensitivity, 0.0f, 100.0f);
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();

        double frameEndTime = lastTime + targetFrameTime;
        double timeRemaining = frameEndTime - glfwGetTime();

        if (timeRemaining > 0.002) {
            std::this_thread::sleep_for(std::chrono::duration<double>(timeRemaining - 0.002));
        }
        while (glfwGetTime() < frameEndTime) {}
    }
}
