#include "ModelBuilder.h"

Model ModelBuilder::createCube(float s)
{
    Model m;

    float h = s * 0.5f;

    m.vertices =
    {
        -h,-h,-h,
         h,-h,-h,
         h, h,-h,
        -h, h,-h,

        -h,-h, h,
         h,-h, h,
         h, h, h,
        -h, h, h
    };

    m.indices =
    {
        0,1,2, 0,2,3,
        4,5,6, 4,6,7,
        0,1,5, 0,5,4,
        2,3,7, 2,7,6,
        1,2,6, 1,6,5,
        3,0,4, 3,4,7
    };

    m.upload();

    return m;
}
//-----------------
Model ModelBuilder::createAxis(float size)
{
    Model m;
    m.vertices = {
    0,0,0,  size,0,0,
    0,0,0,  0,size,0,
    0,0,0,  0,0,size
    };

    m.indices = {
    0,1,
    2,3,
    4,5
    };

    m.upload();
    return m;
}