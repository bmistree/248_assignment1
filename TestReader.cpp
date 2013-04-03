#include <iostream>
#include <iomanip>
#include <string>
#include "ObjReader.hpp"
#include "ObjElement.hpp"
#include <vector>
#include <unordered_map>

int main(int argc, char** argv)
{
    std::string filename(argv[1]);
    
    std::vector<Face*> face_list;
    std::unordered_map<Vertex::VertexID,Vertex*> vertex_map;
    ObjReader::read_object_file(filename,vertex_map,face_list);
    
    for (std::vector<Face*>::const_iterator iter = face_list.begin();
         iter != face_list.end(); ++iter)
    {
        (*iter)->pretty_print();
        std::cout<<"\n";
    }

    for (std::unordered_map<Vertex::VertexID,Vertex*>::const_iterator iter = vertex_map.begin();
         iter != vertex_map.end(); ++iter)
    {
        iter->second->pretty_print();
        std::cout<<"\n";
    }
            
    return 0;
}
