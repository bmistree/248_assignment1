#include "Spline.hpp"
#include "Util.hpp"
#include "ControlPoint.hpp"
#include <iostream>

#define SPLINE_NUM_SEGMENTS 1000.

Spline::Spline(const std::string& filename, float _max_time)
 : max_time(_max_time)
{
    ControlPoint::load_control_points(filename, cv);
    time_slice_width = max_time/(float(cv.size()));
}

const Matrix4& Spline::point_matrix()
{
    static Matrix4 point_matrix(
        Point4(0,1,0,0),
        Point4(-.5, 0, .5, 0),
        Point4(1, -2.5, 2, -.5),
        Point4(-.5, 1.5, -1.5, .5));
    return point_matrix;
}

void Spline::generate_spline_path(std::vector<Point3*>& sp_path)
{
    Quaternion quat;
    
    for (float i = 0; i < SPLINE_NUM_SEGMENTS; ++i)
    {
        Point3* pt = new Point3;
        get_pos(max_time* (i/SPLINE_NUM_SEGMENTS), *pt,quat);
        sp_path.push_back(pt);
    }
}

Spline::~Spline()
{
    for (TimeCoeffMapIter iter = time_to_coefficients_x.begin();
         iter != time_to_coefficients_x.end(); ++iter)
    {
        delete iter->second;
    }
    time_to_coefficients_x.clear();

    for (TimeCoeffMapIter iter = time_to_coefficients_y.begin();
         iter != time_to_coefficients_y.end(); ++iter)
    {
        delete iter->second;
    }
    time_to_coefficients_y.clear();

    for (TimeCoeffMapIter iter = time_to_coefficients_z.begin();
         iter != time_to_coefficients_z.end(); ++iter)
    {
        delete iter->second;
    }
    time_to_coefficients_z.clear();
}

void Spline::get_pos(float at_time,Point3& pt,Quaternion& quat)
{
    // if requested a time past the maximum coefficient vector, then just use
    // the last time in the coefficient vector.
    uint64_t control_point_index = cv.size() - 1;
    if (at_time < max_time) 
        control_point_index = uint64_t( (at_time/max_time) * cv.size());

    TimeCoeffMapCIter finder = time_to_coefficients_x.find(control_point_index);
    if (finder == time_to_coefficients_x.end())
        generate_new_coefficients(control_point_index);

    Point4* coeff_x = time_to_coefficients_x[control_point_index];
    Point4* coeff_y = time_to_coefficients_y[control_point_index];
    Point4* coeff_z = time_to_coefficients_z[control_point_index];
    
    // from coefficient, calculate point.
    float remainder_time = at_time - (control_point_index*time_slice_width);
    float normalized_time = remainder_time / time_slice_width;
    
    if (at_time >= max_time)
        normalized_time = 1.0;

    // used for calculating positions based on time
    float squared_normalized_time = normalized_time*normalized_time;
    float cubed_normalized_time = squared_normalized_time* normalized_time;

    pt.x = coeff_x->x + coeff_x->y*normalized_time + coeff_x->z*squared_normalized_time +
        coeff_x->w* cubed_normalized_time;
    
    pt.y = coeff_y->x + coeff_y->y*normalized_time + coeff_y->z*squared_normalized_time +
        coeff_y->w* cubed_normalized_time;

    pt.z = coeff_z->x + coeff_z->y*normalized_time + coeff_z->z*squared_normalized_time +
        coeff_z->w* cubed_normalized_time;


    // load quaternion
    uint64_t next_control_point_index = control_point_index;
    if (control_point_index + 1 < cv.size())
        next_control_point_index = control_point_index + 1;

    if (cv[control_point_index]->quat == cv[next_control_point_index]->quat)
        quat = cv[control_point_index]->quat;
    else
    {
        slerp(
            normalized_time,cv[control_point_index]->quat,
            cv[next_control_point_index]->quat,quat);
    }
}

void Spline::slerp(
    float normalized_time, const Quaternion& q1, const Quaternion& q2, Quaternion& result)
{
    float alpha = acos(q1 * q2);
    float coeff1 = sin( (1 - normalized_time)*alpha)/sin(alpha);
    float coeff2 = sin( normalized_time*alpha)/sin(alpha);

    result = Quaternion(
        coeff1*q1.x() + coeff2*q2.x(),
        coeff1*q1.y() + coeff2*q2.y(),
        coeff1*q1.z() + coeff2*q2.z(),
        coeff1*q1.w() + coeff2*q2.w());
    result.normalize();
}



// populates the time_to_coefficients matrices for each control point
void Spline::generate_new_coefficients(uint64_t control_point_index)
{
    Point3 prev_point = cv[0]->pt;
    if (control_point_index != 0)
        prev_point = cv[control_point_index -1]->pt;

    Point3 current_point = cv[control_point_index]->pt;

    Point3 next_point = cv[control_point_index]->pt;
    if ( (control_point_index +1) < cv.size())
        next_point = cv[control_point_index + 1]->pt;

    Point3 next_next_point = cv[control_point_index]->pt;
    if ( (control_point_index + 2) < cv.size())
        next_next_point = cv[control_point_index + 2]->pt;
    
    Point4 xs (
        prev_point.x,current_point.x,next_point.x,next_next_point.x);

    Point4 ys(
        prev_point.y,current_point.y,next_point.y,next_next_point.y);

    Point4 zs(
        prev_point.z,current_point.z,next_point.z,next_next_point.z);

    Point4* xcoeff = new Point4;
    *xcoeff = point_matrix()* xs;
    
    Point4* ycoeff = new Point4;
    *ycoeff = point_matrix()* ys;

    Point4* zcoeff = new Point4;
    *zcoeff = point_matrix()* zs;

    time_to_coefficients_x[control_point_index] = xcoeff;
    time_to_coefficients_y[control_point_index] = ycoeff;
    time_to_coefficients_z[control_point_index] = zcoeff;
}
