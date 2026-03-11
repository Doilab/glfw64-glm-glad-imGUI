CC=g++
CC_MINGW64VSC =..\TOOLS\mingw64\bin\g++

APP_SRC = ./src/App.cpp ./src/Renderer.cpp ./src/main.cpp ./src/Camera.cpp \
	./src/Model.cpp ./src/ModelBuilder.cpp ./src/SceneObject.cpp ./src/GUI.cpp
IMGUI_SRC = imGUI/imgui/imgui.cpp imGUI/imgui/imgui_draw.cpp imGUI/imgui/imgui_widgets.cpp imGUI/imgui/imgui_tables.cpp \
    imGUI/imgui/backends/imgui_impl_glfw.cpp imGUI/imgui/backends/imgui_impl_opengl3.cpp 
IMGUI_SRC_WEB = imGUI/imgui/imgui.cpp imGUI/imgui/imgui_draw.cpp imGUI/imgui/imgui_widgets.cpp imGUI/imgui/imgui_tables.cpp \
     imGUI/imgui/backends/imgui_impl_opengl3.cpp 
WINOPT = -lglu32 -lopengl32 -static -lstdc++ -lgcc

all:win64

win64:
	$(CC_MINGW64VSC) $(APP_SRC)   $(IMGUI_SRC) ./glad/src/glad.c \
	-I. -I./include -I./imGUI  -I./glm -I./glad/include \
	-DWIN32  \
	./GLFW/lib64/libglfw3dll.a  $(WINOPT) \
	-o app.exe

linux:
	$(CC)  $(APP_SRC) $(IMGUI_SRC) ./glad/src/glad.c \
	-I. -I./include -I./imGUI  -I./glm -I./glad/include \
	-I/usr/X11R6/include \
	-std=c++17 \
    -Wall -Wextra \
    -lglfw -lGL -ldl \
	-DIMGUI_IMPL_OPENGL_LOADER_GLAD \
	-o app.exe

linux-web:
	em++ $(APP_SRC) $(IMGUI_SRC_WEB) \
	-I. -I./include -I./imGUI -I./glm \
	-s USE_GLFW=3 \
	-s USE_WEBGL2=1 \
	-s MIN_WEBGL_VERSION=2 \
	-s MAX_WEBGL_VERSION=2 \
	-s ALLOW_MEMORY_GROWTH=1 \
	-s ASSERTIONS=1 \
	-s INITIAL_MEMORY=134217728 \
	-s ALLOW_MEMORY_GROWTH=1 \
	-fexceptions \
	--preload-file ipag.ttf \
	--preload-file shaders \
	-Wno-nontrivial-memcall \
	-O2 \
	-o docs/index.html

clean:
	rm *.o *.exe
