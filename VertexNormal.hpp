#ifndef __VERTEX_NORMAL_HPP__
#define __VERTEX_NORMAL_HPP__

#include "Util.hpp"
#include <sstream>
#include <GL/gl.h>
#include <GL/glut.h>
#include <unordered_map>


class VertexNormal 
{
public:
    typedef uint64_t VertNormalId;
    typedef std::unordered_map<VertNormalId,VertexNormal*> VertNormalMap;
    typedef VertNormalMap::iterator VertNormalMapIter;
    typedef VertNormalMap::const_iterator VertNormalMapCIter;
    
    
    ~VertexNormal();
    static bool construct_from_line(VertNormalMap& vnmap,std::string line);

    VertNormalId vnid() const
    {
        return _vnid;
    }
    
    const Point3& pt() const
    {
        return _pt;
    }
    
    VertexNormal(VertNormalMap& vnmap,const GLfloat& x, const GLfloat& y, const GLfloat& z);
    
private:

    VertNormalId _vnid;    
    Point3 _pt;

    static VertNormalId _next_vnid()
    {
        static VertNormalId vnid = 0;
        return ++vnid;
    }
};

#endif
