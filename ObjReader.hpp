#ifndef __OBJ_READER_HPP__
#define __OBJ_READER_HPP__

#include "ObjElement.hpp"
#include <string>
#include <OpenVolumeMesh/Mesh/PolyhedralMesh.hh>


class ObjReader
{
public:
    static OpenVolumeMesh::GeometricPolyhedralMeshV4f* read_object_file(
        const std::string& filename,
        TextureCoordinate::TextureCoordinateMap& obj_tc_map,
        TextureCoordinate::TextureCoordinateMap& open_tc_map,
        VertexNormal::VertNormalMap& obj_vnmap,
        VertexNormal::VertNormalMap& open_vnmap,
        Vertex::VertexMap& vmap);
    
private:

    /**
       @param{std::string} line --- Line from which to make ObjElement
     */
    static void read_element_from_string(
        const std::string& line,
        TextureCoordinate::TextureCoordinateMap& obj_tc_map,
        TextureCoordinate::TextureCoordinateMap& open_tc_map,        
        VertexNormal::VertNormalMap& obj_vnmap,
        VertexNormal::VertNormalMap& open_vnmap,
        Vertex::VertexMap& vmap,
        OpenVolumeMesh::GeometricPolyhedralMeshV4f*);

    /**
       @param {std::string} filename --- Reads the .obj file stored by filename
       and returns all elements it contains as a vector.
     */
    static void read_all_file_elements(
        const std::string& filename,
        TextureCoordinate::TextureCoordinateMap& obj_tc_map,
        TextureCoordinate::TextureCoordinateMap& open_tc_map,
        VertexNormal::VertNormalMap& obj_vnmap,
        VertexNormal::VertNormalMap& open_vnmap,
        Vertex::VertexMap& vmap,
        OpenVolumeMesh::GeometricPolyhedralMeshV4f* obj_mesh);
};

#endif
