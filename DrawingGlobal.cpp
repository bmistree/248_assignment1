#include "DrawingGlobal.hpp"
#include <unordered_map>
#include <vector>
#include "ObjElement.hpp"
#include <GL/gl.h>
#include <GL/glut.h>
#include <iostream>
#include <OpenVolumeMesh/Mesh/PolyhedralMesh.hh>

DrawingGlobal::DrawingGlobal(
    OpenVolumeMesh::GeometricPolyhedralMeshV4f* _obj_mesh,
    VertexNormal::VertNormalMap* _vnmap, Vertex::VertexMap* _vmap)
 : obj_mesh(_obj_mesh), vnmap(_vnmap),vmap(_vmap),shading(GL_FLAT),
   gl_begin_type(GL_TRIANGLES)
{
    diffuse[0] = 1.0;
    diffuse[1] = 1.0;
    diffuse[2] = 1.0;
    diffuse[3] = 1.0;

    specular[0] = 1.0;
    specular[1] = 1.0;
    specular[2] = 1.0;
    specular[3] = 1.0;

    shininess[0] = 10.0f;

    ambient[0] = .1;
    ambient[1] = .1;
    ambient[2] = .1;
    ambient[3] = 1.0;

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

    else if (key == '1') // smooth shading
        shading = GL_SMOOTH;
    else if (key == '2')
        shading = GL_FLAT;
    else if (key == '3')
        gl_begin_type = GL_LINE_LOOP;
    else if (key == '4')
        gl_begin_type = GL_TRIANGLES;
    
    glutPostRedisplay();    
}


void DrawingGlobal::render_frame()
{
    glShadeModel(shading);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);

    
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

    // draw ground plane before lookat
    glColor3f(.7f,0.f,0.f);
    float y_pos = -4.f;
    glBegin(GL_QUADS);
    glNormal3f(0,.707,-.707);
    glVertex3f(-20.f,y_pos,-20.f);
    glVertex3f(-20.f,y_pos,20.f);
    glVertex3f(20.f,y_pos,20.f);
    glVertex3f(20.f,y_pos,-20.f);
    glEnd();

        
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
        glBegin(gl_begin_type);
        
        const OpenVolumeMesh::OpenVolumeMeshFace& face = obj_mesh->face(*iter);
        const std::vector<OpenVolumeMesh::HalfEdgeHandle>& half_edge_handles = face.halfedges();

        // calculate the face normal as average of vertex normal and draw points.
        std::vector<OpenVolumeMesh::Geometry::Vec4f> vertex_points;
        std::vector<OpenVolumeMesh::Geometry::Vec3f> vertex_normals;
        OpenVolumeMesh::Geometry::Vec3f vertex_normal(0,0,0);
        for (std::vector<OpenVolumeMesh::HalfEdgeHandle>::const_iterator he_iter = half_edge_handles.begin();
             he_iter != half_edge_handles.end(); ++he_iter)
        {
            const OpenVolumeMesh::OpenVolumeMeshEdge& edge = obj_mesh->halfedge(*he_iter);
            const OpenVolumeMesh::VertexHandle& from_vertex = edge.from_vertex();
            const OpenVolumeMesh::Geometry::Vec4f& vertex_vec = obj_mesh->vertex(from_vertex);

            vertex_points.push_back(vertex_vec);
            vertex_normals.push_back((*vnmap)[from_vertex]->open_vec3());
            vertex_normal += (*vnmap)[from_vertex]->open_vec3();
        }

        if (shading == GL_FLAT)
        {
            float averager = 1./((float)half_edge_handles.size());
            vertex_normal *= averager;
            glNormal3f(
                vertex_normal[0],
                vertex_normal[1],
                vertex_normal[2]);
        }        
        
        // actually draw the points
        uint64_t counter = 0;
        for (std::vector<OpenVolumeMesh::Geometry::Vec4f>::const_iterator citer = vertex_points.begin();
             citer != vertex_points.end(); ++citer)
        {
            if (shading == GL_SMOOTH)
            {
                OpenVolumeMesh::Geometry::Vec3f smooth_normal = vertex_normals[counter];
                glNormal3f(
                    smooth_normal[0],
                    smooth_normal[1],
                    smooth_normal[2]);
            }
            
            glVertex4f(
                (GLfloat)((*citer)[0]),
                (GLfloat)((*citer)[1]),
                (GLfloat)((*citer)[2]),
                (GLfloat)((*citer)[3]));
        }
        
        
        glEnd();
    }


    glutSwapBuffers();
    glPopMatrix();
    glFlush();
    
}

