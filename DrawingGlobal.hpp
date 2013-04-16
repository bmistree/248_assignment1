#ifndef __DRAWING_GLOBAL_HPP__
#define __DRAWING_GLOBAL_HPP__

#include <unordered_map>
#include <vector>
#include "ObjElement.hpp"
#include "Util.hpp"
#include <OpenVolumeMesh/Mesh/PolyhedralMesh.hh>
#include "Bitmap.h"

#define PERSPECTIVE_NEAR_PLANE_ANGLE 60.f
#define INITIAL_EYE_X 0.f
#define INITIAL_EYE_Y 0.f
#define INITIAL_EYE_Z 0.f

#define INCREMENT_POS_ON_KEY .1f
#define ANGLE_INCREMENT .05f

//Responsible for deleting all resources as well as drawing them
class DrawingGlobal
{
public:
    DrawingGlobal(
        OpenVolumeMesh::GeometricPolyhedralMeshV4f* obj_mesh,
        TextureCoordinate::TextureCoordinateMap* tc_map,
        VertexNormal::VertNormalMap* vnmap,
        Vertex::VertexMap* vmap, Bitmap* bm);
    ~DrawingGlobal();

    void set_window_width_height(GLfloat window_width,GLfloat window_height);

    static OpenVolumeMesh::GeometricPolyhedralMeshV4f* subdivide(
        OpenVolumeMesh::GeometricPolyhedralMeshV4f* to_subdivide);
    
    void render_frame();
    void keyboard_func(unsigned char key,int x, int y);
    void idle_func();
    
private:
    OpenVolumeMesh::GeometricPolyhedralMeshV4f* obj_mesh;
    TextureCoordinate::TextureCoordinateMap* tc_map;
    VertexNormal::VertNormalMap* vnmap;
    Vertex::VertexMap* vmap;

    void draw_global_coords();
    
    OpenVolumeMesh::GeometricPolyhedralMeshV4f* original_obj_mesh;
    TextureCoordinate::TextureCoordinateMap* oringal_tc_map;
    VertexNormal::VertNormalMap* original_vnmap;
    Vertex::VertexMap* original_vmap;

    void initialize();
    
    Point4 eye;
    Point4 eye_direction_delta;
    float angle;
    GLfloat window_width,window_height;

    Bitmap* bm;
    GLuint texture_id;
    

    uint64_t shading;
    uint64_t gl_begin_type;

    GLfloat diffuse [4];
    GLfloat specular[4];
    GLfloat shininess[1];
    GLfloat ambient[4];

    bool initialized;
        
};

#endif
