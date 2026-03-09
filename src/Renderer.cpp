#include "Renderer.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#include <sstream>


std::string loadFile(const std::string& path)
{
    std::ifstream f(path);

        if (!f.is_open()) {
        std::cout << "Failed to open: " << path << std::endl;
        return "";
    }

    std::stringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

Renderer::Renderer()
{
    shaderProgram = 0;
    mvpLoc = -1;
    colorLoc = -1;

}

unsigned int Renderer::createShader(const char* vs, const char* fs)
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

    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        std::cout << "Fragment Shader Error:\n" << infoLog << std::endl;
    }

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
    #ifdef __EMSCRIPTEN__
    auto vert = loadFile("/shaders/basic_webgl.vert");
    auto frag = loadFile("/shaders/basic_webgl.frag");
    #else
    auto vert = loadFile("./shaders/basic_gl.vert");
    auto frag = loadFile("./shaders/basic_gl.frag");
    #endif    
    
    std::cout << "vert size = " << vert.size() << std::endl;
    std::cout << "frag size = " << frag.size() << std::endl;

    shaderProgram = createShader(
        vert.c_str(),
        frag.c_str()
    );


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