#include "App.h"
#include <GLFW/glfw3.h>
#include "GLHeaders.h"


#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "imGUI/imgui.h"
#include "imGUI/imgui_impl_glfw.h"
#include "imGUI/imgui_impl_opengl3.h"

#include "Renderer.h"
#include "ModelBuilder.h"
#include "SceneObject.h"

#ifdef __EMSCRIPTEN__
    #include <emscripten.h>
#endif

// ImGUI用の変数初期化
float imgui_x = 0.0f, imgui_y = 0.0f;
float now_second = 0;

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
void imgui_setup(GLFWwindow* window)
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);

    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2((float)w, (float)h);
    io.DisplayFramebufferScale = ImVec2(1.0f,1.0f);

    #ifndef __EMSCRIPTEN__
        //io.Fonts->AddFontFromFileTTF("ipag.ttf",32.0f,NULL,io.Fonts->GetGlyphRangesDefault());
        io.Fonts->AddFontFromFileTTF("ipag.ttf",32.0f,NULL,io.Fonts->GetGlyphRangesJapanese());
        //io.FontGlobalScale = 5;//スケール大きく
    #else
        //日本語フォント↓260308スマホで成功
        io.Fonts->AddFontFromFileTTF("/ipag.ttf",32.0f,NULL,io.Fonts->GetGlyphRangesJapanese());//em++用
        // Webはフォントロード失敗しやすいので
        // デフォルトフォントを使う場合↓
        //io.Fonts->AddFontDefault();
    #endif

    // Setup Dear ImGui style
    //ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer bindings
    //ImGui_ImplGlfwGL3_Init(window, true);
    #ifdef __EMSCRIPTEN__
        ImGui_ImplOpenGL3_Init("#version 300 es");//for emcc web用
    #else
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        const char* glsl_version = "#version 130";
        ImGui_ImplOpenGL3_Init(glsl_version);//PC用
    #endif
}
//----------------------
void imgui_begin(GLFWwindow* window)
{
    int w,h;
    glfwGetFramebufferSize(window,&w,&h);

    ImGuiIO& io = ImGui::GetIO();

    io.DisplaySize = ImVec2((float)w,(float)h);
    io.DeltaTime = 1.0f/60.0f;

    double mx,my;
    glfwGetCursorPos(window,&mx,&my);

    io.MousePos = ImVec2((float)mx,(float)my);

    io.MouseDown[0] =
        glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_LEFT)==GLFW_PRESS;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();
}

//----------------------
void imgui_draw()
{
    // ImGUI描画内容セット
    ImGui::Begin(u8"update 260309");
    //ImGui::Text("日本語. %d", 123);
    ImGui::Text("経過時間. %.1f", now_second);
    ImGui::DragFloat("x", &imgui_x);
    ImGui::DragFloat("y", &imgui_y);
    ImGui::End();
 
}
//----------------------
void imgui_end()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
//----------------------
void App::modelling()
{
    //モデリング処理．事前に生成しておく．
    axis3 = ModelBuilder::create3Axis(2);
    xAxis = ModelBuilder::createXAxis(2);
    yAxis = ModelBuilder::createYAxis(2);
    zAxis = ModelBuilder::createZAxis(2);
    cube = ModelBuilder::createCube(1);
    cyl = ModelBuilder::createCylinder(0.1f,0.5f,8);

    root = std::make_shared<SceneObject>();

    auto xNode = std::make_shared<SceneObject>();
    xNode->model = &xAxis;
    xNode->color = {1,0,0};
    xNode->mode  = GL_LINES;

    root->addChild(xNode);

    auto yNode = std::make_shared<SceneObject>();
    yNode->model = &yAxis;
    yNode->color = {0,1,0};
    yNode->mode  = GL_LINES;

    root->addChild(yNode);

    auto zNode = std::make_shared<SceneObject>();
    zNode->model = &zAxis;
    zNode->color = {0,0,1};
    zNode->mode  = GL_LINES;

    root->addChild(zNode);

    bodyNode = std::make_shared<SceneObject>();
    bodyNode->model = &cube;
    bodyNode->color = {0.0f, 0.5f, 0.0f};
    bodyNode->mode  = GL_LINES;

    root->addChild(bodyNode);


    link1Node = std::make_shared<SceneObject>();
    link1Node->model = &cyl;
    link1Node->color = {0.0f, 0.5f, 0.0f};
    link1Node->mode  = GL_TRIANGLES;
    
    float theta = glm::radians(45.0f);
    auto move_vec = glm::vec3(1,0,0);
    link1Node->transform = glm::rotate(glm::mat4(1), theta, {1,0,0});
    link1Node->transform = glm::translate(glm::mat4(1), move_vec);
    root->addChild(link1Node);




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
    imgui_setup(window);
    
    return true;
}

//------------------------------------
void App::mainLoop()
{
    glfwPollEvents();

    //ImGUI初期化と入力読み取り
    imgui_begin(window);
    //ImGUIの描画準備
    imgui_draw();
 

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
    now_second = (time/1000);
    float currentFrame = time;
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    //アニメーションのためのモデル行列
    glm::mat4 model1 = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f,0.0f,0.0f));
    model1 = glm::rotate(model1, time, glm::vec3(1,1,0));
    glm::mat4 model2 = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f,0.0f,0.0f));
    model2 = glm::rotate(model2, time, glm::vec3(1,1,0));
    glm::mat4 model3 = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f,0.0f,0.0f));
    model3 = glm::rotate(model3, time, glm::vec3(1,1,0));

    // 毎フレームカメラ更新
    camera.processKeyboard(window, deltaTime);
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);
    bool pressed =
        glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    camera.processMouse(mouseX, mouseY, pressed);
    renderer.setViewMatrix(camera.getViewMatrix());
    //glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0,0,-5));
    //renderer.setViewMatrix(view);


    // 描画
    // 面
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0, 1.0);

    renderer.draw(cube, model1, glm::vec3(0.8,0.0,0.0), GL_TRIANGLES);
    //renderer.draw(cyl, model2, glm::vec3(0.0,0.8,0.0), GL_TRIANGLES);

    glDisable(GL_POLYGON_OFFSET_FILL);

    // エッジ
    //glDisable(GL_DEPTH_TEST);
    glLineWidth(2.0f);
    renderer.drawEdges(cube,model1,glm::vec3(0.0,0.0,0.0));
    //renderer.drawEdges(cyl,model2,glm::vec3(0.0,0.0,0.0));
    //glEnable(GL_DEPTH_TEST);


    // Axis
    glLineWidth(3.0f);
//    renderer.draw(axis, model1, glm::vec3(0,1,0), GL_LINES);
    renderer.draw(xAxis, model1, glm::vec3(1,0,0), GL_LINES);
    renderer.draw(yAxis, model1, glm::vec3(0,1,0), GL_LINES);
    renderer.draw(zAxis, model1, glm::vec3(0,0,1), GL_LINES);
    


    //SceneObject描画
    root->draw(renderer, model2);

    // ImGUI描画（必ず最後）
    glDisable(GL_DEPTH_TEST);
        imgui_end();
    glEnable(GL_DEPTH_TEST);
    
    glfwSwapBuffers(window);
}

void App::shutdown()
{
    cube.cleanup();

    ImGui_ImplOpenGL3_Shutdown();
    #ifdef __EMSCRIPTEN__
    ImGui_ImplGlfw_Shutdown();
    #endif
    ImGui::DestroyContext();

    if (window)
    {
        glfwDestroyWindow(window);
        window = nullptr;
    }
	glfwTerminate();
}
