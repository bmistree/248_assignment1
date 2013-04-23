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
#include "ControlPoint.hpp"
#include "Spline.hpp"
#include <sstream>
#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480



// Also responsible for cleanup of all data.
DrawingGlobal* drawing_global = NULL;

void setup_gl(
    const std::string& filename, int argc,char** argv,
    DrawingGlobal* dg);

void render_frame(void);
void keyboard_func(unsigned char key,int x, int y);
void timer_func(int value);

int main(int argc, char** argv)
{
    int c;
    bool ctrl_pts_flag, obj_filename_flag, bmp_flag;
    ctrl_pts_flag = obj_filename_flag = bmp_flag = false;
    float max_spline_time = DEFAULT_MAX_SPLINE_TIME;
    std::string obj_filename, ctrl_pts_filename, bmp_filename;
    
    while( (c=getopt(argc,argv,"f:c:o:t:")) != -1)
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
          case 't':
            std::istringstream tokenizer(optarg);
            tokenizer >> max_spline_time;
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
    Spline* spline = NULL;
    if (ctrl_pts_flag)
        spline = new Spline(ctrl_pts_filename, max_spline_time);
    
    drawing_global = new DrawingGlobal(vmap,vnmap,tcmap,fmap,bm,spline);
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

    glutTimerFunc(CALLBACK_TIME_MS,timer_func,0);
    glutDisplayFunc(render_frame);
    glutKeyboardFunc(keyboard_func);
}

void timer_func(int value)
{
    drawing_global->timer_func();
    glutTimerFunc(CALLBACK_TIME_MS,timer_func,value);
}

void keyboard_func(unsigned char key,int x, int y)
{
    drawing_global->keyboard_func(key,x,y);
}


void render_frame(void)
{
    drawing_global->render_frame();
}

