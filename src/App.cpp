#include "App.h"
#include <GLFW/glfw3.h>


#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "imGUI/imgui.h"
#include "imGUI/imgui_impl_glfw.h"
#include "imGUI/imgui_impl_opengl3.h"

#include "Renderer.h"
#include "ModelBuilder.h"


#ifdef __EMSCRIPTEN__
    #include <GLES3/gl3.h>
    #include <emscripten.h>
#else
    #include <glad/glad.h>
#endif

// カメラ用
//static float fov = 45.0f;

// ImGUI用の変数初期化
float imgui_x = 0.0f, imgui_y = 0.0f;

App::App()
{
}

App::~App()
{
}


void loopProxy(void* arg)
{
    static_cast<App*>(arg)->mainLoop();
}

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
    ImGui::Begin(u8"Hello, world!日本語3");
    ImGui::Text("This is some useful text. %d", 123);
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
    cube = ModelBuilder::createCube();
    axis = ModelBuilder::createAxis(3);

    //imGUIの初期設定
    imgui_setup(window);
    
    return true;
}

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

    if (height > 0) {
        glm::mat4 p = glm::perspective(
        glm::radians(45.0f),
        (float)width / (float)height,
        0.1f,
        100.0f);
        renderer.setProjection(p);
    }
    
    float time = glfwGetTime();
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
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0, 1.0);

    renderer.draw(cube, model1, glm::vec3(0.8,0.0,0.0), GL_TRIANGLES);

    glDisable(GL_POLYGON_OFFSET_FILL);

    // エッジ
    glLineWidth(2.0f);
    renderer.draw(cube, model1, glm::vec3(0,0,0), GL_LINES);

    // Axis
    glLineWidth(3.0f);
    renderer.draw(axis, model1, glm::vec3(1,0,0), GL_LINES);
    
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);//ワイヤーフレームでデバッグ
    //renderer.draw(cube, model1, glm::vec3(0,0,0));

 

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
