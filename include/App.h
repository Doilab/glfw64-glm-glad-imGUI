#pragma once
#include "Renderer.h"
#include "Camera.h"
#include <GLFW/glfw3.h>

class App
{
public:
    App();
    ~App();

    int run();

private:
    Renderer renderer;
    Renderer::Mesh cube;
    Renderer::Mesh cyl;
    Renderer::Mesh box;

    Camera camera;

	GLFWwindow* window = nullptr;

    // --- メイン処理 ---  
    bool init();
    void mainLoop();
    void shutdown();

    // --- コールバック関連 ---
    static void mouseCallback(GLFWwindow* window, double xpos, double ypos);
    void handleMouse(double xpos, double ypos);// マウス入力処理
    void processInput(float deltaTime);// キーボード入力処理

    //時間管理
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;


};