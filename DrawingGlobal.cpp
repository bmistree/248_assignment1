#include "DrawingGlobal.hpp"
#include <unordered_map>
#include <vector>
#include "ObjElement.hpp"
#include <GL/gl.h>
#include <GL/glut.h>

DrawingGlobal::DrawingGlobal(
        std::unordered_map<Vertex::VertexID, Vertex*>& _vertex_map,
        std::vector<Face*>& _face_list)
 : vertex_map(_vertex_map),
   face_list(_face_list)
{
    centroid_x = 0;
    centroid_y = 0;
    centroid_z = 0;

    max_x = 0;
    max_y = 0;
    max_z = 0;

    min_x = 0;
    min_y = 0;
    min_z = 0;


    for (std::vector<Face*>::const_iterator citer = face_list.begin();
         citer != face_list.end(); ++citer)
    {
        Face* face = *citer;

        GLfloat cx,cy,cz;
        GLfloat maxx,maxy,maxz;
        GLfloat minx,miny,minz;

        face->centroid_and_maxes(cx,cy,cz,maxx,maxy,maxz,minx,miny,minz, vertex_map);

        if (citer == face_list.begin())
        {
            max_x = maxx;
            max_y = maxy;
            max_z = maxz;
            min_x = minx;
            min_y = miny;
            min_z = minz;
        }

        if (max_x < maxx) max_x = maxx;
        if (max_y < maxy) max_y = maxy;
        if (max_z < maxz) max_z = maxz;
        
        if (min_x > minx) min_x = minx;
        if (min_y > miny) min_y = miny;
        if (min_z > minz) min_z = minz;

        centroid_x += cx;
        centroid_y += cy;
        centroid_z += cz;        
    }

    centroid_x /= ((GLfloat) face_list.size());
    centroid_y /= ((GLfloat) face_list.size());
    centroid_z /= ((GLfloat) face_list.size());
}

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
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluLookAt(
        // eye is positioned at 5z
        // 0.f,0.f,1.2f,
        0.f,0.f,3.2f,
       // 0.f,0.f,.5f,
        // looking at origin
        0.f,0.f,0.f,
        // camera is oriented so that it's top is in the y direction
        0.f,1.f,0.f);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity(); // clear current matrix
    glTranslatef(-centroid_x,-centroid_y,-centroid_z);
    // glTranslatef(centroid_x,centroid_y,centroid_z);
    glColor3f(.5,.5,.5);
    for (std::vector<Face*>::const_iterator citer = face_list.begin();
         citer != face_list.end(); ++citer)
    {
        (*citer)->draw_face(vertex_map);
    }

    
    
}
