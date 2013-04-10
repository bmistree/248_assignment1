#ifndef __UTIL_HPP__
#define __UTIL_HPP__

#include <GL/gl.h>

struct Point4
{
    Point4()
    {}
    Point4(GLfloat _x, GLfloat _y, GLfloat _z, GLfloat _w=1.0f)
     : x(_x), y(_y), z(_z), w(_w)
    {}
    GLfloat x,y,z,w;
};

#endif
