#include "ObjElement.hpp"
#include <sstream>
#include <GL/gl.h>
#include <GL/glut.h>
#include <stdio.h>
#include <iostream>
#include <cassert>
#include <boost/regex.hpp>
#include <math.h>

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

Vertex::Vertex(VertMap& vmap, float x, float y, float z,float w)
 : _vid (_next_vid()),
   _pt(x,y,z,w)
{
    vmap[_vid] = this;
}

bool Vertex::construct_from_line(VertMap& vmap,std::string line)
{
    remove_comment(line);
    trim(line);

    // using 2 here because, we must check at least second index to ensure that
    // not a texture coordinate or normal
    if (line.size() < 2)
        return false;
    
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

        new Vertex(vmap,x,y,z,w);
        return true;
    }
    return false;
}



TextureCoordinate::TextureCoordinate(
    TextCoordinateMap& tcmap,GLfloat _u, GLfloat _v, GLfloat _w)
 : _tcid( _next_tcid()),
   _pt(_u,_v,_w)
{
    tcmap[_tcid] = this;
}

TextureCoordinate::~TextureCoordinate()
{}

bool TextureCoordinate::construct_from_line(
    TextureCoordinate::TextCoordinateMap& tcmap,std::string line)
{
    remove_comment(line);
    trim(line);

    // using 2 here because, we must check at least second index to ensure that
    // not a texture coordinate or normal
    if (line.size() < 2)
        return false;
    
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

        new TextureCoordinate(tcmap,(GLfloat)u, (GLfloat)v, (GLfloat)w);
        return true;
    }
    return false;
}


/************* Vertex Normal **********/
VertexNormal::VertexNormal(VertNormalMap& vnmap, const GLfloat &x, const GLfloat& y, const GLfloat&z)
 : _vnid(_next_vnid()),
   _pt(x,y,z)
{
    vnmap[_vnid] = this;
}

VertexNormal::~VertexNormal()
{}


bool VertexNormal::construct_from_line(VertNormalMap& vnmap,std::string line)
{
    remove_comment(line);
    trim(line);

    // using 2 here because, we must check at least second index to ensure that
    // not a texture coordinate or normal
    if (line.size() < 2)
        return false;

    if ((line[0] == 'v') && (line[1] == 'n'))
    {
        // start from 1 to remove command characters, 'vn'.
        std::string to_tokenize = line.substr(2);
        std::istringstream tokenizer(to_tokenize);
        float x,y,z;
        tokenizer >> x;
        tokenizer >> y;
        tokenizer >> z;        

        new VertexNormal(vnmap,(GLfloat)x, (GLfloat)y, (GLfloat)z);
        return true;
    }
    return false;
}




/************* FACE *************/
Face::~Face()
{
    for (FaceVertDataVecIter fvdv_iter = _vert_data_vec.begin();
         fvdv_iter != _vert_data_vec.end(); ++fvdv_iter)
    {
        delete *fvdv_iter;
    }
}

Face::Face(FaceMap& fmap, const FaceVertDataVec& fvdv)
 : _fid(_next_fid()),
   _vert_data_vec(fvdv)
{
    fmap[_fid] = this;

    for( FaceVertDataVecIter fvdv_iter = _vert_data_vec.begin();
         fvdv_iter != _vert_data_vec.end(); ++fvdv_iter)
    {
        (*fvdv_iter) -> vert -> add_face(this);
    }
}


bool Face::construct_from_line(
    Vertex::VertMap& vmap,TextureCoordinate::TextCoordinateMap& tcmap,
    VertexNormal::VertNormalMap& vnmap,Face::FaceMap& fmap, std::string line)
{
    remove_comment(line);
    trim(line);

    // using 2 here because, we must check at least second index to ensure that
    // not a texture coordinate or normal
    if (line.size() < 1)
        return false;

    FaceVertDataVec face_vert_data_vec;

    if (line[0] == 'f')
    {
        // start from 1 to remove command character, 'f'.
        std::string to_tokenize = line.substr(1);
        std::istringstream tokenizer(to_tokenize);
        if (to_tokenize.find("/") == std::string::npos)
        {
            while (! tokenizer.eof())
            {
                Vertex::VertId vid;
                tokenizer >> vid;

                FaceVertexData* fvd = new FaceVertexData(
                    vmap[vid], NULL,NULL);
                
                face_vert_data_vec.push_back(fvd);
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
                Vertex::VertId vid;
                TextureCoordinate::TextCoordId tcid;
                VertexNormal::VertNormalId vnid;
                vid = atoi(
                    std::string(matches[1].first,matches[1].second).c_str());

                tcid = atoi(
                    std::string(matches[2].first,matches[2].second).c_str());

                vnid = atoi(
                    std::string(matches[3].first,matches[3].second).c_str());

                FaceVertexData* fvd = new FaceVertexData(
                    vmap[vid],vnmap[vnid],tcmap[tcid]);
                face_vert_data_vec.push_back(fvd);

                to_search_from = matches[3].second;
            }
        }
        new Face(fmap,face_vert_data_vec);
        return true;
    }
    return false;
}


