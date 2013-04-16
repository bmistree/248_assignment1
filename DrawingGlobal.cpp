#include "DrawingGlobal.hpp"
#include <unordered_map>
#include <vector>
#include "ObjElement.hpp"
#include <GL/gl.h>
#include <GL/glut.h>
#include <iostream>
#include <OpenVolumeMesh/Mesh/PolyhedralMesh.hh>
#include "Subdivider.hpp"


static void b_normalize(OpenVolumeMesh::Geometry::Vec3f& normal)
{
    float len = sqrt(normal[0]*normal[0] + normal[1]*normal[1] + normal[2]*normal[2]);
    float mult = 1./len;
    normal = mult * normal;
}


DrawingGlobal::DrawingGlobal(
    OpenVolumeMesh::GeometricPolyhedralMeshV4f* _obj_mesh,
    TextureCoordinate::TextureCoordinateMap* _tc_map,
    VertexNormal::VertNormalMap* _vnmap, Vertex::VertexMap* _vmap, bitmap_image* _bm)
 : obj_mesh(_obj_mesh), tc_map(_tc_map),vnmap(_vnmap),vmap(_vmap),
   original_obj_mesh(_obj_mesh), original_vnmap(_vnmap),original_vmap(_vmap),
   bm(_bm),shading(GL_FLAT),gl_begin_type(GL_TRIANGLES)
{
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

    angle = 0;

    if (bm != NULL)
    {
        glGenTextures(1,&texture_id);
        glBindTexture(GL_TEXTURE_2D,texture_id);
        glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,
            bm->width, bm->height,0,GL_RGBA,
            GL_UNSIGNED_BYTE,bm->data);
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
        angle += ANGLE_INCREMENT;
        eye_direction_delta.x = sin(angle);
        eye_direction_delta.z = -cos(angle);
    }
    else if (key == 'j')
    {
        angle -= ANGLE_INCREMENT;
        eye_direction_delta.x = sin(angle);
        eye_direction_delta.z = -cos(angle);
    }

    
    else if (key == 'w')
        eye.y += INCREMENT_POS_ON_KEY;
    else if (key == 's')
        eye.y -= INCREMENT_POS_ON_KEY;

    else if (key == '1') // smooth shading
        shading = GL_SMOOTH;
    else if (key == '2')
        shading = GL_FLAT;
    else if (key == '3')
        gl_begin_type = GL_LINE_LOOP;
    else if (key == '4')
        gl_begin_type = GL_TRIANGLES;

    else if (key == '+')
    {
        vnmap = new VertexNormal::VertNormalMap;
        obj_mesh = Subdivider::subdivide(obj_mesh);
        VertexNormal::calculate_normals(*vnmap,obj_mesh);
    }
    else if (key == '0')
    {
        obj_mesh = original_obj_mesh;
        vnmap = original_vnmap;
        vmap = original_vmap;
    }
    
    glutPostRedisplay();    
}

