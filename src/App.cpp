#include "App.h"
#include <GLFW/glfw3.h>
#include "GLHeaders.h"


#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include "Renderer.h"
#include "ModelBuilder.h"
#include "SceneObject.h"
#include "GUI.h"

#ifdef __EMSCRIPTEN__
    #include <emscripten.h>
#endif



//GUIクラス（ImGUI）
Gui gui;

App::App()
{
}

App::~App()
{
}

//---------------------------------------------------
void loopProxy(void* arg)
{
    static_cast<App*>(arg)->mainLoop();
}

//---------------------------------------------------
int App::run()
{
    if (!init())
        return -1;

    #ifdef __EMSCRIPTEN__
        emscripten_set_main_loop_arg(loopProxy, this, 0, true);
    #else
        while (!glfwWindowShouldClose(window)) {
            mainLoop();
        }
        shutdown();
    #endif

    return 0;
}

//---------------------------------------------------

//----------------------
void App::AttachAxis(std::shared_ptr<SceneObject> obj)
{
    auto x = std::make_shared<SceneObject>();
    x->model = &xAxis;
    x->color = {1,0,0};
    x->mode = GL_LINES;
    obj->addChild(x);

    auto y = std::make_shared<SceneObject>();
    y->model = &yAxis;
    y->color = {0,1,0};
    y->mode = GL_LINES;
    obj->addChild(y);

    auto z = std::make_shared<SceneObject>();
    z->model = &zAxis;
    z->color = {0,0,1};
    z->mode = GL_LINES;
    obj->addChild(z);
}
//----------------------
std::shared_ptr<SceneObject> App::makeJoint(
    std::shared_ptr<SceneObject> parent,
    glm::vec3 pos,
    float angle,
    glm::vec3 axis)
{
    auto node = std::make_shared<SceneObject>();

    glm::mat4 M = glm::mat4(1);
    M = glm::translate(M, pos);
    M = glm::rotate(M, glm::radians(angle), axis);

    node->transform = M;

    parent->addChild(node);
    AttachAxis(node);

    return node;
}
//----------------------
std::shared_ptr<SceneObject> App::makeLink(
    std::shared_ptr<SceneObject> parent,
    Model* model,
    glm::vec3 pos,
    glm::vec3 color)
{
    auto node = std::make_shared<SceneObject>();

    node->model = model;
    node->color = color;
    node->mode  = GL_TRIANGLES;

    glm::mat4 M = glm::translate(glm::mat4(1), pos);
    node->transform = M;

    parent->addChild(node);

    return node;
}

Joint& App::addJoint(Leg& leg, std::shared_ptr<SceneObject> node)
{
    Joint j;
    j.node = node;
    leg.joints.push_back(j);
    return leg.joints.back();
}
//-----------------------
std::shared_ptr<SceneObject> App::makeRobotJoint(
    Leg& leg,
    std::shared_ptr<SceneObject> parent,
    glm::vec3 pos,
    float angle,
    glm::vec3 axis)
{
    auto node = makeJoint(parent,pos,angle,axis);

    Joint j;
    j.node = node;
    j.pos  = pos;
    j.axis = axis;
    j.angle = angle;

    j.baseTransform = node->transform;

    leg.joints.push_back(j);

    return node;
}
//----------------------
void App::modelling()
{
    //モデリング処理．事前に生成しておく．
    axis3 = ModelBuilder::create3Axis(2);
    xAxis = ModelBuilder::createXAxis(0.5);
    yAxis = ModelBuilder::createYAxis(0.5);
    zAxis = ModelBuilder::createZAxis(0.5);
    cube = ModelBuilder::createCube(0.5);
    cyl = ModelBuilder::createCylinder(0.05f,0.5f,8);
    link = ModelBuilder::createBox(0.5f,0.05f,0.1f);
    body = ModelBuilder::createBox(1.0f,0.8f,0.1f);
    ground = ModelBuilder::createBox(2.0f,2.0f,0.01f);

    root = std::make_shared<SceneObject>();

    auto  BodyJntNode = makeJoint(root, {0.0,0.0,0.5}, 0, {0,0,1});

    Leg leg1;


    auto Jnt11Node = makeRobotJoint(leg1, BodyJntNode, {0.5,0.4,0}, 30, {0,0,1});

    auto Jnt12Node = makeRobotJoint(leg1, Jnt11Node, {0,0,0}, 45, {0,1,0});

    auto Jnt13Node = makeRobotJoint(leg1, Jnt12Node, {0.5,0,0}, 30, {0,1,0});

    //リンク（肉）の設定
    auto GndNode = makeLink(root, &ground, {0,0,0}, {0,0,0});
    GndNode->mode = GL_LINES;
    auto BodyNode = makeLink(BodyJntNode, &body, {0,0,0}, {0,0.5,0});
    BodyNode->mode  = GL_LINES;
    auto link11Node = makeLink(Jnt12Node, &link, {0.25,0,0}, {0,0.5,0});
    auto link12Node = makeLink(Jnt13Node, &link, {0.25,0,0}, {0,0,0.5});
    
    legs.push_back(leg1);


}
//----------------------
void App::update(float t)
{
    //関節角を変えるアニメーション関数
    float omega = 2.0f;

    for(auto &leg : legs)
    {
        for(size_t i=0;i<leg.joints.size();i++)
        {
            Joint &j = leg.joints[i];

            j.angle = glm::radians(30.0f) * sin(omega * t);

            glm::mat4 M = j.baseTransform;   // ★初期姿勢

            //M = glm::translate(M, j.pos);
            M = glm::rotate(M, j.angle, j.axis);

            j.node->transform = M;
        }
    }
}

