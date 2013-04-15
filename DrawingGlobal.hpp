#ifndef __DRAWING_GLOBAL_HPP__
#define __DRAWING_GLOBAL_HPP__

#include <unordered_map>
#include <vector>
#include "ObjElement.hpp"
#include "Util.hpp"
#include <OpenVolumeMesh/Mesh/PolyhedralMesh.hh>

#define PERSPECTIVE_NEAR_PLANE_ANGLE 60.f
#define INITIAL_EYE_X 0.f
#define INITIAL_EYE_Y 0.f
// #define INITIAL_EYE_Z 7.5f
#define INITIAL_EYE_Z 0.f

#define INCREMENT_POS_ON_KEY .1f


//Responsible for deleting all resources as well as drawing them
class DrawingGlobal
{
public:
    DrawingGlobal(
        OpenVolumeMesh::GeometricPolyhedralMeshV4f* obj_mesh,
        VertexNormal::VertNormalMap* vnmap,
        Vertex::VertexMap* vmap);
    ~DrawingGlobal();

    void set_window_width_height(GLfloat window_width,GLfloat window_height);

    static OpenVolumeMesh::GeometricPolyhedralMeshV4f* subdivide(
        OpenVolumeMesh::GeometricPolyhedralMeshV4f* to_subdivide);
    
    void render_frame();
    void keyboard_func(unsigned char key,int x, int y);
    void idle_func();
    
private:
    OpenVolumeMesh::GeometricPolyhedralMeshV4f* obj_mesh;
    VertexNormal::VertNormalMap* vnmap;
    Vertex::VertexMap* vmap;
    Point4 eye;
    GLfloat window_width,window_height;


    uint64_t shading;

    
    
};

#endif
