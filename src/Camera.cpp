#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera()
{
    position = glm::vec3(0.0f, 0.0f, 5.0f);
    worldUp  = glm::vec3(0.0f, 1.0f, 0.0f);

    yaw   = -90.0f;
    pitch = 0.0f;

    front = glm::vec3(0.0f, 0.0f, -1.0f);

    movementSpeed   = 5.0f;
    mouseSensitivity = 0.1f;

    firstMouse = true;
    lastX = 0.0f;
    lastY = 0.0f;

    updateVectors();
}
//-----------------------------
void Camera::processKeyboard(GLFWwindow* window, float deltaTime)
{
    float velocity = movementSpeed * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        yaw -= 60.0f * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        yaw += 60.0f * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        pitch += 60.0f * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        pitch -= 60.0f * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)
        position += front * velocity;

    if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
        position -= front * velocity;

    if (pitch > 89.0f)  pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    updateVectors();
}
//-----------------------------
void Camera::processMouse(double xpos, double ypos, bool mousePressed)
{
    if (!mousePressed)
    {
        firstMouse = true;
        return;
    }

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    yaw   += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)  pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    updateVectors();
}
//-----------------------------
void Camera::updateVectors()
{
    glm::vec3 f;
    f.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    f.y = sin(glm::radians(pitch));
    f.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    front = glm::normalize(f);
    right = glm::normalize(glm::cross(front, worldUp));
    up    = glm::normalize(glm::cross(right, front));
}
//-----------------------------
glm::mat4 Camera::getViewMatrix() const
{
    return glm::lookAt(position, position + front, up);
}