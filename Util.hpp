#ifndef __UTIL_HPP__
#define __UTIL_HPP__

#include <GL/gl.h>
#include <math.h>
#include <string>

#define OBJ_FILE_COMMENT_START '#'




/**
   Returns copy of string with no whitespace at beginning or end of string.
 */
void trim(std::string& line);
/**
   Comment begins at '#' and ends at end of line.  Removes everything in the
   comment from begin to end of line.
 */
void remove_comment(std::string& line);


struct Point4
{
    Point4()
    {}
    Point4(GLfloat _x, GLfloat _y, GLfloat _z, GLfloat _w=1.0f)
     : x(_x), y(_y), z(_z), w(_w)
    {}
    GLfloat x,y,z,w;

    
};

struct Point3
{
    Point3(){}
    Point3(GLfloat _x, GLfloat _y, GLfloat _z)
     : x(_x), y(_y), z(_z)
    {}
    GLfloat x,y,z;


    Point3 operator* (float b) const
    {
        return Point3(
            x * b,
            y * b,
            z * b);
    }
    
    Point3 operator- (const Point3& pt_b) const
    {
        return Point3(
            x - pt_b.x,
            y - pt_b.y,
            z - pt_b.z);
    }
    
    Point3 operator+ (const Point3& pt_b) const
    {
        return Point3(
            x + pt_b.x,
            y + pt_b.y,
            z + pt_b.z);
    }

    Point3 operator% (const Point3& pt_b) const
    {
        return Point3 (
            y * pt_b.z - z* pt_b.y,
            z * pt_b.x - x* pt_b.z,
            x * pt_b.y - y* pt_b.x);
    }

    void normalize()
    {
        float len = sqrt(x*x + y*y + z*z);
        x /= len;
        y /= len;
        z /= len;
    }
    
};



#endif
