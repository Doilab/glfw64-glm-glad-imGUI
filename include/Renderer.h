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

class Renderer
{
public:

    struct Mesh
    {
        GLuint VAO;
        GLuint VBO;
        GLuint EBO;        // 三角形インデックス

        GLuint edgeEBO;    // エッジ用インデックス

        int indexCount;
        int edgeIndexCount;
        void cleanup()
        {
            if (VBO) glDeleteBuffers(1, &VBO);
            if (EBO) glDeleteBuffers(1, &EBO);
            if (edgeEBO) glDeleteBuffers(1, &edgeEBO);
            if (VAO) glDeleteVertexArrays(1, &VAO);
        }
    };

private:
    unsigned int shaderProgram;
    unsigned int mvpLoc;
    unsigned int colorLoc;

    glm::mat4 projection;
    glm::mat4 view;

    unsigned int createShader(const char* vs,
                              const char* fs);

    Mesh createMesh(
    const std::vector<float>& vertices,
    const std::vector<unsigned int>& indices,
    const std::vector<unsigned int>& edgeIndices);

public:
    bool init(int width, int height);
    void setProjection(const glm::mat4& proj);
    void setViewMatrix(const glm::mat4& v);

    void draw(const Mesh& mesh,
              const glm::mat4& model,
              const glm::vec3& color);
    void drawEdges(
    const Mesh& mesh,
    const glm::mat4& model,
    const glm::vec3& color);

    void drawTriangles(
    const Mesh& mesh,
    const glm::mat4& model,
    const glm::vec3& color);

    // 形状生成
    Mesh createCube(float size);
    //Mesh createBox(float sx, float sy, float sz);
    //Mesh createCylinder(float radius,
    //                    float height,
    //                    int segments);

    
};

#endif