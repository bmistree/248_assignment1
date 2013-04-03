#ifndef __OBJ_READER_HPP__
#define __OBJ_READER_HPP__

#include "ObjElement.hpp"
#include <string>
#include <vector>

class ObjReader
{
public:
    /**
       @param {std::string} filename --- Reads the .obj file stored by filename
       and returns all elements it contains as a vector.
     */
    static std::vector<ObjElement*> read_object_file(std::string filename);

private:

    /**
       @param{std::string} line --- Line from which to make ObjElement
       
       @returns {ObjElement*} --- NULL if the line contains no rule (eg, blank,
       malformed, or comment).
     */
    static ObjElement* read_element_from_string(std::string line);
};

#endif
