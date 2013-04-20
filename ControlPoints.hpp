#ifndef __CONTROL_POINTS_HPP__
#define __CONTROL_POINTS_HPP__

#include "Util.hpp"
#include <string>
#include <vector>
#include <fstream>

typedef std::vector<Point3*> ControlVec;
typedef ControlVec::iterator ControlVecIter;


class ControlPoints
{
public:
    static void load_control_points(const std::string& filename, ControlVec& cv)
    {
        std::ifstream file;
        file.open (filename.c_str());
        std::string single_line;
        while (getline(file,single_line))
        {
            Point3* pt = parse_control_point_line(single_line);
            if (pt != NULL)
                cv.push_back(pt);
        }

        file.close();
    }
    
private:
    static Point3* parse_control_point_line(std::string& line)
    {
        trim(line);
        if (line.size() == 0)
            return NULL;
        
        std::istringstream tokenizer(line);
        float x,y,z;
        tokenizer >> x;
        tokenizer >> y;
        tokenizer >> z;

        return new Point3(x,y,z);
    }
    
};

#endif
