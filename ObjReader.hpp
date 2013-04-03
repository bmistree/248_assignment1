#ifndef __OBJ_READER_HPP__
#define __OBJ_READER_HPP__

#include "ObjElement.hpp"
#include <string>
#include <vector>
#include <unordered_map>

class ObjReader
{
public:
    static void read_object_file(
        std::string filename,
        std::unordered_map<Vertex::VertexID,Vertex*> & vertex_map,
        std::vector<Face*> & face_list);

private:

    /**
       @param{std::string} line --- Line from which to make ObjElement
       
       @returns {ObjElement*} --- NULL if the line contains no rule (eg, blank,
       malformed, or comment).
     */
    static ObjElement* read_element_from_string(std::string line);

    /**
       @param {std::string} filename --- Reads the .obj file stored by filename
       and returns all elements it contains as a vector.
     */
    static std::vector<ObjElement*> read_all_file_elements(std::string filename);

    
};

#endif
