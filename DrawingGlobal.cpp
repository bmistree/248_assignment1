#include "DrawingGlobal.hpp"
#include <unordered_map>
#include <vector>
#include "ObjElement.hpp"
#include <GL/gl.h>
#include <GL/glut.h>
#include <iostream>

DrawingGlobal::DrawingGlobal(
        std::unordered_map<Vertex::VertexID, Vertex*>& _vertex_map,
        std::vector<Face*>& _face_list)
 : vertex_map(_vertex_map),
   face_list(_face_list)
{
    eye.x = INITIAL_EYE_X;
    eye.y = INITIAL_EYE_Y;
    eye.z = INITIAL_EYE_Z;
    
    // Calculate obj centroid and scale image
    centroid.x = 0;
    centroid.y = 0;
    centroid.z = 0;

    max.x = 0;
    max.y = 0;
    max.z = 0;
    min.x = 0;
    min.y = 0;
    min.z = 0;

    for (std::vector<Face*>::const_iterator citer = face_list.begin();
         citer != face_list.end(); ++citer)
    {
        Face* face = *citer;

        Point4 c,maxes,mins;

        face->centroid_and_maxes(c,maxes,mins, vertex_map);

        if (citer == face_list.begin())
        {
            max.x = maxes.x;
            max.y = maxes.y;
            max.z = maxes.z;
            min.x = mins.x;
            min.y = mins.y;
            min.z = mins.z;
        }

        if (max.x < maxes.x) max.x = maxes.x;
        if (max.y < maxes.y) max.y = maxes.y;
        if (max.z < maxes.z) max.z = maxes.z;
        
        if (min.x > mins.x) min.x = mins.x;
        if (min.y > mins.y) min.y = mins.y;
        if (min.z > mins.z) min.z = mins.z;

        centroid.x += c.x;
        centroid.y += c.y;
        centroid.z += c.z;        
    }

    centroid.x /= ((GLfloat) face_list.size());
    centroid.y /= ((GLfloat) face_list.size());
    centroid.z /= ((GLfloat) face_list.size());

}

void DrawingGlobal::set_window_width_height(
    GLfloat _window_width,GLfloat _window_height)
{
    window_width = _window_width;
    window_height = _window_height;
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

void DrawingGlobal::keyboard_func(unsigned char key,int x, int y)
{
    if (key == 'i')
        eye.z -= INCREMENT_POS_ON_KEY;
    else if (key == 'k')
        eye.z += INCREMENT_POS_ON_KEY;
    else if (key == 'l')
        eye.x += INCREMENT_POS_ON_KEY;
    else if (key == 'j')
        eye.x -= INCREMENT_POS_ON_KEY;
    else if (key == 'w')
        eye.y += INCREMENT_POS_ON_KEY;
    else if (key == 's')
        eye.y -= INCREMENT_POS_ON_KEY;

    glutPostRedisplay();    
}


void DrawingGlobal::render_frame()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
    glLoadIdentity();

    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(
        PERSPECTIVE_NEAR_PLANE_ANGLE,  // angle to top from center
        (window_width) / (window_height), //aspect ratio
        .01f, // dist to near clip plane
        100.f // dist to far clip plane
    );


    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(
        // eye positioning
        eye.x,eye.y,eye.z,
        eye.x ,eye.y, eye.z - 1.f,
        // // looking at origin
        // 0.f,0.f,0.f,
        // camera is oriented so that it's top is in the y direction
        0.f,1.f,0.f);
    

    
    glTranslatef(-centroid.x,-centroid.y,-centroid.z);
    glColor3f(.5f,.5f,.5f);
    for (std::vector<Face*>::const_iterator citer = face_list.begin();
         citer != face_list.end(); ++citer)
    {
        (*citer)->draw_face(vertex_map);
    }

    glutSwapBuffers();
    glPopMatrix();
    glFlush();
    
}
