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
#include "Bitmap.h"
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
    Vertex::VertMap* vmap = new Vertex::VertMap;
    VertexNormal::VertNormalMap* vnmap = new VertexNormal::VertNormalMap;
    TextureCoordinate::TextCoordinateMap* tcmap =
        new TextureCoordinate::TextCoordinateMap;
    Face::FaceMap* fmap = new Face::FaceMap;
    
    Bitmap* bm = NULL;
    if (argc > 2)
    {
        bm = new Bitmap();
        if (! bm->loadBMP(argv[2]))
        {
            std::cout<<"\n\n"<<argv[2]<<"\n\n";
            assert(false);
        }
    }

    ObjReader::read_object_file(filename,*vmap,*vnmap,*tcmap,*fmap);

    drawing_global = new DrawingGlobal(vmap,vnmap,tcmap,fmap,bm);
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

