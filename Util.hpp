#ifndef __UTIL_HPP__
#define __UTIL_HPP__

#include <GL/gl.h>
#include <OpenVolumeMesh/Mesh/PolyhedralMesh.hh>

struct Point4
{
    Point4()
    {}
    Point4(GLfloat _x, GLfloat _y, GLfloat _z, GLfloat _w=1.0f)
     : x(_x), y(_y), z(_z), w(_w)
    {}
    GLfloat x,y,z,w;
};


class VertexHandleHasher{
public:
    size_t operator() (const OpenVolumeMesh::VertexHandle& vh) const
    {
        return hasher(vh);
    }

    static size_t hasher (const OpenVolumeMesh::VertexHandle& vh)
    {
        return vh.idx();
    }
};

class bitmap_image
{
public:
    bitmap_image(int _w,int _h, int _n, unsigned char* _data)
     : width(_w),height(_h), n(_n),data(_data)
    {}
    int width,height,n;
    unsigned char* data;
};



#endif
