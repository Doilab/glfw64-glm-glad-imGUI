#pragma once

#include "GLHeaders.h"
//#include <GLFW/glfw3.h>
#include "Robot.h"

class Gui
{
public:

    void setup(GLFWwindow* window);
    void begin(GLFWwindow* window);
    void draw();
    void end();
    void shutdown();

    // GUI用パラメータ
    float imgui_x = 0.0f;
    float imgui_y = 0.0f;

    double now_second = 0.0;
};