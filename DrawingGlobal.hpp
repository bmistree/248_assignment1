#ifndef __DRAWING_GLOBAL_HPP__
#define __DRAWING_GLOBAL_HPP__

#include <unordered_map>
#include <vector>
#include "ObjElement.hpp"

#define PERSPECTIVE_NEAR_PLANE_ANGLE 60.f

//Responsible for deleting all resources as well as drawing them
class DrawingGlobal
{
public:
    DrawingGlobal(
        std::unordered_map<Vertex::VertexID, Vertex*>& vertex_map,
        std::vector<Face*>& face_list);
    ~DrawingGlobal();

    void set_window_width_height(GLfloat window_width,GLfloat window_height);

    void render_frame();
    
private:
    std::unordered_map<Vertex::VertexID, Vertex*>& vertex_map;
    std::vector<Face*>& face_list;

    GLfloat centroid_x,centroid_y,centroid_z;
    GLfloat max_x,max_y, max_z;
    GLfloat min_x, min_y, min_z;

    GLfloat window_width,window_height;
    
};

#endif
