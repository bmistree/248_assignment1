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

#include "Subdivider.hpp"

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

// Also responsible for cleanup of all data.
DrawingGlobal* drawing_global = NULL;

void setup_gl(
    const std::string& filename, int argc,char** argv,
    DrawingGlobal* dg);

void render_frame(void);
void keyboard_func(unsigned char key,int x, int y);


int main(int argc, char** argv)
{
    std::string filename(argv[1]);
    Vertex::VertexMap* vmap = new Vertex::VertexMap;
    VertexNormal::VertNormalMap* obj_vnmap = new VertexNormal::VertNormalMap;
    VertexNormal::VertNormalMap* open_vnmap = new VertexNormal::VertNormalMap;
    
    OpenVolumeMesh::GeometricPolyhedralMeshV4f* obj_mesh =
        ObjReader::read_object_file(filename,*obj_vnmap,*open_vnmap,*vmap);
    
    if (open_vnmap->size() == 0)
        VertexNormal::calculate_normals(*open_vnmap,obj_mesh);

    
    
    drawing_global = new DrawingGlobal(obj_mesh,open_vnmap,vmap);
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


    // // hard code the position of the spotlight 5 units above the origin with a
    // // 30 degree cutoff.
    // GLfloat light_pos[4] = {0.f,5.f,0.f,.5f};
    // GLfloat light_dir[3] = {0.f,-1.f,0.f};
    // GLfloat light_ambient[4] = {.3,.3,.3,1.0f};
    // GLfloat diffuse[4] = {1.0f,1.0f,1.0f,1.0f};    
    // glLightfv(GL_LIGHT0,GL_POSITION,light_pos);
    // glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    // glLightf(GL_LIGHT0,GL_SPOT_CUTOFF,45.f);
    // glLightfv(GL_LIGHT0,GL_SPOT_DIRECTION,light_dir);
    // glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    // glLightfv(GL_LIGHT0, GL_SPECULAR, diffuse);

    // // light1 is an ambient light
    // GLfloat ambientlight[ ] = {0.3, 0.3 , 0.3, 1.0};
    // glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientlight);

    // // enable a spotlight
    // glEnable(GL_LIGHTING);
    // glEnable(GL_LIGHT0);
    // glEnable(GL_COLOR_MATERIAL);

    glClearColor(0.0f,0.0f,.7f,1.0f);
    glViewport(0,0,WINDOW_WIDTH,WINDOW_HEIGHT);
    dg->set_window_width_height(WINDOW_WIDTH,WINDOW_HEIGHT);
    
    glutDisplayFunc(render_frame);
    glutKeyboardFunc(keyboard_func);
}

void keyboard_func(unsigned char key,int x, int y)
{
    drawing_global->keyboard_func(key,x,y);
}


void render_frame(void)
{
    drawing_global->render_frame();
}

