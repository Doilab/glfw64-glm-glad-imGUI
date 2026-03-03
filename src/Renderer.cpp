#include "Renderer.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#ifndef __EMSCRIPTEN__
static const char* vertexShaderSource = R"(#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 MVP;

void main()
{
    gl_Position = MVP * vec4(aPos, 1.0);
})";
static const char* fragmentShaderSource = R"(#version 330 core
uniform vec3 color;

out vec4 FragColor;

void main()
{
    FragColor = vec4(color, 1.0);
})";
#else
static const char* vertexShaderSource = R"(#version 300 es
precision mediump float;

layout (location = 0) in vec3 aPos;

uniform mat4 MVP;

void main()
{
    gl_Position = MVP * vec4(aPos, 1.0);
})";
static const char* fragmentShaderSource = R"(#version 300 es
precision mediump float;

uniform vec3 color;

out vec4 FragColor;

void main()
{
    FragColor = vec4(color, 1.0);
})";
#endif


unsigned int Renderer::createShader(
    const char* vs,
    const char* fs)
{

    unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vs, NULL);
    glCompileShader(vertex);

    int success;
    char infoLog[512];

    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cout << "Vertex Shader Error:\n" << infoLog << std::endl;
    }

    unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fs, NULL);
    glCompileShader(fragment);

    unsigned int program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return program;
}

bool Renderer::init(int width, int height)
{
    shaderProgram = createShader(
        vertexShaderSource,
        fragmentShaderSource);

    mvpLoc = glGetUniformLocation(shaderProgram, "MVP");
    colorLoc = glGetUniformLocation(shaderProgram, "color");

    glEnable(GL_DEPTH_TEST);

    projection = glm::perspective(
        glm::radians(45.0f),
        (float)width / height,
        0.1f,
        100.0f);

    view = glm::lookAt(
        glm::vec3(0,0,3),
        glm::vec3(0,0,0),
        glm::vec3(0,1,0));

    return true;}

//----------------------------
Renderer::Mesh
Renderer::createMesh(
    const std::vector<float>& vertices)
{
    Mesh mesh;

    glGenVertexArrays(1, &mesh.VAO);
    glGenBuffers(1, &mesh.VBO);

    glBindVertexArray(mesh.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);

    glBufferData(GL_ARRAY_BUFFER,
                 vertices.size() * sizeof(float),
                 vertices.data(),
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT,
                          GL_FALSE,
                          3*sizeof(float),
                          (void*)0);

    glEnableVertexAttribArray(0);

    mesh.vertexCount = vertices.size() / 3;

    return mesh;
}

Renderer::Mesh
Renderer::createCube(float size)
{
    float s = size * 0.5f;

    std::vector<float> v = {
        // front
        -s,-s, s,  s,-s, s,  s, s, s,
         s, s, s, -s, s, s, -s,-s, s,
        // back
        -s,-s,-s, -s, s,-s,  s, s,-s,
         s, s,-s,  s,-s,-s, -s,-s,-s,
        // left
        -s, s, s, -s, s,-s, -s,-s,-s,
        -s,-s,-s, -s,-s, s, -s, s, s,
        // right
         s, s, s,  s,-s,-s,  s, s,-s,
         s,-s,-s,  s, s, s,  s,-s, s,
        // top
        -s, s,-s, -s, s, s,  s, s, s,
         s, s, s,  s, s,-s, -s, s,-s,
        // bottom
        -s,-s,-s,  s,-s,-s,  s,-s, s,
         s,-s, s, -s,-s, s, -s,-s,-s
    };

    return createMesh(v);
}

Renderer::Mesh
Renderer::createBox(float sx,
                          float sy,
                          float sz)
{
    float x = sx * 0.5f;
    float y = sy * 0.5f;
    float z = sz * 0.5f;

    std::vector<float> v = {
        -x,-y, z,  x,-y, z,  x, y, z,
         x, y, z, -x, y, z, -x,-y, z,
        -x,-y,-z, -x, y,-z,  x, y,-z,
         x, y,-z,  x,-y,-z, -x,-y,-z,
        -x, y, z, -x, y,-z, -x,-y,-z,
        -x,-y,-z, -x,-y, z, -x, y, z,
         x, y, z,  x,-y,-z,  x, y,-z,
         x,-y,-z,  x, y, z,  x,-y, z,
        -x, y,-z, -x, y, z,  x, y, z,
         x, y, z,  x, y,-z, -x, y,-z,
        -x,-y,-z,  x,-y,-z,  x,-y, z,
         x,-y, z, -x,-y, z, -x,-y,-z
    };

    return createMesh(v);
}

Renderer::Mesh
Renderer::createCylinder(float r,
                               float h,
                               int seg)
{
    std::vector<float> v;

    float half = h * 0.5f;

    for (int i = 0; i < seg; ++i)
    {
        float t0 = 2.0f * 3.1415926f * i / seg;
        float t1 = 2.0f * 3.1415926f * (i+1) / seg;

        float x0 = r * cos(t0);
        float z0 = r * sin(t0);
        float x1 = r * cos(t1);
        float z1 = r * sin(t1);

        // side
        v.insert(v.end(), {
            x0,-half,z0,
            x1,-half,z1,
            x1, half,z1,

            x1, half,z1,
            x0, half,z0,
            x0,-half,z0
        });

        // top
        v.insert(v.end(), {
            0,half,0,
            x1,half,z1,
            x0,half,z0
        });

        // bottom
        v.insert(v.end(), {
            0,-half,0,
            x0,-half,z0,
            x1,-half,z1
        });
    }

    return createMesh(v);
}



void Renderer::draw(
    const Mesh& mesh,
    const glm::mat4& model,
    const glm::vec3& color)
{
    glUseProgram(shaderProgram);

    // MVP計算
    glm::mat4 MVP = projection * view * model;

    // 行列送信
    glUniformMatrix4fv(
        mvpLoc,
        1,
        GL_FALSE,
        glm::value_ptr(MVP));

    // 色送信
    glUniform3fv(
        colorLoc,
        1,
        &color[0]);

    // VAOバインド
    glBindVertexArray(mesh.VAO);

    // 描画
    glDrawArrays(
        GL_TRIANGLES,
        0,
        mesh.vertexCount);
}


void Renderer::setProjection(const glm::mat4& proj)
{
    projection = proj;
}
void Renderer::setViewMatrix(const glm::mat4& v)
{
    view = v;
}