void DrawingGlobal::draw_global_coords()
{
    // hard code the position of the spotlight 5 units above the origin with a
    // 30 degree cutoff.
    //GLfloat light_pos[4] = {0.f,5.f,0.f,.5f};
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

    
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
    glDisable(GL_CULL_FACE);
    

    // draw ground plane before lookat
    glColor3f(.7f,0.f,0.f);
    // float y_pos = -4.f;
    float y_pos = -10.f;    
    float left_side = -20.f;
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

void DrawingGlobal::render_frame()
{
    glDisable(GL_CULL_FACE);
    if (bm != NULL)
        glEnable(GL_TEXTURE_2D);


    glEnable(GL_DEPTH_TEST);
    glShadeModel(shading);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);

    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
    glLoadIdentity();

    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(
        PERSPECTIVE_NEAR_PLANE_ANGLE,  // angle to top from center
        (window_width) / (window_height), //aspect ratio
        .01f, // dist to near clip plane
        100.f // dist to far clip plane
    );

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

        
    gluLookAt(
        // eye positioning
        eye.x,eye.y,eye.z,
        eye.x + eye_direction_delta.x, eye.y + eye_direction_delta.y,
        eye.z + eye_direction_delta.z,
        // camera is oriented so that it's top is in the y direction
        0.f,1.f,0.f);

    glDisable(GL_CULL_FACE);
    draw_global_coords();

    for (OpenVolumeMesh::FaceIter iter = obj_mesh->faces_begin();
         iter != obj_mesh->faces_end(); ++iter)
    {
        glBegin(gl_begin_type);
        
        const OpenVolumeMesh::OpenVolumeMeshFace& face = obj_mesh->face(*iter);
        const std::vector<OpenVolumeMesh::HalfEdgeHandle>& half_edge_handles = face.halfedges();

        // calculate the face normal as average of vertex normal and draw points.
        std::vector<OpenVolumeMesh::Geometry::Vec4f> vertex_points;
        std::vector<OpenVolumeMesh::Geometry::Vec3f> vertex_normals;
        std::vector<std::pair<float,float> > texture_coords;

        OpenVolumeMesh::Geometry::Vec3f vertex_normal(0,0,0);
        for (std::vector<OpenVolumeMesh::HalfEdgeHandle>::const_iterator he_iter = half_edge_handles.begin();
             he_iter != half_edge_handles.end(); ++he_iter)
        {
            const OpenVolumeMesh::OpenVolumeMeshEdge& edge = obj_mesh->halfedge(*he_iter);
            const OpenVolumeMesh::VertexHandle& from_vertex = edge.from_vertex();
            const OpenVolumeMesh::Geometry::Vec4f& vertex_vec = obj_mesh->vertex(from_vertex);

            vertex_points.push_back(vertex_vec);

            if (vnmap->find(from_vertex) == vnmap->end())
                assert(false);

            if (bm != NULL)
            {
                TextureCoordinate* tc = (*tc_map)[from_vertex];
                std::pair<float,float> p (tc->get_u(),tc->get_v());
                texture_coords.push_back(p);
            }
            
            VertexNormal* vn = (*vnmap)[from_vertex];
            vertex_normals.push_back(vn->open_vec3());
            vertex_normal += vn->open_vec3();
        }

        if (shading == GL_FLAT)
        {
            OpenVolumeMesh::Geometry::Vec3f vec0(
                vertex_points[0][0],vertex_points[0][1],vertex_points[0][2]);
            OpenVolumeMesh::Geometry::Vec3f vec1(
                vertex_points[1][0],vertex_points[1][1],vertex_points[1][2]);
            OpenVolumeMesh::Geometry::Vec3f vec2(
                vertex_points[2][0],vertex_points[2][1],vertex_points[2][2]);
            
            OpenVolumeMesh::Geometry::Vec3f dir =(vec1 - vec0) % (vec2 - vec0);
            b_normalize(dir);
            glNormal3f(dir[0],dir[1],dir[2]);
        }        

        // actually draw the points
        uint64_t counter = 0;
        for (std::vector<OpenVolumeMesh::Geometry::Vec4f>::const_iterator citer = vertex_points.begin();
             citer != vertex_points.end(); ++citer)
        {
            if (shading == GL_SMOOTH)
            {
                OpenVolumeMesh::Geometry::Vec3f smooth_normal = vertex_normals[counter];
                b_normalize(smooth_normal);
                glNormal3f(
                    smooth_normal[0],
                    smooth_normal[1],
                    smooth_normal[2]);
            }

            if (counter < texture_coords.size())
            {
                std::pair<float,float> p = texture_coords[counter];
                glTexCoord2f(p.first,p.second);
            }

            glVertex4f(
                (GLfloat)((*citer)[0]),
                (GLfloat)((*citer)[1]),
                (GLfloat)((*citer)[2]),
                (GLfloat)((*citer)[3]));

            ++counter;
        }
        glEnd();
    }


    glutSwapBuffers();
    glPopMatrix();
    glFlush();
    
}

