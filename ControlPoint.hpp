#ifndef __CONTROL_POINTS_HPP__
#define __CONTROL_POINTS_HPP__

#include "Util.hpp"
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include "Quaternion.hpp"

class ControlPoint;
typedef std::vector<ControlPoint*> ControlVec;
typedef ControlVec::iterator ControlVecIter;
typedef ControlVec::const_iterator ControlVecCIter;


class ControlPoint
{
public:
    ControlPoint(const Point3& _pt, const Quaternion& _quat)
     : pt(_pt),
       quat(_quat)
    {}
    ~ControlPoint(){}

    Point3 pt;
    Quaternion quat;
    
    
    static void load_control_points(const std::string& filename, ControlVec& cv)
    {
        std::ifstream file;
        file.open (filename.c_str());
        std::string single_line;
        
        Point3 pt;
        Quaternion quat;
        
        while (getline(file,single_line))
        {
            if (parse_control_point_line(single_line,pt,quat))
                cv.push_back(new ControlPoint(pt,quat));
        }

        file.close();
    }
    
private:
    static bool parse_control_point_line(
        std::string& line,Point3& pos, Quaternion& quat)
    {
        trim(line);
        if (line.size() == 0)
            return false;

        std::istringstream tokenizer(line);
        float pt_x,pt_y,pt_z;
        tokenizer >> pt_x;
        tokenizer >> pt_y;
        tokenizer >> pt_z;

        pos = Point3(pt_x,pt_y,pt_z);
        
        float quat_x,quat_y,quat_z,quat_w;
        tokenizer >> quat_x;
        tokenizer >> quat_y;
        tokenizer >> quat_z;
        tokenizer >> quat_w;
        quat = Quaternion(quat_x,quat_y,quat_z,quat_w);
        return true;
    }
    
};

#endif
