#ifndef __SPLINE_HPP__
#define __SPLINE_HPP__

#include "Util.hpp"
#include "ControlPoint.hpp"
#include <unordered_map>

class Spline
{
public:

    Spline(const std::string& filename, float max_time);
    ~Spline();

    void generate_spline_path(std::vector<Point3*>& sp_path);
    
    void get_pos(float at_time,Point3& pt,Quaternion& quat);
    const ControlVec& control_points()
    {
        return cv;
    }
    
private:
    static const Matrix4& point_matrix();
    void generate_new_coefficients(uint64_t control_point_index);
    
    void slerp(
        float normalized_time, const Quaternion& q1,
        const Quaternion& q2, Quaternion& result);

    
    ControlVec cv;
    float time_slice_width;
    float max_time;

    typedef std::unordered_map<uint64_t,Point4*> TimeCoeffMap;
    typedef TimeCoeffMap::const_iterator TimeCoeffMapCIter;
    typedef TimeCoeffMap::iterator TimeCoeffMapIter;

    // Each control point has an associated coefficient Point4* vector.  To get
    // the 
    TimeCoeffMap time_to_coefficients_x;
    TimeCoeffMap time_to_coefficients_y;
    TimeCoeffMap time_to_coefficients_z;
};

#endif
