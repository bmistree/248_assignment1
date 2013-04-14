#include "DrawingGlobal.hpp"
#include <unordered_map>
#include <vector>
#include "ObjElement.hpp"
#include <GL/gl.h>
#include <GL/glut.h>
#include <iostream>
#include <OpenVolumeMesh/Mesh/PolyhedralMesh.hh>

DrawingGlobal::DrawingGlobal(
    OpenVolumeMesh::GeometricPolyhedralMeshV4f* _obj_mesh,Vertex::VertexMap* _vmap)
 : obj_mesh(_obj_mesh), vmap(_vmap)
{
    eye.x = INITIAL_EYE_X;
    eye.y = INITIAL_EYE_Y;
    eye.z = INITIAL_EYE_Z;
}

void DrawingGlobal::set_window_width_height(
    GLfloat _window_width,GLfloat _window_height)
{
    window_width = _window_width;
    window_height = _window_height;
}

DrawingGlobal::~DrawingGlobal()
{}

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
    
    glColor3f(.5f,.5f,.5f);
    for (OpenVolumeMesh::FaceIter iter = obj_mesh->faces_begin();
         iter != obj_mesh->faces_end(); ++iter)
    {
        glBegin(GL_LINE_LOOP);
        
        const OpenVolumeMesh::OpenVolumeMeshFace& face = obj_mesh->face(*iter);
        const std::vector<OpenVolumeMesh::HalfEdgeHandle>& half_edge_handles = face.halfedges();

        for (std::vector<OpenVolumeMesh::HalfEdgeHandle>::const_iterator he_iter = half_edge_handles.begin();
             he_iter != half_edge_handles.end(); ++he_iter)
        {
            const OpenVolumeMesh::OpenVolumeMeshEdge& edge = obj_mesh->halfedge(*he_iter);
            const OpenVolumeMesh::VertexHandle& from_vertex = edge.from_vertex();
            const OpenVolumeMesh::Geometry::Vec4f& vertex_vec = obj_mesh->vertex(from_vertex);

            glVertex4f(
                (GLfloat)vertex_vec[0],
                (GLfloat)vertex_vec[1],
                (GLfloat)vertex_vec[2],
                (GLfloat)vertex_vec[3]);
        }
        
        glEnd();
    }
    
    glutSwapBuffers();
    glPopMatrix();
    glFlush();
    
}
