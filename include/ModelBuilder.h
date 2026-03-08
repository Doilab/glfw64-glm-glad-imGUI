#pragma once

#include "Model.h"

class ModelBuilder
{
public:

    static Model createCube(float size = 1.0f);
    static Model createAxis(float length = 1.0f);
    static Model createPlane(float size);
    static Model createCylinder(float radius, float height, int segments);
    static Model createSphere(float radius, int stacks, int slices);

};