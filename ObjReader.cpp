#include "ObjReader.hpp"
#include <fstream>
#include <iostream>
#include <string>



void ObjReader::read_object_file(
    std::string filename,
    std::unordered_map<Vertex::VertexID,Vertex*> & vertex_map,
    std::vector<Face*> & face_list)    
{
    std::vector<ObjElement*> all_elements = read_all_file_elements(filename);
    for (std::vector<ObjElement*>::iterator iter = all_elements.begin();
         iter != all_elements.end(); ++iter)
    {
        Vertex* vert = dynamic_cast<Vertex*> (*iter);
        if (vert != NULL)
        {
            vertex_map[vert->get_vid()] = vert;
            continue;
        }

        Face* face = dynamic_cast<Face*> (*iter);
        if (face != NULL)
        {
            face_list.push_back(face);
            // putting continue in in case add more rules later.
            continue;
        }
    }
}


std::vector<ObjElement*> ObjReader::read_all_file_elements(std::string filename)
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
