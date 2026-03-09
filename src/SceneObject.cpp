#include "SceneObject.h"
#include "Renderer.h"

void SceneObject::addChild(std::shared_ptr<SceneObject> child)
{
    children.push_back(child);
}

void SceneObject::draw(Renderer& renderer,
                       const glm::mat4& parent)
{
    glm::mat4 world = parent * transform;

    if(model)
    {
        renderer.draw(*model, world, color, mode);
        renderer.drawEdges(*model, world, glm::vec3(0.0f,0.0f,0.0f));
    }

    for(auto& c : children)
    {
        c->draw(renderer, world);
    }
}