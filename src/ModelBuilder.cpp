#include "ModelBuilder.h"
#include <set>
#include <utility>
#include <math.h>
#include <fstream>
#include <vector>
#include <string>
#include <cstdint>
#include <iostream>

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

Model ModelBuilder::createBox(float w, float h, float d)
{
    Model m;

    w = w * 0.5f;
    h = h * 0.5f;
    d = d * 0.5f;

    m.vertices =
    {
        -w,-h,-d,
         w,-h,-d,
         w, h,-d,
        -w, h,-d,

        -w,-h, d,
         w,-h, d,
         w, h, d,
        -w, h, d
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

Model ModelBuilder::loadSTL(const std::string& filename, float scale)
{
    Model model;

    std::ifstream file(filename, std::ios::binary);
    if(!file)
    {
        std::cerr << "STL load failed: " << filename << '\n';
        return model;
    }

    // ----- header -----
    char header[80];
    file.read(header,80);

    uint32_t triCount;
    file.read(reinterpret_cast<char*>(&triCount),4);

    // ----- triangles -----
    for(uint32_t i=0;i<triCount;i++)
    {
        float normal[3];
        float v[9];
        uint16_t attr;

        file.read(reinterpret_cast<char*>(normal),12);
        file.read(reinterpret_cast<char*>(v),36);
        file.read(reinterpret_cast<char*>(&attr),2);

        unsigned int base = model.vertices.size()/3;

        // v1
        model.vertices.push_back(v[0]*scale);
        model.vertices.push_back(v[1]*scale);
        model.vertices.push_back(v[2]*scale);

        // v2
        model.vertices.push_back(v[3]*scale);
        model.vertices.push_back(v[4]*scale);
        model.vertices.push_back(v[5]*scale);

        // v3
        model.vertices.push_back(v[6]*scale);
        model.vertices.push_back(v[7]*scale);
        model.vertices.push_back(v[8]*scale);

        // triangle indices
        model.indices.push_back(base);
        model.indices.push_back(base+1);
        model.indices.push_back(base+2);
    }

    // ----- edges -----
    buildEdges(model);

    // ----- GPU upload -----
    model.upload();

    return model;
}
