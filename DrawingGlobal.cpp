#include "DrawingGlobal.hpp"
#include <unordered_map>
#include <vector>
#include "ObjElement.hpp"
#include <GL/gl.h>


DrawingGlobal::DrawingGlobal(
        std::unordered_map<Vertex::VertexID, Vertex*>& _vertex_map,
        std::vector<Face*>& _face_list)
 : vertex_map(_vertex_map),
   face_list(_face_list)
{}

DrawingGlobal::~DrawingGlobal()
{
    // free memory associated with vertex map
    for (std::unordered_map<Vertex::VertexID,Vertex*>::iterator iter = vertex_map.begin();
         iter != vertex_map.end(); ++iter)
    {
        delete iter->second;
    }

    vertex_map.clear();

    // free memory associated with face list
    for (std::vector<Face*>::iterator iter = face_list.begin();
         iter != face_list.end(); ++iter)
    {
        delete (*iter);
    }

    face_list.clear();
}


void DrawingGlobal::render_frame()
{
    glColor3f(.5,.5,.5);
    for (std::vector<Face*>::const_iterator citer = face_list.begin();
         citer != face_list.end(); ++citer)
    {
        (*citer)->draw_face(vertex_map);
    }
}
