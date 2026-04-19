cmake_minimum_required(VERSION 3.16)
project(App)

set(CMAKE_CXX_STANDARD 17)

# ===== ソース =====
file(GLOB APP_SRC
    src/*.cpp
)

file(GLOB IMGUI_SRC
    imGUI/imgui/*.cpp
    imGUI/imgui/backends/imgui_impl_glfw.cpp
    imGUI/imgui/backends/imgui_impl_opengl3.cpp
)

file(GLOB IXWEB_SRC
    ixwebsocket/*.cpp
)

set(GLAD_SRC glad/src/glad.c)

# ===== 実行ファイル =====
add_executable(app
    ${APP_SRC}
    ${IMGUI_SRC}
    ${IXWEB_SRC}
    ${GLAD_SRC}
)

# ===== include =====
target_include_directories(app PRIVATE
    .
    include
    imGUI
    glm
    glad/include
    ixwebsocket
)

# ===== OS別設定 =====
if(WIN32)

    target_compile_definitions(app PRIVATE WIN32)

    target_link_libraries(app
        ${CMAKE_SOURCE_DIR}/GLFW/lib64/libglfw3dll.a
        glu32 opengl32 ws2_32 z ssl crypto pthread
    )

elseif(UNIX)

    target_compile_definitions(app PRIVATE IMGUI_IMPL_OPENGL_LOADER_GLAD)

    target_include_directories(app PRIVATE
        /usr/X11R6/include
        ixwebsocket/ixwebsocket
    )

    target_link_libraries(app
        glfw GL dl
        ssl crypto z pthread
    )

endif()

# ===== websocket test =====
add_executable(websocket_server
    src/robot_server.cpp
    ${IXWEB_SRC}
)

target_include_directories(websocket_server PRIVATE
    .
    include
    ixwebsocket
)

target_link_libraries(websocket_server
    ssl crypto z pthread
)