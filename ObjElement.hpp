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
    /**
       @returns{ObjElement*} --- NULL if line did not match rule for this
       ObjElement.  Otherwise, a pointer to a newly-constructed object element.
     */
    typedef uint64_t VertexId;
    typedef std::unordered_map<VertexId,OpenVolumeMesh::VertexHandle> VertexMap;
    typedef VertexMap::iterator VertexMapIter;
    typedef VertexMap::const_iterator VertexMapCIter;

    static OpenVolumeMesh::VertexHandle construct_from_line(
        OpenVolumeMesh::GeometricPolyhedralMeshV4f* obj_mesh,VertexMap& vmap,
        std::string line);
};

class TextureCoordinate : public ObjElement
{
public:
    ~TextureCoordinate();
    static TextureCoordinate* construct_from_line(
        OpenVolumeMesh::GeometricPolyhedralMeshV4f* obj_mesh,
        std::string line);
    virtual void pretty_print() const;
    typedef uint64_t TextureCoordId;

private:
    TextureCoordinate(GLfloat u, GLfloat v, GLfloat w=0.0);
    GLfloat u,v,w;

    TextureCoordId tid;
};



class VertexNormal : public ObjElement
{
public:
    typedef uint64_t VertexNormalId;
    typedef std::unordered_map<VertexNormalId,VertexNormal*> VertNormalMap;
    typedef VertNormalMap::iterator VertNormalMapIter;
    typedef VertNormalMap::const_iterator VertNormalMapCIter;
    
    ~VertexNormal();
    static VertexNormal* construct_from_line(
        OpenVolumeMesh::GeometricPolyhedralMeshV4f* obj_mesh,
        std::string line);
    virtual void pretty_print() const;
    
    VertexNormalId get_vnid() const
    {
        return vnid;
    }
    
private:
    VertexNormal(const GLfloat& x, const GLfloat& y, const GLfloat& z);
    Point4 vn;
    VertexNormalId vnid;
};


class Face : public ObjElement
{
public:
    static OpenVolumeMesh::FaceHandle construct_from_line(
        OpenVolumeMesh::GeometricPolyhedralMeshV4f* obj_mesh,
        const Vertex::VertexMap& vmap, std::string line);
};

#endif
