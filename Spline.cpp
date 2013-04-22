#include "Spline.hpp"
#include "Util.hpp"
#include "ControlPoints.hpp"

Spline::Spline(const std::string& filename, float _max_time)
 : max_time(_max_time)
{
    ControlPoints::load_control_points(filename, cv);
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

Point3* Spline::get_pos(float at_time)
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

    float time_slice_width = (((float)cv.size())/max_time);
    uint64_t num_full_slices = (at_time/time_slice_width);
    float remainder_time = at_time - (num_full_slices*time_slice_width);
    float normalized_time = remainder_time / time_slice_width;

    // used for calculating positions based on time
    float squared_normalized_time = normalized_time*normalized_time;
    float cubed_normalized_time = squared_normalized_time* normalized_time;

    Point3* pt = new Point3 (
        coeff_x->x + coeff_x->y*normalized_time + coeff_x->z*squared_normalized_time +
        coeff_x->w* cubed_normalized_time,

        coeff_y->x + coeff_y->y*normalized_time + coeff_y->z*squared_normalized_time +
        coeff_y->w* cubed_normalized_time,

        coeff_z->x + coeff_z->y*normalized_time + coeff_z->z*squared_normalized_time +
        coeff_z->w* cubed_normalized_time);

    return pt;
}


// populates the time_to_coefficients matrices for each control point
void Spline::generate_new_coefficients(uint64_t control_point_index)
{
    Point3* prev_point = cv[0];
    if (control_point_index != 0)
        prev_point = cv[control_point_index -1];

    Point3* current_point = cv[control_point_index];

    Point3* next_point = cv[control_point_index];
    if ( (control_point_index +1) < cv.size())
        next_point = cv[control_point_index + 1];

    Point3* next_next_point = cv[control_point_index];
    if ( (control_point_index + 2) < cv.size())
        next_next_point = cv[control_point_index + 2];
    
    Point4 xs (
        prev_point->x,current_point->x,next_point->x,next_next_point->x);

    Point4 ys(
        prev_point->y,current_point->y,next_point->y,next_next_point->y);

    Point4 zs(
        prev_point->z,current_point->z,next_point->z,next_next_point->z);

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
