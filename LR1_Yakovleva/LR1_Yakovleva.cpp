#define GLFW_DLL
#define GLEW_DLL
#include "glew-2.1.0/include/GL/glew.h"
#include "glfw-3.4.bin.WIN64/include/GLFW/glfw3.h"
#include "shader.h"
#include "Model.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <iostream>

// Настройки положения камеры
glm::vec3 cameraPos = glm::vec3(-1.0f, 2.0f, 10.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

// Мышь
bool firstMouse = true;
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = 512.0f;
float lastY = 384.0f;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

void processInput(GLFWwindow* window) {
    float cameraSpeed = 2.5f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

int main() {
    if (!glfwInit()) return -1;

    GLFWwindow* window = glfwCreateWindow(1024, 768, "LR 5 Assimp", NULL, NULL);
    if (!window) { glfwTerminate(); return -1; }

    glfwMakeContextCurrent(window);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // захват мыши

    glewInit();
    glEnable(GL_DEPTH_TEST);

    Shader ourShader("vertex.txt", "fragment.txt");
    Model ourModel("Yakovleva.obj");

    std::cout << "Model loaded successfully" << std::endl;
    std::cout << "Number of meshes in model: " << ourModel.meshes.size() << std::endl;
    if (ourModel.meshes.empty())
        std::cout << "The model has no meshes" << std::endl;

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
        glfwPollEvents();

        glClearColor(0.4f, 0.5f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ourShader.Use();

        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1024.0f / 768.0f, 0.1f, 1000.0f);
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(1.5f));

        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);
        ourShader.setMat4("model", model);

        ourShader.SetUniform("lightColor", 1.0f, 1.0f, 0.2f);
        ourModel.Draw();

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}