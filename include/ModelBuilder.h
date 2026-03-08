#pragma once

#include "Model.h"

class ModelBuilder
{
public:

    static Model createCube(float size = 1.0f);
    static Model create3Axis(float length = 1.0f);
    static Model createXAxis(float L=1.0f);
    static Model createYAxis(float L=1.0f);
    static Model createZAxis(float L=1.0f);

    static Model createPlane(float size);
    static Model createCylinder(float radius = 1.0f, float height = 2.0f, int segments = 16);
    static Model createSphere(float radius, int stacks, int slices);

    static void buildEdges(Model& model);

};