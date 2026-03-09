#pragma once

#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "GLHeaders.h"
//#include <glad/glad.h>

#include "Model.h"
#include "Renderer.h"

class SceneObject
{
public:

    // local transform
    glm::mat4 transform = glm::mat4(1.0f);

    // optional model
    Model* model = nullptr;

    // rendering parameters
    glm::vec3 color = {1.0f,1.0f,1.0f};
    GLenum mode = GL_TRIANGLES;

    // hierarchy
    std::vector<std::shared_ptr<SceneObject>> children;

    SceneObject() = default;

    void addChild(std::shared_ptr<SceneObject> child);

    void draw(Renderer& renderer,
              const glm::mat4& parent);
};