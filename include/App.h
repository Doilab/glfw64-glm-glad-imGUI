#pragma once
#include "GLHeaders.h"
#include <GLFW/glfw3.h>
#include "Renderer.h"
#include "Camera.h"
#include "Model.h"
#include "SceneObject.h"
#include "Robot.h"

struct Joint
{
    std::shared_ptr<SceneObject> node;

    glm::vec3 pos;   // 関節位置
    glm::vec3 axis;  // 回転軸
    float angle;     // 現在角度
    glm::mat4 baseTransform;    // 初期transform
};

struct Leg
{
    std::vector<Joint> joints;
};

class App
{
public:
    App();
    ~App();

    int run();

private:
    Renderer renderer;
    Model cube, cyl;
    Model link1, link2, body;
    Model axis3, xAxis, yAxis, zAxis;
    Model ground;
    Model arrow;

    Robot robot1;//ロボットクラス

    std::shared_ptr<SceneObject> root;

    std::vector<Leg> legs;

    Camera camera;
	GLFWwindow* window = nullptr;

    // --- メイン処理 ---  
    bool init();
    void mainLoop();
    void update(float t);
    void shutdown();
    friend void loopProxy(void*);//Web用


    // --- リンクとジョイント操作 ---  
    void modelling();
    void AttachAxis(std::shared_ptr<SceneObject> obj);
    std::shared_ptr<SceneObject> makeJoint(
        std::shared_ptr<SceneObject> parent,
        glm::vec3 pos,
        float angle,
        glm::vec3 axis);
    std::shared_ptr<SceneObject> makeLink(
        std::shared_ptr<SceneObject> parent,
        Model* model,
        glm::vec3 pos,
        glm::vec3 color);
    Joint& addJoint(Leg& leg, std::shared_ptr<SceneObject> node);
    std::shared_ptr<SceneObject> makeRobotJoint(
        Leg& leg,
        std::shared_ptr<SceneObject> parent,
        glm::vec3 pos,
        float angle,
        glm::vec3 axis);


    // --- コールバック関連 ---
    static void mouseCallback(GLFWwindow* window, double xpos, double ypos);
    void handleMouse(double xpos, double ypos);// マウス入力処理
    void processInput(float deltaTime);// キーボード入力処理

    //時間管理
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;


};