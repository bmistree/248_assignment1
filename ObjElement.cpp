#include "ObjElement.hpp"
#include <sstream>
#include <GL/gl.h>
#include <GL/glut.h>
#include <stdio.h>
#include <iostream>
#include <cassert>

/**
   Returns copy of string with no whitespace at beginning or end of string.
 */
static void trim(std::string& line)
{
    std::size_t index_str_front = line.find_first_not_of(" \t");
    if (index_str_front != std::string::npos)
    {
        std::size_t index_str_end = line.find_last_not_of(" \t");
        line.replace(index_str_end + 1,line.size(),"");
        line.replace(0,index_str_front,"");
    }
}

/**
   Comment begins at '#' and ends at end of line.  Removes everything in the
   comment from begin to end of line.
 */
static void remove_comment(std::string& line)
{
    std::size_t index_comment_begin = line.find_first_of(OBJ_FILE_COMMENT_START);
    if (index_comment_begin != std::string::npos)
        line.replace(index_comment_begin,line.size(),"");
}


Vertex::Vertex(GLfloat _x, GLfloat _y, GLfloat _z, GLfloat _w)
 : vert_pt(_x,_y,_z,_w)
{
    static VertexID _vid = 0;
    _vid += 1;
    vid = _vid;
}


Vertex::~Vertex()
{}

void Vertex::pretty_print() const
{
    std::cout<<"List of texture coordinates: id="<<
        vid<<" x="<<vert_pt.x<<" y="<<vert_pt.y<<
        " z="<<vert_pt.z<<" w="<<vert_pt.w;
}

Vertex* Vertex::construct_from_line(std::string line)
{
    remove_comment(line);
    trim(line);

    // using 2 here because, we must check at least second index to ensure that
    // not a texture coordinate or normal
    if (line.size() < 2)
        return NULL;
    
    if ((line[0] == 'v') &&

        // ensure that not a texture or normal coordinate, both of which also
        // begin with 'v'.  FIXME: this is sort of gross to hard-code.  Maybe
        // figure out a better approach later.
        (line[1] != 't') &&
        (line[1] != 'n'))
    {
        // start from 1 to remove command character, 'v'.
        std::string to_tokenize = line.substr(1);
        std::istringstream tokenizer(to_tokenize);
        float x,y,z,w;
        w = 1.0;
        tokenizer >> x;
        tokenizer >> y;
        tokenizer >> z;
        if (! tokenizer.eof())
            tokenizer >> w;

        
        return new Vertex(
            (GLfloat)x, (GLfloat)y, (GLfloat)z, (GLfloat)w);
    }
    
    return NULL;
}



TextureCoordinate::TextureCoordinate(GLfloat _u, GLfloat _v, GLfloat _w)
 : u(_u),
   v(_v),
   w(_w)
{}
TextureCoordinate::~TextureCoordinate()
{}
void TextureCoordinate::pretty_print() const
{
    printf("List of vertices: u=%f v=%f w=%f",u,v,w);
}

TextureCoordinate* TextureCoordinate::construct_from_line(std::string line)
{
    remove_comment(line);
    trim(line);

    // using 2 here because, we must check at least second index to ensure that
    // not a texture coordinate or normal
    if (line.size() < 2)
        return NULL;
    
    if ((line[0] == 'v') && (line[1] == 't'))
    {
        // start from 1 to remove command characters, 'vt'.
        std::string to_tokenize = line.substr(2);
        std::istringstream tokenizer(to_tokenize);
        float u,v,w;
        w = 0.0;
        tokenizer >> u;
        tokenizer >> v;
        if (! tokenizer.eof())
            tokenizer >> w;

        return new TextureCoordinate(
            (GLfloat)u, (GLfloat)v, (GLfloat)w);
    }
    return NULL;
}

/************* FACE *************/
Face::Face(const std::vector<Vertex::VertexID>& vert_index_vec)
 :vert_ids(vert_index_vec)
{}

Face::~Face()
{}

