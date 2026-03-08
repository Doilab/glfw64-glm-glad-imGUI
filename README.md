# OpenGL GLFW + GLAD + GLM + Dear ImGui Template
A minimal OpenGL starter project using:
# モダンOpenGLのテスト
2026.3.1 -


- GLFW (window / input)
- GLAD (OpenGL loader)
- GLM (math library)
- Dear ImGui (debug UI)

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
Renderer.cpp
Model.cpp
ModelBuilder.cpp

third_party/
imgui/
glad/
glm/
GLFW/



---

## Build

### Web(em++)


make linux-web

### Linux / macOS


make linux

### Windows (MinGW)


make win64


---

## Dependencies

- GLFW
- GLAD
- GLM
- Dear ImGui

---



## Future improvements

OBJ loader

Lighting

Texture support

Camera system

License

MIT License


---
