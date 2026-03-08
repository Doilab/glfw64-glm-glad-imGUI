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

    return true;
}

//----------------------------
void Renderer::draw(Model& model,
                    const glm::mat4& modelMat,
                    const glm::vec3& color,
                    GLenum mode)
{
    glUseProgram(shaderProgram);

    glm::mat4 mvp = projection * view * modelMat;

    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));

    glUniform3fv(colorLoc, 1, glm::value_ptr(color));

    model.draw(mode);
}
//----------------------------
void Renderer::drawEdges(Model& model,
        const glm::mat4& modelMat,
        const glm::vec3& color)
{
    glUseProgram(shaderProgram);
    glm::mat4 mvp = projection * view * modelMat;

    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));
    glUniform3fv(colorLoc, 1, glm::value_ptr(color));
    model.drawEdges();
}
//----------------------------




void Renderer::setProjection(const glm::mat4& proj)
{
    projection = proj;
}
void Renderer::setViewMatrix(const glm::mat4& v)
{
    view = v;
}