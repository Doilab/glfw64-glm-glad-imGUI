#pragma once

#include "GLHeaders.h"
#include "Robot.h"
#include <glm/glm.hpp>

class Gui
{
public:

    void setup(GLFWwindow* window);
    void begin(GLFWwindow* window);
    void draw(RobotState* rs);
    void end();
    void shutdown();
    bool WantCaptureMouse();

    // GUI用パラメータ
    float imgui_x = 0.0f;
    float imgui_y = 0.0f;

    float joy[2]={0,0};
    glm::vec3 joint[4];

    double now_second = 0.0;
};