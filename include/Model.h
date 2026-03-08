#pragma once

#include <vector>
#ifdef __EMSCRIPTEN__
    #include <GLES3/gl3.h>
#else
    #include <glad/glad.h>
#endif

class Model
{
public:
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    std::vector<unsigned int> edges;

    GLuint VAO = 0;
    GLuint VBO = 0;
    GLuint EBO = 0;

    void upload();
    void draw(GLenum mode);

    void cleanup()
    {
        if (VAO) glDeleteVertexArrays(1, &VAO);
        if (VBO) glDeleteBuffers(1, &VBO);
        if (EBO) glDeleteBuffers(1, &EBO);
        //if (edgeEBO) glDeleteBuffers(1, &edgeEBO);
    }
};