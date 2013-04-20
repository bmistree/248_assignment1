#include <iostream>
#include <iomanip>
#include <string>
#include "ObjReader.hpp"
#include "ObjElement.hpp"
#include "VertexNormal.hpp"
#include <vector>
#include <unordered_map>
#include <GL/glut.h>
#include <GL/gl.h>
#include "DrawingGlobal.hpp"
#include "Util.hpp"
#include "Bitmap.h"
#include "ControlPoints.hpp"
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


    int c;
    bool ctrl_pts_flag, obj_filename_flag, bmp_flag;
    ctrl_pts_flag = obj_filename_flag = bmp_flag = false;
    std::string obj_filename, ctrl_pts_filename, bmp_filename;
    
    while( (c=getopt(argc,argv,"f:c:o:")) != -1)
    {
        switch(c)
        {
          case 'f':
            obj_filename_flag = true;
            obj_filename = std::string(optarg);
            break;
          case 'c':
            ctrl_pts_flag = true;
            ctrl_pts_filename = std::string(optarg);
            break;
          case 'o':
            bmp_flag = true;
            bmp_filename = std::string(optarg);
            break;
        }
    }

    // read object file
    Vertex::VertMap* vmap = new Vertex::VertMap;
    VertexNormal::VertNormalMap* vnmap = new VertexNormal::VertNormalMap;
    TextureCoordinate::TextCoordinateMap* tcmap =
        new TextureCoordinate::TextCoordinateMap;
    Face::FaceMap* fmap = new Face::FaceMap;    
    if (!obj_filename_flag)
    {
        std::cout<<"\nError: require .obj filename.\n";
        return 0;
    }
    ObjReader::read_object_file(obj_filename,*vmap,*vnmap,*tcmap,*fmap);

    
    // read bitmap if specified
    Bitmap* bm = NULL;
    if (bmp_flag)
    {
        bm = new Bitmap();
        if (! bm->loadBMP((char*)bmp_filename.c_str()))
        {
            std::cout<<"\nError loading bmp file "<<bmp_filename<<".\n";
            assert(false);
        }
    }

    // read control points file
    if (ctrl_pts_flag)
    {
        ControlVec cv;
        ControlPoints::load_control_points(ctrl_pts_filename,cv);
    }
    
    
    drawing_global = new DrawingGlobal(vmap,vnmap,tcmap,fmap,bm);
    setup_gl(obj_filename,argc,argv,drawing_global);

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
    glShadeModel(GL_FLAT);
    glEnable(GL_DEPTH_TEST);
    
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

