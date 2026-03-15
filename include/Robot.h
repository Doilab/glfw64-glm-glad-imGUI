#pragma once

#include <vector>
#include <memory>

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

#include "SceneObject.h"

struct RobotState
{
    int id = 0; // ID

    glm::vec3 position {0,0,0}; // 位置
    glm::quat orientation {1,0,0,0}; // 姿勢（クォータニオン）

    std::vector<float> joint; // 関節角

    double time = 0;
};

struct RobotJoint
{
    std::shared_ptr<SceneObject> node;
    glm::vec3 axis;
    glm::mat4 baseTransform;
};

class Robot
{
public:

    RobotState state;

    std::vector<RobotJoint> joints;

    Robot(int jointNum = 12)
    {
        state.joint.resize(jointNum,0.0f);
    }
    
    void addJoint(std::shared_ptr<SceneObject> node,
                  glm::vec3 axis)
    {
        RobotJoint j;
        j.node = node;
        j.axis = axis;
        j.baseTransform = node->transform;

        joints.push_back(j);
    }

    void update()
    {
        for(size_t i=0;i<joints.size();i++)
        {
            glm::mat4 M = joints[i].baseTransform;

            M = glm::rotate(M, state.joint[i], joints[i].axis);

            joints[i].node->transform = M;
        }
    }
    
    void setPose(glm::vec3 p, glm::quat q)
    {
        state.position = p;
        state.orientation = q;
    }
};