#ifndef __DRAWING_GLOBAL_HPP__
#define __DRAWING_GLOBAL_HPP__

#include <unordered_map>
#include <vector>
#include "ObjElement.hpp"
#include "Util.hpp"
#include "Bitmap.h"
#include "VertexNormal.hpp"
#include "Spline.hpp"

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
        Vertex::VertMap* vmap, VertexNormal::VertNormalMap* vnmap,
        TextureCoordinate::TextCoordinateMap* tcmap, Face::FaceMap* fmap,
        Bitmap* bm, Spline* spline);

    ~DrawingGlobal();

    void set_window_width_height(GLfloat window_width,GLfloat window_height);

    void timer_func();
    void render_frame();
    void keyboard_func(unsigned char key,int x, int y);
    void idle_func();
    
private:

    // Info about mesh
    Vertex::VertMap* vmap;
    VertexNormal::VertNormalMap* vnmap;
    TextureCoordinate::TextCoordinateMap* tcmap;
    Face::FaceMap* fmap;
    Bitmap* bm;
    Spline* spline;
    uint64_t ticks_since_start;
    GLuint texture_id;

    // info about light, shading and eye
    uint64_t shading;
    uint64_t gl_begin_type;
    
    Point3 eye, eye_direction_delta;
    float eye_angle;
    GLfloat window_width,window_height;

    GLfloat diffuse [4];
    GLfloat specular[4];
    GLfloat shininess[1];
    GLfloat ambient[4];

    bool initialized;
    bool draw_ctrl_pts;
    bool draw_spline_path;
    std::vector<Point3*> spline_path;
    
    void draw_control_point(ControlPoint* cp);
    void draw_global_coords();
    void initialize();
    void gl_draw_spline_path();
};

#endif
