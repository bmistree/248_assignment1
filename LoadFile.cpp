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
    TextureCoordinate::TextureCoordinateMap* obj_tc_map =
        new TextureCoordinate::TextureCoordinateMap;
    TextureCoordinate::TextureCoordinateMap* open_tc_map =
        new TextureCoordinate::TextureCoordinateMap;
    
    
    OpenVolumeMesh::GeometricPolyhedralMeshV4f* obj_mesh =
        ObjReader::read_object_file(
            filename,*obj_tc_map,*open_tc_map,*obj_vnmap,*open_vnmap,*vmap);
    
    if (open_vnmap->size() == 0)
        VertexNormal::calculate_normals(*open_vnmap,obj_mesh);
    
    
    drawing_global = new DrawingGlobal(obj_mesh,open_tc_map,open_vnmap,vmap);
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

    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glClearColor(0.0f,0.0f,.7f,1.0f);
    glViewport(0,0,WINDOW_WIDTH,WINDOW_HEIGHT);
    dg->set_window_width_height(WINDOW_WIDTH,WINDOW_HEIGHT);

    glDisable(GL_CULL_FACE);
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

