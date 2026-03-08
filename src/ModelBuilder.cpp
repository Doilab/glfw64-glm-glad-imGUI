#include "ModelBuilder.h"
#include <set>
#include <utility>

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