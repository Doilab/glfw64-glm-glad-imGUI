#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera()
{
    worldUp = glm::vec3(0.0f, 0.0f, 1.0f);

    target = glm::vec3(0.0f, 0.0f, 0.0f);

    azimuth   = 120.0f;  // 斜め後ろ
    elevation = 30.0f;   // 見下ろし
    distance  = 4.0f;

    movementSpeed   = 5.0f;
    mouseSensitivity = 0.1f;

    firstMouse = true;
    lastX = 0.0f;
    lastY = 0.0f;

    updateVectors();
}
//-----------------------------

 void Camera::processKeyboard(GLFWwindow* window, float dt)
{
    float rotSpeed  = 60.0f * dt;
    float zoomSpeed = 5.0f  * dt;
    float panSpeed  = 2.0f  * dt;

    bool shift = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;

    if(!shift)
    {
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
            azimuth -= rotSpeed;

        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
            azimuth += rotSpeed;

        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
            elevation += rotSpeed;

        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
            elevation -= rotSpeed;

        if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)
            distance -= zoomSpeed;

        if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
            distance += zoomSpeed;
    }
    else
    {
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
            target -= right * panSpeed;

        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
            target += right * panSpeed;

        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
            target += up * panSpeed;

        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
            target -= up * panSpeed;
    }

    elevation = glm::clamp(elevation, -89.0f, 89.0f);
    distance  = glm::max(distance, 0.5f);

    updateVectors();
}
//-----------------------------
void Camera::processMouse(double xpos, double ypos, bool pressed)
{
    if(!pressed)
    {
        firstMouse = true;
        return;
    }

    if(firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float dx = xpos - lastX;
    float dy = ypos - lastY;

    lastX = xpos;
    lastY = ypos;

    dx *= mouseSensitivity;
    dy *= mouseSensitivity;

    azimuth   -= dx;
    elevation += dy;

    elevation = glm::clamp(elevation, -89.0f, 89.0f);

    updateVectors();
}
//-----------------------------
void Camera::updateVectors()
{
    float a = glm::radians(azimuth);
    float e = glm::radians(elevation);

    position.x = target.x + distance * cos(e) * cos(a);
    position.y = target.y + distance * cos(e) * sin(a);
    position.z = target.z + distance * sin(e);

    front = glm::normalize(target - position);
    right = glm::normalize(glm::cross(front, worldUp));
    up    = glm::normalize(glm::cross(right, front));
}
//-----------------------------
glm::mat4 Camera::getViewMatrix() const
{
    //return glm::lookAt(position, position + front, up);
    return glm::lookAt(position, target, up);
}