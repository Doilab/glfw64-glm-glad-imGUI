#include "GUI.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <string>

#ifdef __EMSCRIPTEN__
#include "BrowserWebSocket.h"
#else
//#include "NativeWebSocket.h"
#include "IXWebSocket.h"
#endif

std::unique_ptr<IWebSocket> ws;


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

    #ifdef __EMSCRIPTEN__
    const char* fontPath = "assets/ipag.ttf";
    #else
    const char* fontPath = "assets/ipag.ttf";
    #endif

    io.Fonts->AddFontFromFileTTF(
        fontPath,
        16.0f,
        NULL,
        io.Fonts->GetGlyphRangesJapanese()
    );

    #ifdef __EMSCRIPTEN__
    ws = std::make_unique<BrowserWebSocket>();
    #else
    ws = std::make_unique<NativeWebSocket>();
    #endif


}
//---------------

//----------------------
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



}

void Gui::draw(RobotState* rs)
{
    ImGui::SetNextWindowPos(ImVec2(0,0),ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(200,100),ImGuiCond_FirstUseEver);

    // ===== WebSocket受信処理 =====
    if((ws)&&(ws->isConnected() ))
    {
        ws->poll(); // （Emscripten用、ネイティブは何もしない）

        while(ws->hasMessage())
        {
            std::string msg = ws->popMessage();

            try
            {
                nlohmann::json j =nlohmann::json::parse(msg);
                RobotState newState = RobotState::from_json(j);

                // サイズチェック
                if(rs->joint.size() == newState.joint.size())
                {
                    for(size_t i = 0; i < rs->joint.size(); i++)
                    {
                        rs->joint[i] = newState.joint[i];
                        ;
                    }
                }
                else
                {
                    std::cout << "Size mismatch\n";
                }

                //std::cout << "received Gui::draw()\n";
            }
            catch(...)
            {
                std::cout << "JSON parse error\n";
            }
        }
    }
    ImGui::Begin(u8"W1 update260417");

        ImGui::Text("経過時間 %.2f", now_second);

        if(ImGui::Button("<")) joy[0]-=0.1;
        ImGui::SameLine();

        if(ImGui::Button("Up")) joy[1]+=0.1;
        ImGui::SameLine();

        if(ImGui::Button("Dn")) joy[1]-=0.1;
        ImGui::SameLine();

        if(ImGui::Button(">")) joy[0]+=0.1;

        ImGui::SliderFloat2("JS", joy, -1.0f, 1.0f);

        if(ImGui::Button("Dump JSON"))
        {
            dumpText = rs->to_json().dump(2);
        }
        ImGui::InputTextMultiline(
            "##json",
            dumpText.data(),
            dumpText.size()+1,
            ImVec2(400,300),
            ImGuiInputTextFlags_ReadOnly
        );


    ImGui::End();

    ImGui::SetNextWindowPos(ImVec2(0,110),ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(200,350),ImGuiCond_FirstUseEver);
    ImGui::Begin(u8"W2 Joints[rad]");
         for(int i=0;i<rs->joint.size();i++)
         {
            std::string label = "J" + std::to_string(i+1);
            ImGui::SliderFloat(label.c_str(), &(rs->joint[i]), -3.14f, 3.14f);  
         }
        ImGui::DragFloat("x",&imgui_x);
        ImGui::DragFloat("y",&imgui_y);
    ImGui::End();

    ImGui::SetNextWindowPos(ImVec2(210,0),ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300,120),ImGuiCond_FirstUseEver);
    ImGui::Begin("WebSocket");

        static char uri_buf[256];
        strncpy(uri_buf, uri.c_str(), sizeof(uri_buf));

        ImGui::InputText("URI", uri_buf, sizeof(uri_buf));
        uri = uri_buf;
        if(!ws->isConnected())
        {
            if(ImGui::Button("Connect"))
            {
                ws->connect(uri);
            }
        }
        else
        {
            ImGui::TextColored(ImVec4(0,1,0,1),"Connected");

            if(ImGui::Button("Disconnect"))
            {
                ws->disconnect();
            }
        }
        if(ws->isConnected())
        {
            if(ImGui::Button("Send RobotState JSON"))
            {
                std::string json = rs->to_json().dump();
                ws->send(json);
            }
        }
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

bool Gui::WantCaptureMouse()
{
    bool flag;
    flag = ImGui::GetIO().WantCaptureMouse;
    return flag;
}