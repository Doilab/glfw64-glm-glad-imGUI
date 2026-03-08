#ifndef RENDERER_H
#define RENDERER_H

//#define __EMSCRIPTEN__ //web用

#ifdef __EMSCRIPTEN__
    #include <GLES3/gl3.h>
#else
    #include <glad/glad.h>
#endif

#include <glm/glm.hpp>
#include <vector>
#include "Model.h"

class Renderer
{
public:

private:
    unsigned int shaderProgram;
    unsigned int mvpLoc;
    unsigned int colorLoc;

    glm::mat4 projection;
    glm::mat4 view;

    unsigned int createShader(const char* vs,
                              const char* fs);

public:
    bool init(int width, int height);
    void setProjection(const glm::mat4& proj);
    void setViewMatrix(const glm::mat4& v);

    //void draw(Model& model);
    void draw(Model& model,
            const glm::mat4& modelMat,
            const glm::vec3& color,
            GLenum mode);

   
};

#endif