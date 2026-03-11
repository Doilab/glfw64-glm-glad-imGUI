#include "GUI.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

void Gui::setup(GLFWwindow* window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    int w, h;
    glfwGetFramebufferSize(window, &w, &h);

    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2((float)w, (float)h);
    io.DisplayFramebufferScale = ImVec2(1.0f,1.0f);

    #ifndef __EMSCRIPTEN__

    ImGui_ImplGlfw_InitForOpenGL(window, true);

    const char* glsl_version = "#version 130";
    ImGui_ImplOpenGL3_Init(glsl_version);

    #else

        ImGui_ImplOpenGL3_Init("#version 300 es");

    #endif

    #ifndef __EMSCRIPTEN__

        io.Fonts->AddFontFromFileTTF(
            "ipag.ttf",
            32.0f,
            NULL,
            io.Fonts->GetGlyphRangesJapanese()
        );

    #else

        io.Fonts->AddFontFromFileTTF(
            "/ipag.ttf",
            32.0f,
            NULL,
            io.Fonts->GetGlyphRangesJapanese()
        );

    #endif


}

void Gui::begin(GLFWwindow* window)
{
    #ifdef __EMSCRIPTEN__
        ImGui_ImplOpenGL3_NewFrame();
    #else
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
    #endif

    ImGui::NewFrame();

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


    ImGui::SetNextWindowPos(ImVec2(10,10),ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300,100),ImGuiCond_FirstUseEver);
}

void Gui::draw()
{
    ImGui::Begin(u8"update 260311");

    ImGui::Text("経過時間 %.2f", now_second);

    ImGui::DragFloat("x",&imgui_x);
    ImGui::DragFloat("y",&imgui_y);

    ImGui::End();
}

void Gui::end()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
void Gui::shutdown()
{
#ifndef __EMSCRIPTEN__
    ImGui_ImplGlfw_Shutdown();
#endif

    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();
}