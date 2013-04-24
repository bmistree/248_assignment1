#include "DrawingGlobal.hpp"
#include <unordered_map>
#include <vector>
#include "ObjElement.hpp"
#include <GL/gl.h>
#include <GL/glut.h>
#include <iostream>
#include <math.h>

DrawingGlobal::DrawingGlobal(
    Vertex::VertMap* _vmap, VertexNormal::VertNormalMap* _vnmap,
    TextureCoordinate::TextCoordinateMap* _tcmap, Face::FaceMap* _fmap,
    Bitmap* _bm, Spline* _spline)
 : vmap(_vmap),
   vnmap(_vnmap),
   tcmap(_tcmap),
   fmap(_fmap),
   bm(_bm),
   spline(_spline),
   ticks_since_start(0),
   draw_ctrl_pts(false)
{
    gl_begin_type = GL_TRIANGLES;
    shading = GL_FLAT;
    
    diffuse[0] = 1.0;
    diffuse[1] = 1.0;
    diffuse[2] = 1.0;
    diffuse[3] = 1.0;

    specular[0] = 1.0;
    specular[1] = 1.0;
    specular[2] = 1.0;
    specular[3] = 1.0;

    shininess[0] = 10.0f;

    ambient[0] = .1;
    ambient[1] = .1;
    ambient[2] = .1;
    ambient[3] = 1.0;

    eye.x = INITIAL_EYE_X;
    eye.y = INITIAL_EYE_Y;
    eye.z = INITIAL_EYE_Z;

    eye_direction_delta.x = 0;
    eye_direction_delta.y = 0;
    eye_direction_delta.z = - 1.0f;

    eye_angle = 0;
    initialized = false;
}

void DrawingGlobal::initialize()
{
    if (initialized)
        return;

    initialized = true;
    
    glPushMatrix();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(
        PERSPECTIVE_NEAR_PLANE_ANGLE,  // angle to top from center
        (window_width) / (window_height), //aspect ratio
        .01f, // dist to near clip plane
        100.f // dist to far clip plane
    );
    glPopMatrix();

    // load textures
    if (bm != NULL)
    {        
        glGenTextures(1, &texture_id);
        glBindTexture(GL_TEXTURE_2D, texture_id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, 
            GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 
            GL_NEAREST);
        
        glTexImage2D(GL_TEXTURE_2D, 0, 3, bm->width, bm->height, 0,
            GL_RGB, GL_UNSIGNED_BYTE, bm->data);
    }
}

void DrawingGlobal::set_window_width_height(
    GLfloat _window_width,GLfloat _window_height)
{
    window_width = _window_width;
    window_height = _window_height;
}

DrawingGlobal::~DrawingGlobal()
{}

void DrawingGlobal::keyboard_func(unsigned char key,int x, int y)
{
    // Based on tutorial here:
    //http://www.lighthouse3d.com/tutorials/glut-tutorial/keyboard-example-moving-around-the-world/
    if (key == 'i')
    {
        eye.z += eye_direction_delta.z *INCREMENT_POS_ON_KEY;
        eye.x += eye_direction_delta.x *INCREMENT_POS_ON_KEY;
    }
    else if (key == 'k')
    {
        eye.z -= eye_direction_delta.z *INCREMENT_POS_ON_KEY;
        eye.x -= eye_direction_delta.x *INCREMENT_POS_ON_KEY;
    }
    else if (key == 'l')
    {
        eye_angle += ANGLE_INCREMENT;
        eye_direction_delta.x = sin(eye_angle);
        eye_direction_delta.z = -cos(eye_angle);
    }
    else if (key == 'j')
    {
        eye_angle -= ANGLE_INCREMENT;
        eye_direction_delta.x = sin(eye_angle);
        eye_direction_delta.z = -cos(eye_angle);
    }
    else if (key == 'w')
        eye.y += INCREMENT_POS_ON_KEY;
    else if (key == 's')
        eye.y -= INCREMENT_POS_ON_KEY;

    else if (key == '1') // smooth shading
        shading = GL_SMOOTH;
    else if (key == '2')
        shading = GL_FLAT;
    else if (key == 'c')
        draw_ctrl_pts = !draw_ctrl_pts;
    else if (key == '3')
        gl_begin_type = GL_LINE_LOOP;
    else if (key == '4')
        gl_begin_type = GL_TRIANGLES;
    else if (key == ',')
        shininess[0] += 10;
    else if (key == '.')
        shininess[0] -= 10;
    else if (key == 'g')
    {
        diffuse[0] += .1;
        diffuse[1] += .1;
        diffuse[2] += .1;
    }
    else if (key == 'h')
    {
        diffuse[0] -= .1;
        diffuse[1] -= .1;
        diffuse[2] -= .1;
    }
    else if (key == 'v')
    {
        specular[0] += .1;
        specular[1] += .1;
        specular[2] += .1;
    }
    else if (key == 'b')
    {
        specular[0] -= .1;
        specular[1] -= .1;
        specular[2] -= .1;
    }
    else if (key == ' ')
        ticks_since_start = 0;
    else if (key == '+')
    {
        // avg_vnmap = new VertexNormal::VertNormalMap;
        // obj_mesh = Subdivider::subdivide(obj_mesh);
        // VertexNormal::calculate_average_normals(*avg_vnmap,obj_mesh);
    }
    else if (key == '0')
    {
        // vnmap = original_vnmap;
        // vmap = original_vmap;
        // avg_vnmap = original_avg_vnmap;
    }

    glutPostRedisplay();    
}



