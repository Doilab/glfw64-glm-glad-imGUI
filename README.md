# OpenGL GLFW + GLAD + GLM + Dear ImGui Template
A minimal OpenGL starter project using:
# モダンOpenGLのテスト
2026.3.1 -


This project provides a simple structure for experimenting with OpenGL rendering.

---

## Features

- Modern OpenGL (VAO / VBO / EBO)
- GLM matrix transformations
- Dear ImGui debug UI
- Simple Model / Renderer structure
- Edge rendering for mesh visualization

---

## Screenshot

![screenshot](docs/screenshot.png)

---
## EmscriptenによるWebGL化

https://doilab.github.io/glfw64-glm-glad-imGUI/


## Emscriptenをローカルで試す場合

別ターミナルで

python3 -m http.server 8080

ブラウザで

localhost:8080

にアクセス

---

## Project Structure

src/
App.cpp
Camera.cpp
GUI.cpp
Model.cpp
ModelBuilder.cpp
Renderer.cpp
Robot.h
SceneObject.cpp

third_party/
imgui/
glad/
glm/
GLFW/



---

## Build

### Web(em++)


make linux-web

### Linux (g++)


make linux

### Windows (MinGW g++)


make win64


---

## Dependencies

- GLFW (window / input)
- GLAD (OpenGL loader)
- GLM (math library)
- Dear ImGui (debug UI)

---



## Future improvements

Camera system

STL loader

JSON support

WebSocket

License

MIT License


---
