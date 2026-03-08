#pragma once

#include <vector>
#include "GLHeaders.h"

class Model
{
public:
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    //std::vector<unsigned int> edges;
    std::vector<unsigned int> edgeIndices;

    GLuint VAO_faces = 0;
    GLuint VAO_edges = 0;
    GLuint VBO = 0;
    GLuint EBO = 0;
    GLuint edgeEBO=0;

    void upload();
    void draw(GLenum mode);
    void drawEdges();
    void buildEdges(Model& m);

    void cleanup()
    {
        if (VAO_faces) glDeleteVertexArrays(1, &VAO_faces);
        if (VAO_edges) glDeleteVertexArrays(1, &VAO_edges);
        if (VBO) glDeleteBuffers(1, &VBO);
        if (EBO) glDeleteBuffers(1, &EBO);
        if (edgeEBO) glDeleteBuffers(1, &edgeEBO);
    }
};