//----------------------
bool App::init()
{
    if (!glfwInit())
    {
        std::cerr << "Failed to init GLFW\n";
        return false;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,
                   GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(800, 600, "GLFW - test2", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);

    #ifdef __EMSCRIPTEN__
        // WebではGLAD不要
    #else
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cerr << "Failed to init GLAD\n";
            return false;
        }
    #endif
    
    // ここでレンダリング初期化（GLコンテキスト後！）
    if (!renderer.init(800,600))
        return false;
    glEnable(GL_DEPTH_TEST);
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);


    //ここで形状生成
    modelling();
    
    //imGUIの初期設定
    gui.setup(window);
    
    return true;
}

//------------------------------------
void App::mainLoop()
{
    glfwPollEvents();

    //ImGUI初期化と入力読み取り
    gui.begin(window);
    //ImGUIの描画準備
    gui.draw();
 

    // 画面クリア
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);//背景白
    //glClearColor(0,0,0,1);//背景黒
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    
    // 3D描画
    glEnable(GL_DEPTH_TEST);
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    //視点決定
    if (height > 0) {
        glm::mat4 p = glm::perspective(
        glm::radians(45.0f),
        (float)width / (float)height,
        0.1f,
        100.0f);
        renderer.setProjection(p);
    }
    
    float time = glfwGetTime();
    gui.now_second = (time);
    float currentFrame = time;
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    //アニメーションのためのモデル行列
    glm::mat4 model1 = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f,0.0f,0.0f));
    model1 = glm::rotate(model1, (time*0.5f), glm::vec3(1,1,0));
    model1 = glm::scale(model1,glm::vec3(0.2,0.2,0.2));
    glm::mat4 model2 = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f,0.0f,0.0f));
    model2 = glm::rotate(model2, time*0.5f, glm::vec3(1,1,0));
    

    // 毎フレームカメラ更新
    camera.processKeyboard(window, deltaTime);
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);
    bool pressed =
        glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    camera.processMouse(mouseX, mouseY, pressed);
    renderer.setViewMatrix(camera.getViewMatrix());
   


    // 描画
    // 面
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0, 1.0);

    renderer.draw(cube, model1, glm::vec3(0.8,0.0,0.0), GL_TRIANGLES);

    glDisable(GL_POLYGON_OFFSET_FILL);

    // エッジ
    glLineWidth(2.0f);
    renderer.drawEdges(cube,model1,glm::vec3(0.0,0.0,0.0));


    // Axis
    glLineWidth(3.0f);
    renderer.draw(xAxis, model1, glm::vec3(1,0,0), GL_LINES);
    renderer.draw(yAxis, model1, glm::vec3(0,1,0), GL_LINES);
    renderer.draw(zAxis, model1, glm::vec3(0,0,1), GL_LINES);
    

    update(time);//関節角レベルでのアニメーション

    //SceneObject（階層構造あり）描画
    root->draw(renderer, model2);

    // ImGUI描画（必ず最後）
    glDisable(GL_DEPTH_TEST);
        gui.end();
    glEnable(GL_DEPTH_TEST);
    
    glfwSwapBuffers(window);
}

void App::shutdown()
{
    cube.cleanup();
    xAxis.cleanup();
    yAxis.cleanup();
    zAxis.cleanup();

    gui.shutdown();
	glfwTerminate();
}
