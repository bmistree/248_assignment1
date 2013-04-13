#ifndef __OBJ_ELEMENT_HPP__
#define __OBJ_ELEMENT_HPP__

#include "Util.hpp"
#include <string>
#include <GL/gl.h>
#include <vector>
#include <unordered_map>

#define OBJ_FILE_COMMENT_START '#'

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

    VertexID get_vid() const
    {
        return vid;
    }
    GLfloat get_x() const
    {
        return vert_pt.x;
    }
    GLfloat get_y() const
    {
        return vert_pt.y;
    }    
    GLfloat get_z() const
    {
        return vert_pt.z;
    }
    GLfloat get_w() const
    {
        return vert_pt.w;
    }
    
private:
    Vertex(GLfloat x, GLfloat y, GLfloat z, GLfloat w=1.0f);
    Point4 vert_pt;
    VertexID vid;

};

class TextureCoordinate : public ObjElement
{
public:
    ~TextureCoordinate();
    static TextureCoordinate* construct_from_line(std::string line);
    virtual void pretty_print() const;
    typedef Vertex::VertexID TextureCoordID;
    
private:
    TextureCoordinate(GLfloat u, GLfloat v, GLfloat w=0.0);
    GLfloat u,v,w;

    TextureCoordID tid;
};



class VertexNormal : public ObjElement
{
public:
    typedef Vertex::VertexID VertexNormalID;
    typedef std::unordered_map<VertexNormalID,VertexNormal*> VertNormalMap;
    typedef VertNormalMap::iterator VertNormalMapIter;
    typedef VertNormalMap::const_iterator VertNormalMapCIter;
    
    ~VertexNormal();
    static VertexNormal* construct_from_line(std::string line);
    virtual void pretty_print() const;
    
    VertexNormalID get_vnid() const
    {
        return vnid;
    }
    
private:
    VertexNormal(const GLfloat& x, const GLfloat& y, const GLfloat& z);
    Point4 vn;
    VertexNormalID vnid;
};






class Face : public ObjElement
{
public:
    ~Face();
    static Face* construct_from_line(std::string line);
    virtual void pretty_print() const;

    void draw_face(const std::unordered_map<Vertex::VertexID,Vertex*>& vert_map) const;
    void centroid_and_maxes(
        Point4& centroid,
        Point4& max,Point4& min,
        const std::unordered_map<Vertex::VertexID,Vertex*>& vert_map) const;
    
    
private:

    struct VertexDescriptor
    {
        VertexDescriptor(
            Vertex::VertexID* _vid, TextureCoordinate::TextureCoordID* _tid,
            VertexNormal::VertexNormalID* _vnid)
         : vid(_vid), tid(_tid), vnid(_vnid)
        {}

        ~VertexDescriptor()
        {
            if (vid != NULL)
            {
                delete vid;
                vid = NULL;
            }
            if (tid != NULL)
            {
                delete tid;
                tid = NULL;
            }
            if (vnid != NULL)
            {
                delete vnid;
                vnid = NULL;
            }
        }
        
        
        Vertex::VertexID* vid;
        TextureCoordinate::TextureCoordID* tid;
        VertexNormal::VertexNormalID* vnid;
    };
    
    Face();
    void add_vertex_descriptor(VertexDescriptor* vd);
    std::vector<VertexDescriptor*> vert_descriptors;
};

#endif
