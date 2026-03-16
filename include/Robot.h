#pragma once

#include <vector>
#include <memory>
#include <string>
#include <iostream>

#include <nlohmann/json.hpp>

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

#include "SceneObject.h"

struct RobotState
{
    int id = 0;

    glm::vec3 position{0,0,0};
    glm::quat orientation{1,0,0,0};

    std::vector<float> joint;

    double time = 0;

    nlohmann::json to_json() const
    {
        nlohmann::json j;

        j["type"] = "robot_state";
        j["id"] = id;

        j["position"] = {position.x, position.y, position.z};

        j["orientation"] = {
            orientation.w,
            orientation.x,
            orientation.y,
            orientation.z
        };

        j["joint"] = joint;
        j["time"] = time;

        return j;
    }

    static RobotState from_json(const nlohmann::json& j)
    {
        RobotState s;

        s.id = j["id"];

        auto p = j["position"];
        s.position = glm::vec3(p[0], p[1], p[2]);

        auto q = j["orientation"];
        s.orientation = glm::quat(q[0], q[1], q[2], q[3]);

        s.joint = j["joint"].get<std::vector<float>>();

        s.time = j["time"];

        return s;
    }
    void dumpRobotState(const RobotState& s)
    {
        std::cout << s.to_json().dump(2) << std::endl;
    }

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