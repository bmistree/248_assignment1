#include <iostream>
#include <iomanip>
#include <string>
#include "ObjReader.hpp"
#include "ObjElement.hpp"
#include <vector>

int main(int argc, char** argv)
{
    std::string filename(argv[1]);
    
    std::vector<ObjElement*> all_elements =
        ObjReader::read_object_file(filename);

    for (std::vector<ObjElement*>::const_iterator iter = all_elements.begin();
         iter != all_elements.end(); ++iter)
    {
        (*iter)->pretty_print();
        std::cout<<"\n";
    }
    
        
    return 0;
}
