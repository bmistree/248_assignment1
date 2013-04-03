#include "ObjElement.hpp"
#include <sstream>
#include <GL/gl.h>
#include <stdio.h>

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


ListOfVertices::ListOfVertices(GLfloat _x, GLfloat _y, GLfloat _z, GLfloat _w)
 : x(_x),
   y(_y),
   z(_z),
   w(_w)
{}


ListOfVertices::~ListOfVertices()
{}

void ListOfVertices::pretty_print() const
{
    printf("List of vertices: x=%f y=%f z=%f w=%f",x,y,z,w);
}

ListOfVertices* ListOfVertices::construct_from_line(std::string line)
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

        
        return new ListOfVertices(
            (GLfloat)x, (GLfloat)y, (GLfloat)z, (GLfloat)w);
    }
    
    return NULL;
}

