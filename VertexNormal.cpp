#include "VertexNormal.hpp"
#include "Util.hpp"
#include <sstream>
#include <GL/gl.h>
#include <GL/glut.h>

/************* Vertex Normal **********/
VertexNormal::VertexNormal(
    VertNormalMap& vnmap, const GLfloat &x, const GLfloat& y, const GLfloat&z)
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

