#ifndef __DRAWING_GLOBAL_HPP__
#define __DRAWING_GLOBAL_HPP__

#include <unordered_map>
#include <vector>
#include "ObjElement.hpp"

//Responsible for deleting all resources as well as drawing them
class DrawingGlobal
{
public:
    DrawingGlobal(
        std::unordered_map<Vertex::VertexID, Vertex*>& vertex_map,
        std::vector<Face*>& face_list);
    ~DrawingGlobal();

    void render_frame();
    
private:
    std::unordered_map<Vertex::VertexID, Vertex*>& vertex_map;
    std::vector<Face*>& face_list;
};

#endif