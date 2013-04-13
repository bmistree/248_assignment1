#include "ObjElement.hpp"
#include <sstream>
#include <GL/gl.h>
#include <GL/glut.h>
#include <stdio.h>
#include <iostream>
#include <cassert>
#include <boost/regex.hpp>


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
{
    static TextureCoordID _tid = 0;
    tid = ++_tid;
}

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
Face::Face()
{}
void Face::add_vertex_descriptor(VertexDescriptor* vd)
{
    vert_descriptors.push_back(vd);
}
Face::~Face()
{
    for (std::vector<VertexDescriptor*>::iterator iter = vert_descriptors.begin();
         iter != vert_descriptors.end(); ++iter)
    {
        delete *iter;
    }
    vert_descriptors.clear();
}

void Face::draw_face(const std::unordered_map<Vertex::VertexID,Vertex*>& vert_map) const
{
    glColor3f(.5f,.5f,.5f);
    glBegin(GL_LINE_LOOP);

    for (std::vector<VertexDescriptor*>::const_iterator citer = vert_descriptors.begin();
         citer != vert_descriptors.end(); ++citer)
    {
        Vertex::VertexID* vid = (*citer)->vid;
        std::unordered_map<Vertex::VertexID,Vertex*>::const_iterator vert_iter = vert_map.find(*vid);
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
    for (std::vector<VertexDescriptor*>::const_iterator citer = vert_descriptors.begin();
         citer != vert_descriptors.end(); ++citer)
    {
        Vertex::VertexID* vid = (*citer)->vid;
        std::cout<<"   vert: "<<*vid;
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
        Face* f = new Face();
        
        // start from 1 to remove command character, 'f'.
        std::string to_tokenize = line.substr(1);
        std::istringstream tokenizer(to_tokenize);
        if (to_tokenize.find("/") == std::string::npos)
        {
            while (! tokenizer.eof())
            {
                Vertex::VertexID* vid = new Vertex::VertexID;
                tokenizer >> (*vid);
                f->add_vertex_descriptor(new VertexDescriptor(vid,NULL,NULL));
            }
        }
        else
        {
            // Face has / format for each grouping:
            // a/b/c  d/e/f g/h/i ...
            boost::regex regex("(\\d+)/(\\d+)/(\\d+)");
            std::string::const_iterator to_search_from, end;
            to_search_from = to_tokenize.begin();
            end = to_tokenize.end();
            boost::match_results<std::string::const_iterator> matches;

            while(boost::regex_search(to_search_from,end,matches,regex))
            {
                Vertex::VertexID* vid = new Vertex::VertexID;
                *vid = atoi(
                    std::string(matches[1].first,matches[1].second).c_str());

                TextureCoordinate::TextureCoordID* tcid = new TextureCoordinate::TextureCoordID;
                *tcid = atoi(
                    std::string(matches[2].first,matches[2].second).c_str());

                VertexNormal::VertexNormalID* vnid = new VertexNormal::VertexNormalID;
                *vnid = atoi(
                    std::string(matches[3].first,matches[3].second).c_str());

                VertexDescriptor* vd = new VertexDescriptor(vid,tcid,vnid);
                f->add_vertex_descriptor(vd);

                to_search_from = matches[3].second;
                
            }
        }

        return f;
    }
    return NULL;
}

void Face::centroid_and_maxes(
    Point4& centroid,
    Point4& max,Point4& min,
    const std::unordered_map<Vertex::VertexID,Vertex*>& vert_map) const
{
    centroid.x = centroid.y = centroid.z = 0;
    
    for (std::vector<VertexDescriptor*>::const_iterator citer = vert_descriptors.begin();
         citer != vert_descriptors.end(); ++citer)
    {
        // note: the following line of code is only acceptable because we *know*
        // that the vertex must be in the map.
        Vertex::VertexID vid = *((*citer)->vid);
        Vertex* vert = vert_map.find(vid)->second;
        GLfloat vx,vy,vz;
        vx = vert->get_x();
        vy = vert->get_y();
        vz = vert->get_z();
        centroid.x += vx;
        centroid.y += vy;
        centroid.z += vz;

        if (citer == vert_descriptors.begin())
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

    centroid.x /= ((GLfloat) vert_descriptors.size());
    centroid.y /= ((GLfloat) vert_descriptors.size());
    centroid.z /= ((GLfloat) vert_descriptors.size());
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

