#include "ObjReader.hpp"
#include <fstream>
#include <iostream>
#include <string>

std::vector<ObjElement*> ObjReader::read_object_file(std::string filename)
{
    std::vector<ObjElement*> all_elements;
    
    std::ifstream file;
    file.open (filename.c_str());
    std::string single_line;
    while (getline(file,single_line))
    {
        ObjElement* line_element =
            ObjReader::read_element_from_string(single_line);

        if (line_element != NULL)
            all_elements.push_back(line_element);
    }
    file.close();
    return all_elements;
}


ObjElement* ObjReader::read_element_from_string(std::string line_to_read)
{
    ObjElement* element = NULL;
    element = Vertex::construct_from_line(line_to_read);
    if (element != NULL)
        return element;

    element = TextureCoordinate::construct_from_line(line_to_read);
    if (element != NULL)
        return element;

    element = Face::construct_from_line(line_to_read);
    if (element != NULL)
        return element;
    
    return element;
}
