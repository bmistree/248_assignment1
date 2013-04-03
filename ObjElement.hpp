#ifndef __OBJ_ELEMENT_HPP__
#define __OBJ_ELEMENT_HPP__

#define OBJ_FILE_COMMENT_START '#'

#include <string>
#include <GL/gl.h>
#include <vector>
#include <unordered_map>

// Pure virtual base class
class ObjElement
{
public:
    virtual void pretty_print() const = 0;
};


class Vertex: public ObjElement
{
public:
    typedef uint64_t VertexID;
    
    ~Vertex();
    /**
       @returns{ObjElement*} --- NULL if line did not match rule for this
       ObjElement.  Otherwise, a pointer to a newly-constructed object element.
     */
    static Vertex* construct_from_line(std::string line);
    virtual void pretty_print() const;

    VertexID get_vid()
    {
        return vid;
    }
    GLfloat get_x()
    {
        return x;
    }
    GLfloat get_y()
    {
        return y;
    }    
    GLfloat get_z()
    {
        return z;
    }
    GLfloat get_w()
    {
        return w;
    }
    
private:
    Vertex(GLfloat x, GLfloat y, GLfloat z, GLfloat w=1.0);
    GLfloat x, y, z, w;
    VertexID vid;

};

class TextureCoordinate : public ObjElement
{
public:
    ~TextureCoordinate();
    static TextureCoordinate* construct_from_line(std::string line);
    virtual void pretty_print() const;
    
private:
    TextureCoordinate(GLfloat u, GLfloat v, GLfloat w=0.0);
    GLfloat u,v,w;
};

class Face : public ObjElement
{
public:
    ~Face();
    static Face* construct_from_line(std::string line);
    virtual void pretty_print() const;

    void draw_face(const std::unordered_map<Vertex::VertexID,Vertex*>& vert_map) const;
    
private:
    Face(std::vector <Vertex::VertexID> vert_index_vec);
    std::vector<Vertex::VertexID>  vert_ids;
};


#endif
