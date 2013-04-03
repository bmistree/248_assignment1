#ifndef __OBJ_ELEMENT_HPP__
#define __OBJ_ELEMENT_HPP__

#define OBJ_FILE_COMMENT_START '#'

#include <string>
#include <GL/gl.h>


// Pure virtual base class
class ObjElement
{
public:
    virtual void pretty_print() const = 0;
};


class ListOfVertices: public ObjElement
{
public:
    ~ListOfVertices();

    /**
       @returns{ObjElement*} --- NULL if line did not match rule for this
       ObjElement.  Otherwise, a pointer to a newly-constructed object element.
     */
    static ListOfVertices* construct_from_line(std::string line);
    
    virtual void pretty_print() const;
    
private:
    ListOfVertices(GLfloat x, GLfloat y, GLfloat z, GLfloat w=1.0);

    GLfloat x, y, z, w;
};


#endif __OBJ_ELEMENT_HPP__
