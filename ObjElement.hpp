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
    typedef int TextureCoordId;
    typedef std::unordered_map<TextureCoordId,TextureCoordinate*> TextureCoordinateMap;
    
    ~TextureCoordinate();
    static bool construct_from_line(
        OpenVolumeMesh::GeometricPolyhedralMeshV4f* obj_mesh,
        TextureCoordinateMap& obj_tc_map,
        std::string line);
    virtual void pretty_print() const;

    TextureCoordId get_tc_id() const
    {
        return tid;
    }
            
    GLfloat get_u()
    {
        return u;
    }
    GLfloat get_v()
    {
        return v;
    }
    

private:
    TextureCoordinate(GLfloat u, GLfloat v, GLfloat w=0.0);
    GLfloat u,v,w;

    TextureCoordId tid;
};



class VertexNormal : public ObjElement
{
public:
//    typedef uint64_t VertexNormalId;
    typedef int VertexNormalId;
    typedef std::unordered_map<VertexNormalId,VertexNormal*> VertNormalMap;
    typedef VertNormalMap::iterator VertNormalMapIter;
    typedef VertNormalMap::const_iterator VertNormalMapCIter;

    typedef std::unordered_map<
        OpenVolumeMesh::VertexHandle,VertexNormal*,
        VertexHandleHasher> OpenVertNormalMap;

    virtual void pretty_print() const{}
    
    ~VertexNormal();
    static bool construct_from_line(
        VertNormalMap& vnmap,std::string line);

    static void calculate_normals(VertNormalMap& vnmap,
        OpenVolumeMesh::GeometricPolyhedralMeshV4f* obj_mesh);

    OpenVolumeMesh::Geometry::Vec3f open_vec3() const
    {
        return OpenVolumeMesh::Geometry::Vec3f (vn.x,vn.y,vn.z);
    }
    
    VertexNormalId get_vnid() const
    {
        return vnid;
    }
    
private:
    VertexNormal(const GLfloat& x, const GLfloat& y, const GLfloat& z);

    static OpenVolumeMesh::Geometry::Vec3f calc_normal(
        OpenVolumeMesh::VertexHandle vhandle0, OpenVolumeMesh::VertexHandle vhandle1,
        OpenVolumeMesh::VertexHandle vhandle2,
        OpenVolumeMesh::GeometricPolyhedralMeshV4f* obj_mesh, bool& valid);
    
    Point4 vn;
    VertexNormalId vnid;
};


class Face : public ObjElement
{
public:
    static OpenVolumeMesh::FaceHandle construct_from_line(
        OpenVolumeMesh::GeometricPolyhedralMeshV4f* obj_mesh,
        TextureCoordinate::TextureCoordinateMap& obj_tc_map,
        TextureCoordinate::TextureCoordinateMap& open_tc_map, 
        VertexNormal::VertNormalMap& obj_vnmap,VertexNormal::VertNormalMap& open_vnmap,
        const Vertex::VertexMap& vmap, std::string line);
    
    // static OpenVolumeMesh::FaceHandle construct_from_line(
    //     OpenVolumeMesh::GeometricPolyhedralMeshV4f* obj_mesh,
    //     VertexNormal::VertNormalMap& obj_vnmap,VertexNormal::VertNormalMap& open_vnmap,
    //     const Vertex::VertexMap& vmap, std::string line);
};

#endif
