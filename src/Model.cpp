#include "Model.h"

void Model::upload()
{
    glGenVertexArrays(1,&VAO);
    glGenBuffers(1,&VBO);
    glGenBuffers(1,&EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        vertices.size()*sizeof(float),
        vertices.data(),
        GL_STATIC_DRAW
    );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        indices.size()*sizeof(unsigned int),
        indices.data(),
        GL_STATIC_DRAW
    );

    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void Model::draw(GLenum mode)
{
    glBindVertexArray(VAO);

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