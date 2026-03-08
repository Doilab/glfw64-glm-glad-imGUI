#ifndef RENDERER_H
#define RENDERER_H

#include "GLHeaders.h"

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

    void draw(Model& model,
            const glm::mat4& modelMat,
            const glm::vec3& color,
            GLenum mode);
    void drawEdges(Model& model,
        const glm::mat4& modelMat,
        const glm::vec3& color);

   
};

#endif