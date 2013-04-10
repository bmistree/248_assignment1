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
#include "Util.hpp"
#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

// Also responsible for cleanup of all data.
DrawingGlobal* drawing_global = NULL;

void setup_gl(
    const std::string& filename, int argc,char** argv,
    DrawingGlobal* dg);


void draw_faces(
    std::vector<Face*>&face_list,
    std::unordered_map<Vertex::VertexID,Vertex*>& vertex_map,
    VertexNormal::VertNormalMap& vertex_normal_map);

void render_frame(void);
GLfloat gl_max(GLfloat a, GLfloat b);
void keyboard_func(unsigned char key,int x, int y);


int main(int argc, char** argv)
{
    std::string filename(argv[1]);
    
    std::vector<Face*> face_list;
    std::unordered_map<Vertex::VertexID,Vertex*> vertex_map;
    VertexNormal::VertNormalMap vertex_normal_map;
    ObjReader::read_object_file(
        filename,vertex_map,face_list,vertex_normal_map);

    drawing_global = new DrawingGlobal(vertex_map,face_list);
    
    setup_gl(filename,argc,argv,drawing_global);

    glutMainLoop();
    delete drawing_global;
    return 0;
}

void setup_gl(const std::string &filename, int argc, char** argv,DrawingGlobal* dg)
{
    glutInit(&argc,argv);
    glutInitWindowPosition(-1,-1); // leaves window system to decide
    glutInitWindowSize(WINDOW_WIDTH,WINDOW_HEIGHT);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutCreateWindow(filename.c_str());
    
    glClearColor(0.0f,0.0f,.7f,1.0f);
    glViewport(0,0,WINDOW_WIDTH,WINDOW_HEIGHT);
    dg->set_window_width_height(WINDOW_WIDTH,WINDOW_HEIGHT);
    
    glutDisplayFunc(render_frame);
    glutKeyboardFunc(keyboard_func);
}

GLfloat gl_max(GLfloat a, GLfloat b)
{
    if (a < b)
        return b;
    return a;
}

void keyboard_func(
    unsigned char key,int x, int y)
{
    drawing_global->keyboard_func(key,x,y);
}


void render_frame(void)
{
    drawing_global->render_frame();
}


void draw_faces(
    std::vector<Face*>&face_list,
    std::unordered_map<Vertex::VertexID,Vertex*>& vertex_map,
    VertexNormal::VertNormalMap& vertex_normal_map)
{
    for (std::vector<Face*>::iterator iter = face_list.begin();
         iter != face_list.end(); ++iter)
    {
        (*iter)->draw_face(vertex_map);
    }
}
