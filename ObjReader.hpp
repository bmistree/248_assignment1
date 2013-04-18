#ifndef __OBJ_READER_HPP__
#define __OBJ_READER_HPP__

#include "ObjElement.hpp"
#include <string>
#include <OpenVolumeMesh/Mesh/PolyhedralMesh.hh>


class ObjReader
{
public:
    static void read_object_file(
        const std::string& filename,
        Vertex::VertMap& vmap,
        VertexNormal::VertNormalMap& vnmap,
        TextureCoordinate::TextCoordinateMap& tcmap,        
        Face::FaceMap& fmap);
    
private:

    /**
       @param{std::string} line --- Line from which to make ObjElement
     */
    static void read_element_from_string(
        const std::string& line,
        Vertex::VertMap& vmap,
        VertexNormal::VertNormalMap& vnmap,
        TextureCoordinate::TextCoordinateMap& tcmap,
        Face::FaceMap& fmap);
};

#endif