void DrawingGlobal::draw_global_coords()
{
    // hard code the position of the spotlight 5 units above the origin with a
    // 30 degree cutoff.
    GLfloat light_pos[4] = {0.f,30.f,0.f,.5f};
    GLfloat light_dir[3] = {0.f,-1.f,0.f};
    GLfloat light_ambient[4] = {.3,.3,.3,1.0f};
    GLfloat diffuse[4] = {1.0f,1.0f,1.0f,1.0f};    
    glLightfv(GL_LIGHT0,GL_POSITION,light_pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightf(GL_LIGHT0,GL_SPOT_CUTOFF,45.f);
    glLightfv(GL_LIGHT0,GL_SPOT_DIRECTION,light_dir);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, diffuse);

    // light1 is an ambient light
    GLfloat ambientlight[ ] = {0.3, 0.3 , 0.3, 1.0};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientlight);

    // enable a spotlight
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT);
    glEnable(GL_COLOR_MATERIAL);


    // draw ground plane before lookat
    glColor3f(.7f,0.f,0.f);
    float y_pos = -20.f;    
    float left_side = -80.f;
    int num_tessels = 200;
    float dim_tessel = fabs(left_side)*2./num_tessels;
    for (int i = 0; i < num_tessels; ++i)
    {
        for (int j = 0; j < num_tessels; ++j)
        {
            glBegin(GL_QUADS);
            glNormal3f(0,.707,-.707);

            glVertex3f(
                left_side + i* dim_tessel,
                y_pos,
                left_side + j*dim_tessel);

            glVertex3f(
                left_side + i*dim_tessel,
                y_pos,
                left_side + (j+1)*dim_tessel);

            glVertex3f(
                left_side + (i+1)*dim_tessel,
                y_pos,
                left_side + (j+1)*dim_tessel);

            glVertex3f(
                left_side + (i+1)*dim_tessel,
                y_pos,
                left_side + j*dim_tessel);

            glEnd();
        }
    }
}


void DrawingGlobal::timer_func()
{
    ++ticks_since_start;
    render_frame();
}


void DrawingGlobal::render_frame()
{
    initialize();
    glEnable(GL_DEPTH_TEST);
    glShadeModel(shading);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS,shininess);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    gluLookAt(
        // eye positioning
        eye.x,eye.y,eye.z,
        eye.x + eye_direction_delta.x, eye.y + eye_direction_delta.y,
        eye.z + eye_direction_delta.z,
        // camera is oriented so that it's top is in the y direction
        0.f,1.f,0.f);

    // not moving lights + ground plane to follow interpolation itinerary
    draw_global_coords();
    
    if (spline != NULL)
    {
        // draw control points if we're supposed to
        if (draw_ctrl_pts)
        {
            const ControlVec& cv = spline->control_points();
            for (ControlVecCIter citer = cv.begin(); citer != cv.end();
                 ++citer)
            {
                // Point3* control_point = *citer;
                draw_control_point(*citer);
            }
        }

        // translate model view before drawing
        Point3 translate_point;
        Quaternion rot_quat;
        spline->get_pos(
            ticks_since_start*CALLBACK_TIME_MS/1000.,translate_point,rot_quat);

        glTranslatef(
            translate_point.x,
            translate_point.y,
            translate_point.z);

        glMultMatrixf(rot_quat.gl_mult_matrix());
        
    }

    if (bm != NULL)
    {
        glEnable(GL_TEXTURE_2D);
        glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
        glBindTexture(GL_TEXTURE_2D, texture_id);
    }

    for (Face::FaceMapCIter face_citer = fmap->begin();
         face_citer != fmap->end(); ++face_citer)
    {
        Face* f = face_citer->second;
        glBegin(gl_begin_type);

        if (shading == GL_FLAT)
        {
            VertexNormal* fnormal = f->face_normal();
            const Point3& norm_pt = fnormal->pt();
            glNormal3f(norm_pt.x,norm_pt.y,norm_pt.z);
        }
        
        for (FaceVertDataVecCIter vert_data_iter = f->vert_iter_begin();
             vert_data_iter != f->vert_iter_end(); ++vert_data_iter)
        {
            Vertex* vert = (*vert_data_iter)->vert;
            if (shading == GL_SMOOTH)
            {
                VertexNormal* vert_normal = vert->avg_normal(*vnmap);
                const Point3& norm_pt = vert_normal->pt();
                glNormal3f(norm_pt.x,norm_pt.y,norm_pt.z);
            }
            
            Vertex* single_vert = (*vert_data_iter)->vert;
            const Point3& vert_pt = single_vert->pt();
            glVertex3f(vert_pt.x,vert_pt.y,vert_pt.z);
        }
        glEnd();
    }

    
    glPopMatrix(); // matches push from glulookat
    glutSwapBuffers();
    glFlush();
}



void DrawingGlobal::draw_control_point(ControlPoint* cp)
{
    glPushMatrix();
    glTranslatef(cp->pt.x,cp->pt.y,cp->pt.z);
    glutSolidTeapot(.5);
    glPopMatrix();
}