void Face::draw_face(const std::unordered_map<Vertex::VertexID,Vertex*>& vert_map) const
{
    glColor3f(.5f,.5f,.5f);
    glBegin(GL_LINE_LOOP);
    for (std::vector<Vertex::VertexID>::const_iterator citer = vert_ids.begin();
         citer != vert_ids.end(); ++citer)
    {
        Vertex::VertexID vid = *citer;
        std::unordered_map<Vertex::VertexID,Vertex*>::const_iterator vert_iter = vert_map.find(vid);
        if (vert_iter == vert_map.end())
            assert(false);
        
        Vertex* vert = vert_iter->second;
        // actually draw
        glVertex4f(vert->get_x(),vert->get_y(),vert->get_z(), vert->get_w());
    }
    glEnd();
}

void Face::pretty_print() const
{
    printf("List of face vertices:");
    for (std::vector<Vertex::VertexID>::const_iterator citer = vert_ids.begin();
         citer != vert_ids.end(); ++citer)
    {
        std::cout<<"   vert: "<<*citer;
    }
}

Face* Face::construct_from_line(std::string line)
{
    remove_comment(line);
    trim(line);

    // using 2 here because, we must check at least second index to ensure that
    // not a texture coordinate or normal
    if (line.size() < 1)
        return NULL;
    
    if (line[0] == 'f')
    {
        // start from 1 to remove command character, 'f'.
        std::string to_tokenize = line.substr(1);
        std::istringstream tokenizer(to_tokenize);
        // FIXME: may want to pass as pointer instead to avoid copy in
        // constructor.
        std::vector<Vertex::VertexID> vert_ids;

        if (to_tokenize.find("/") == std::string::npos)
        {
            while (! tokenizer.eof())
            {
                Vertex::VertexID vid;
                tokenizer >> vid;
                vert_ids.push_back(vid);
            }
        }
        // FIXME: handle comlex face with /-s.  

        return new Face(vert_ids);
    }
    return NULL;
}

void Face::centroid_and_maxes(
    Point4& centroid,
    Point4& max,Point4& min,
    const std::unordered_map<Vertex::VertexID,Vertex*>& vert_map) const
{
    centroid.x = centroid.y = centroid.z = 0;
    
    for (std::vector<Vertex::VertexID>::const_iterator citer = vert_ids.begin();
         citer != vert_ids.end(); ++citer)
    {
        // note: the following line of code is only acceptable because we *know*
        // that the vertex must be in the map.
        Vertex* vert = vert_map.find(*citer)->second;
        GLfloat vx,vy,vz;
        vx = vert->get_x();
        vy = vert->get_y();
        vz = vert->get_z();
        centroid.x += vx;
        centroid.y += vy;
        centroid.z += vz;

        if (citer == vert_ids.begin())
        {
            max.x = vx;
            max.y = vy;
            max.z = vz;
            min.x = vx;
            min.y = vy;
            min.z = vz;
        }

        if (max.x < vx) max.x = vx;
        if (max.y < vy) max.y = vy;
        if (max.z < vz) max.z = vz;
        
        if (min.x > vx) min.x = vx;
        if (min.y > vy) min.y = vy;
        if (min.z > vz) min.z = vz;
    }

    centroid.x /= ((GLfloat) vert_ids.size());
    centroid.y /= ((GLfloat) vert_ids.size());
    centroid.z /= ((GLfloat) vert_ids.size());
}

/************* Vertex Normal **********/

VertexNormal::~VertexNormal()
{}

VertexNormal::VertexNormal(const GLfloat &x, const GLfloat& y, const GLfloat&z)
 : vn(x,y,z)
{
    static VertexNormalID _vnid = 0;
    _vnid += 1;
}

void VertexNormal::pretty_print() const
{
    printf("Vertex normals: x=%f y=%f z=%f",vn.x,vn.y,vn.z);
}


VertexNormal* VertexNormal::construct_from_line(std::string line)
{
    remove_comment(line);
    trim(line);

    // using 2 here because, we must check at least second index to ensure that
    // not a texture coordinate or normal
    if (line.size() < 1)
        return NULL;

    if ((line[0] == 'v') && (line[1] == 'n'))
    {
        // start from 1 to remove command characters, 'vn'.
        std::string to_tokenize = line.substr(2);
        std::istringstream tokenizer(to_tokenize);
        float x,y,z;
        tokenizer >> x;
        tokenizer >> y;
        tokenizer >> z;        

        return new VertexNormal((GLfloat)x, (GLfloat)y, (GLfloat)z);
    }
    return NULL;
}

