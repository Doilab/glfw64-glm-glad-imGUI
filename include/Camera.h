#pragma once

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

// ← →   方位角回転
// ↑ ↓   仰角
// PageUp   ズームイン
// PageDown ズームアウト

// Shift + ← →   パン
// Shift + ↑ ↓   パン

class Camera
{
public:
    Camera();

    void processKeyboard(GLFWwindow* window, float deltaTime);
    void processMouse(double xpos, double ypos, bool mousePressed);

    glm::mat4 getViewMatrix() const;

    glm::vec3 getPosition() const { return position; }

private:
    void updateVectors();

    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;

    //float yaw;
    //float pitch;

    float azimuth   = 0.0f;   // 方位角
    float elevation = 20.0f;  // 仰角
    float distance  = 5.0f;

    glm::vec3 target = {0,0,0};

    float lastX;
    float lastY;
    bool firstMouse;

    float movementSpeed;
    float mouseSensitivity;
};