#ifndef __OBJ_ELEMENT_HPP__
#define __OBJ_ELEMENT_HPP__

#include "Util.hpp"
#include <string>
#include <GL/gl.h>
#include <vector>
#include <unordered_map>
#include <OpenVolumeMesh/Mesh/PolyhedralMesh.hh>


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
    ~Vertex();
    /**
       @returns{ObjElement*} --- NULL if line did not match rule for this
       ObjElement.  Otherwise, a pointer to a newly-constructed object element.
     */
    typedef uint64_t VertexId;
    typedef std::unordered_map<VertexId,Vertex*> VertexMap;
    typedef VertexMap::iterator VertexMapIter;
    typedef VertexMap::const_iterator VertexMapCIter;

    
    static Vertex* construct_from_line(
        OpenVolumeMesh::GeometricPolyhedralMeshV4f* obj_mesh,std::string line);
    virtual void pretty_print() const;

    VertexId get_vid() const
    {
        return vid;
    }

    OpenVolumeMesh::VertexHandle get_ovm_id() const
    {
        return ovm_id;
    }
    
private:
    Vertex(OpenVolumeMesh::GeometricPolyhedralMeshV4f* obj_mesh,
        float x, float y, float z, float w=1.0f);
    OpenVolumeMesh::VertexHandle ovm_id;
    VertexId vid;
};

class TextureCoordinate : public ObjElement
{
public:
    ~TextureCoordinate();
    static TextureCoordinate* construct_from_line(
        OpenVolumeMesh::GeometricPolyhedralMeshV4f* obj_mesh,
        std::string line);
    virtual void pretty_print() const;
    typedef uint64_t TextureCoordID;

private:
    TextureCoordinate(GLfloat u, GLfloat v, GLfloat w=0.0);
    GLfloat u,v,w;

    TextureCoordID tid;
};



class VertexNormal : public ObjElement
{
public:
    typedef uint64_t VertexNormalID;
    typedef std::unordered_map<VertexNormalID,VertexNormal*> VertNormalMap;
    typedef VertNormalMap::iterator VertNormalMapIter;
    typedef VertNormalMap::const_iterator VertNormalMapCIter;
    
    ~VertexNormal();
    static VertexNormal* construct_from_line(
        OpenVolumeMesh::GeometricPolyhedralMeshV4f* obj_mesh,
        std::string line);
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
    static OpenVolumeMesh::FaceHandle construct_from_line(
        OpenVolumeMesh::GeometricPolyhedralMeshV4f* obj_mesh,
        const Vertex::VertexMap& vmap, std::string line);
};

#endif
