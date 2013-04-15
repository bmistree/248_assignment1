#include "ObjReader.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <OpenVolumeMesh/Mesh/PolyhedralMesh.hh>


OpenVolumeMesh::GeometricPolyhedralMeshV4f* ObjReader::read_object_file(
    const std::string& filename,VertexNormal::VertNormalMap& obj_vnmap,
    VertexNormal::VertNormalMap& open_vnmap,
    Vertex::VertexMap& vmap)
{
    OpenVolumeMesh::GeometricPolyhedralMeshV4f* obj_mesh =
        new OpenVolumeMesh::GeometricPolyhedralMeshV4f;
    
    read_all_file_elements(filename,obj_vnmap,open_vnmap,vmap,obj_mesh);
    return obj_mesh;
}


void ObjReader::read_all_file_elements(
    const std::string& filename,
    VertexNormal::VertNormalMap& obj_vnmap,
    VertexNormal::VertNormalMap& open_vnmap,
    Vertex::VertexMap& vmap,
    OpenVolumeMesh::GeometricPolyhedralMeshV4f* obj_mesh)
{
    std::ifstream file;
    file.open (filename.c_str());
    std::string single_line;
    while (getline(file,single_line))
        read_element_from_string(single_line,obj_vnmap,open_vnmap,vmap,obj_mesh);

    file.close();
}


void ObjReader::read_element_from_string(
    const std::string& line_to_read,
    VertexNormal::VertNormalMap& obj_vnmap,
    VertexNormal::VertNormalMap& open_vnmap,
    Vertex::VertexMap& vmap,
    OpenVolumeMesh::GeometricPolyhedralMeshV4f* obj_mesh)
{
    OpenVolumeMesh::VertexHandle vh = Vertex::construct_from_line(
        obj_mesh,vmap,line_to_read);
    if (vh.is_valid())
        return;

    // if (VertexCoordinate::construct_from_line(line_to_read))
    //     return;

    OpenVolumeMesh::FaceHandle fh = Face::construct_from_line(
        obj_mesh,obj_vnmap,open_vnmap,vmap,line_to_read);
    if (fh.is_valid())
        return;
    
    if (VertexNormal::construct_from_line(obj_vnmap,line_to_read))
        return;
    
}
