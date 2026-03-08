#include "ModelBuilder.h"
#include <set>
#include <utility>
#include <math.h>

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

    buildEdges(m);

    m.upload();

    return m;
}
//-----------------

Model ModelBuilder::createXAxis(float L)
{
    Model m;

    m.vertices = {
        0,0,0,
        L,0,0
    };

    m.indices = {
        0,1
    };

    m.upload();
    return m;
}

Model ModelBuilder::createYAxis(float L)
{
    Model m;

    m.vertices = {
        0,0,0,
        0,L,0
    };

    m.indices = {
        0,1
    };

    m.upload();
    return m;
}

Model ModelBuilder::createZAxis(float L)
{
    Model m;

    m.vertices = {
        0,0,0,
        0,0,L
    };

    m.indices = {
        0,1
    };

    m.upload();
    return m;
}


//---------------------------------------------
Model ModelBuilder::create3Axis(float size)
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

//-------------------------------------------
void ModelBuilder::buildEdges(Model& m)
{
    std::set<std::pair<int,int>> edges;

    for(size_t i=0;i<m.indices.size();i+=3)
    {
        int a = m.indices[i];
        int b = m.indices[i+1];
        int c = m.indices[i+2];

        auto addEdge=[&](int v1,int v2)
        {
            if(v1>v2) std::swap(v1,v2);
            edges.insert({v1,v2});
        };

        addEdge(a,b);
        addEdge(b,c);
        addEdge(c,a);
    }

    for(auto&e:edges)
    {
        m.edgeIndices.push_back(e.first);
        m.edgeIndices.push_back(e.second);
    }


}
//-----------------
Model ModelBuilder::createCylinder(
    float radius,
    float height,
    int segments
)
{
    //Model ModelBuilder::createCylinder(float r,float h,int seg)
float r = radius;
float h = height;
int seg = segments;

    Model m;
    float half = h*0.5f;

    for(int i=0;i<seg;i++)
    {
        float a = 2*M_PI*i/seg;

        float x=r*cos(a);
        float z=r*sin(a);

        m.vertices.insert(m.vertices.end(),{x,half,z});
        m.vertices.insert(m.vertices.end(),{x,-half,z});
    }

    for(int i=0;i<seg;i++)
    {
        int n=(i+1)%seg;

        unsigned int t0=i*2;
        unsigned int b0=i*2+1;
        unsigned int t1=n*2;
        unsigned int b1=n*2+1;

        m.indices.insert(m.indices.end(),{t0,b0,t1});
        m.indices.insert(m.indices.end(),{t1,b0,b1});
    }

    buildEdges(m);
    m.upload();
    return m;
}