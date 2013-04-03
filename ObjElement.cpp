#include "ObjElement.hpp"
#include <sstream>
#include <GL/gl.h>
#include <stdio.h>

/**
   Returns copy of string with no whitespace at beginning or end of string.
 */
static std::string trim(const std::string& line)
{
    std::size_t index_str_front = line.find_first_not_of(" \t");
    if (index_str_front != std::string::npos)
    {
        std::size_t index_str_end = line.find_last_not_of(" \t");
        return line.substr(index_str_front,index_str_end);
    }    
    return std::string("");
}

/**
   Assumes that trimmed_line is already trimmed.
 */
static bool is_comment(std::string& trimmed_line)
{
    if (trimmed_line.size() > 0)
        return trimmed_line[0] == OBJ_FILE_COMMENT_START;
    return false;
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

ListOfVertices* ListOfVertices::construct_from_line(const std::string& line)
{
    // only need to create the regular expression once

    std::string trimmed_string = trim(line);

    // using 2 here because, we must check at least second index to ensure that
    // not a texture coordinate or normal
    if (trimmed_string.size() < 2)
        return NULL;
    
    if (is_comment(trimmed_string))
        return NULL;
    
    if ((trimmed_string[0] == 'v') &&

        // ensure that not a texture or normal coordinate, both of which also
        // begin with 'v'.  FIXME: this is sort of gross to hard-code.  Maybe
        // figure out a better approach later.
        (trimmed_string[1] != 't') &&
        (trimmed_string[1] != 'n'))
    {
        std::istringstream tokenizer(trimmed_string.substr(1));
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

