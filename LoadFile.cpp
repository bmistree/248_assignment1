#include <iostream>
#include <iomanip>
#include <string>
#include "ObjReader.hpp"
#include "ObjElement.hpp"
#include <vector>
#include <unordered_map>
#include <GL/glut.h>
#include <GL/gl.h>
#include "DrawingGlobal.hpp"
#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

// Also responsible for cleanup of all data.
DrawingGlobal* drawing_global = NULL;

void setup_gl(const std::string& filename, int argc,char** argv);

void draw_faces(
    std::vector<Face*>&face_list,
    std::unordered_map<Vertex::VertexID,Vertex*>& vertex_map);

void render_frame(void);
GLfloat gl_max(GLfloat a, GLfloat b);

int main(int argc, char** argv)
{
    std::string filename(argv[1]);
    
    std::vector<Face*> face_list;
    std::unordered_map<Vertex::VertexID,Vertex*> vertex_map;
    ObjReader::read_object_file(filename,vertex_map,face_list);

    drawing_global = new DrawingGlobal(vertex_map,face_list);
    
    setup_gl(filename,argc,argv);
    draw_faces(face_list,vertex_map);

    glutMainLoop();

    delete drawing_global;
    return 0;
}

void setup_gl(const std::string &filename, int argc, char** argv)
{
    glutInit(&argc,argv);
    glutInitWindowPosition(-1,-1); // leaves window system to decide
    glutInitWindowSize(WINDOW_WIDTH,WINDOW_HEIGHT);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutCreateWindow(filename.c_str());

    glClearColor(0.0f,0.0f,.7f,1.0f);
    // to set units of model to pixels:
    // gluOrtho2D(0,WINDOW_WIDTH,0,WINDOW_HEIGHT);
    // to set units of model to fraction of window (ie, 0 to 1):
    // gluOrtho2D(0,1,0,1);

    // don't want
    // GLfloat max_dim = gl_max((GLfloat) WINDOW_WIDTH, (GLfloat) WINDOW_HEIGHT);
    // GLfloat normalized_window_width = ((GLfloat)WINDOW_WIDTH)/max_dim;
    // GLfloat normalized_window_height = ((GLfloat)WINDOW_HEIGHT)/max_dim;
    // gluOrtho2D(0, normalized_window_width, 0, normalized_window_height);
    // left and right clipping planes, bottom and top clipping planes, near and
    // far clipping planes
    // what part of window we want to render to.
    glViewport(0,0,WINDOW_WIDTH,WINDOW_HEIGHT);

    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(
        20.f, // angle to top from center
        ((GLfloat)WINDOW_WIDTH) / ((GLfloat) WINDOW_HEIGHT), //aspect ratio
        1.f, // dist to near clip plane
        100.f // dist to far clip plane
    );
    
    glutDisplayFunc(render_frame);
}

GLfloat gl_max(GLfloat a, GLfloat b)
{
    if (a < b)
        return b;
    return a;
}

void render_frame(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
    glLoadIdentity();
    drawing_global->render_frame();
    glutSwapBuffers();
    glPopMatrix();
    glFlush();
}


void draw_faces(
    std::vector<Face*>&face_list,
    std::unordered_map<Vertex::VertexID,Vertex*>& vertex_map)
{
    for (std::vector<Face*>::iterator iter = face_list.begin();
         iter != face_list.end(); ++iter)
    {
        (*iter)->draw_face(vertex_map);
    }
}
