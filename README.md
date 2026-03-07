# モダンOpenGLのテスト
2026.3.1 -

## 依存ライブラリなど

- GLFW　基本
- GLM　数学関数群
- GLAD　ImGUIに必要(Emscriptenには不要．条件コンパイルで分ける)
- imGUI　メニューインターフェース


## EmscriptenによるWebGL化

https://doilab.github.io/glfw64-glm-glad-imGUI/


## Emscriptenをローカルで試す場合

別ターミナルで

python3 -m http.server 8080

ブラウザで

localhost:8080

にアクセス