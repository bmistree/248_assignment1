#include "ObjReader.hpp"
#include <fstream>
#include <iostream>
#include <string>


void ObjReader::read_object_file(
    const std::string& filename,
    Vertex::VertMap& vmap,
    VertexNormal::VertNormalMap& vnmap,    
    TextureCoordinate::TextCoordinateMap& tcmap,
    Face::FaceMap& fmap)
{
    std::ifstream file;
    file.open (filename.c_str());
    std::string single_line;
    while (getline(file,single_line))
    {
        read_element_from_string(
            single_line,vmap,vnmap,tcmap,fmap);
    }

    file.close();

    // calculate normals for all faces
    for (Face::FaceMapIter fiter = fmap.begin(); fiter != fmap.end();
         ++fiter)
    {
        fiter->second->calc_normals(vnmap);
    }
    
}


void ObjReader::read_element_from_string(
    const std::string& line_to_read,
    Vertex::VertMap& vmap,
    VertexNormal::VertNormalMap& vnmap,    
    TextureCoordinate::TextCoordinateMap& tcmap,
    Face::FaceMap& fmap)
{
    if (Vertex::construct_from_line(vmap,line_to_read))
        return;
    
    if (TextureCoordinate::construct_from_line(tcmap,line_to_read))
        return;

    if (VertexNormal::construct_from_line(vnmap,line_to_read))
        return;

    if (Face::construct_from_line(vmap,tcmap,vnmap,fmap,line_to_read))
        return;

}
