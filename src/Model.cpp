#include "Model.h"

void Model::upload()
{
    glGenVertexArrays(1, &VAO_faces);
    glGenVertexArrays(1, &VAO_edges);

    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenBuffers(1, &edgeEBO);

    // ---------- faces ----------
    glBindVertexArray(VAO_faces);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,
        vertices.size()*sizeof(float),
        vertices.data(),
        GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        indices.size()*sizeof(unsigned int),
        indices.data(),
        GL_STATIC_DRAW);

    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);


    // ---------- edges ----------
    glBindVertexArray(VAO_edges);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, edgeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        edgeIndices.size()*sizeof(unsigned int),
        edgeIndices.data(),
        GL_STATIC_DRAW);

    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);


}

void Model::draw(GLenum mode)
{
    glBindVertexArray(VAO_faces);

    if(indices.size() > 0)
    {
        glDrawElements(
            mode,
            indices.size(),
            GL_UNSIGNED_INT,
            0
        );
    }
    else
    {
        glDrawArrays(
            mode,
            0,
            vertices.size()/3
        );
    }

    glBindVertexArray(0);
}


//---------------------------
void Model::drawEdges()
{
    glBindVertexArray(VAO_edges);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, edgeEBO);

    glDrawElements(
        GL_LINES,
        edgeIndices.size(),
        GL_UNSIGNED_INT,
        0
    );

    glBindVertexArray(0);
}