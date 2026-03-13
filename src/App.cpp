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
#include "Robot.h"

#ifdef __EMSCRIPTEN__
    #include <emscripten.h>
#endif



//GUIクラス（ImGUI）
Gui gui;


//--------------------------
App::App():robot1(12)//初期値設定robot1(12)12自由度
{
}

//--------------------------
App::~App()
{
}

//---------------------------------------------------
void loopProxy(void* arg)//for Emscripten
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
    xAxis = ModelBuilder::createXAxis(20.0);
    yAxis = ModelBuilder::createYAxis(20.0);
    zAxis = ModelBuilder::createZAxis(20.0);
    cube = ModelBuilder::createCube(5.0);
    cyl = ModelBuilder::createCylinder(0.5f,0.5f,8);
    ground = ModelBuilder::createBox(200.0f,200.0f,1.0f);

    //ジョイント（骨）設定
    float base_x=50;//脚付け根 x
    float base_y=30;//脚付け根 y
    float link1_len=50;
    float link2_len=50;
    float height=50;//初期胴体高さ
    link1 = ModelBuilder::createBox(link1_len,5.0f,10.0f);
    link2 = ModelBuilder::createBox(link2_len,5.0f,10.0f);
    body = ModelBuilder::createBox(base_x*2,base_y*2,10.0f);

    root = std::make_shared<SceneObject>();


    auto  BodyJntNode = makeJoint(root, {0.0,0.0,height}, 0, {0,0,1});

    auto j10 = makeJoint(BodyJntNode,{base_x,base_y,0},90,{0,0,1});//付け根ダミージョイント
    auto j11 = makeJoint(j10,{0.0,0.0,0},-30,{0,0,1});
    robot1.addJoint(j11,{0,0,1});

    auto j12 = makeJoint(j11,{0,0,0},-45,{0,1,0});
    robot1.addJoint(j12,{0,1,0});

    auto j13 = makeJoint(j12,{link1_len,0,0},120,{0,1,0});
    robot1.addJoint(j13,{0,1,0});

    auto j20 = makeJoint(BodyJntNode,{-base_x,base_y,0},90,{0,0,1});//付け根ダミージョイント
    auto j21 = makeJoint(j20,{0.0,0.0,0},10,{0,0,1});
    robot1.addJoint(j21,{0,0,1});

    auto j22 = makeJoint(j21,{0,0,0},-45,{0,1,0});
    robot1.addJoint(j22,{0,1,0});

    auto j23 = makeJoint(j22,{link1_len,0,0},120,{0,1,0});
    robot1.addJoint(j23,{0,1,0});


    auto j30 = makeJoint(BodyJntNode,{-base_x,-base_y,0},-90,{0,0,1});//付け根ダミージョイント
    auto j31 = makeJoint(j30,{-0,0,0},0,{0,0,1});
    robot1.addJoint(j31,{0,0,1});

    auto j32 = makeJoint(j31,{0,0,0},0,{0,1,0});
    robot1.addJoint(j32,{0,1,0});

    auto j33 = makeJoint(j32,{link1_len,0,0},90,{0,1,0});
    robot1.addJoint(j33,{0,1,0});

    auto j40 = makeJoint(BodyJntNode,{base_x,-base_y,0},-90,{0,0,1});//付け根ダミージョイント
    auto j41 = makeJoint(j40,{-0,0,0},0,{0,0,1});
    robot1.addJoint(j41,{0,0,1});

    auto j42 = makeJoint(j41,{0,0,0},0,{0,1,0});
    robot1.addJoint(j42,{0,1,0});

    auto j43 = makeJoint(j42,{link1_len,0,0},90,{0,1,0});
    robot1.addJoint(j43,{0,1,0});


    //リンク（肉）の設定
    auto GndNode = makeLink(root, &ground, {0,0,0}, {0,0,0});
    GndNode->mode = GL_LINES;
    auto BodyNode = makeLink(BodyJntNode, &body, {0,0,0}, {0,0.5,0});
    BodyNode->mode  = GL_LINES;

    auto link11 = makeLink(j12, &link1, {link1_len*0.5,0,0}, {0,0.5,0});
    auto link12 = makeLink(j13, &link2, {link2_len*0.5,0,0}, {0,0,0.5});

    auto link21 = makeLink(j22, &link1, {link1_len*0.5,0,0}, {0,0.5,0});
    auto link22 = makeLink(j23, &link2, {link2_len*0.5,0,0}, {0,0,0.5});

    auto link31 = makeLink(j32, &link1, {link1_len*0.5,0,0}, {0,0.5,0});
    auto link32 = makeLink(j33, &link2, {link2_len*0.5,0,0}, {0,0,0.5});

    auto link41 = makeLink(j42, &link1, {link1_len*0.5,0,0}, {0,0.5,0});
    auto link42 = makeLink(j43, &link2, {link2_len*0.5,0,0}, {0,0,0.5});


}
//----------------------
void App::update(float t)
{
    //関節角を変えるアニメーション関数
    float omega = 2.0f;


    robot1.state.joint[0] =
    glm::radians(30.0f)*sin(omega*t);

    robot1.state.joint[1] =
        glm::radians(20.0f)*sin(omega*t);

    robot1.state.joint[2] =
        glm::radians(10.0f)*sin(omega*t);

    robot1.update();
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

    //物体配置のためのモデル行列
    glm::mat4 model1 = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f,-2.0f,1.0f));
    model1 = glm::rotate(model1, (time*0.5f), glm::vec3(1,1,0));//固定軸周りのアニメーション
    model1 = glm::scale(model1,glm::vec3(0.01,0.01,0.01));

    glm::mat4 model2 = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f,0.0f,0.0f));
    //model2 = glm::rotate(model2, time*0.0f, glm::vec3(1,1,0));//固定軸周りのアニメーション
    model2 = glm::scale(model2,glm::vec3(0.01,0.01,0.01));
    

    // 毎フレームカメラ更新
    camera.processKeyboard(window, deltaTime);
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);
    bool pressed =
        glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    camera.processMouse(mouseX, mouseY, pressed);
    renderer.setViewMatrix(camera.getViewMatrix());
   


    // 回転する立方体描画
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
