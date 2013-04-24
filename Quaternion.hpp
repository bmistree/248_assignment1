#ifndef __QUATERNION_HPP__
#define __QUATERNION_HPP__

#include <cassert>

class Quaternion
{
public:
    Quaternion(float x,float y, float z, float w)
     :_x(x),_y(y),_z(z),_w(w)
    {
        float x_squared = _x*_x;
        float y_squared = _y*_y;
        float z_squared = _z*_z;
        float w_squared = _w*_w;


        if (sqrt(x_squared + y_squared + z_squared + w_squared) > 1.01)
            assert(false);

        if (sqrt(x_squared + y_squared + z_squared + w_squared) < .99)
            assert(false);

        
        // first row
        mult_data [0] = 1 - 2*y_squared - 2*z_squared;
        mult_data [4] = 2*_x*_y - 2*_w*_z;
        mult_data [8] = 2*_x*_z + 2*_w*_y;
        mult_data [12] = 0;

        // second row
        mult_data[1] = 2*_x*_y + 2*_w*_z;
        mult_data[5] = 1 - 2*x_squared - 2*z_squared;
        mult_data[9] = 2*_y*_z - 2*_w*_x;
        mult_data[13] = 0;

        // third row
        mult_data[2] = 2*_x*_z - 2*_w*_y;
        mult_data[6] = 2*_w*_x + 2*_y*_z;
        mult_data[10] = 1 - 2*x_squared - 2*y_squared;
        mult_data[14] = 0;
        
        // fourth row
        mult_data[3] = 0;
        mult_data[7] = 0;
        mult_data[11] = 0;
        mult_data[15] = 1;
        
        // // first row
        // mult_data [0] = 1 - 2*y_squared - 2*z_squared;
        // mult_data [1] = 2*_x*_y - 2*_w*_z;
        // mult_data [2] = 2*_x*_z + 2*_w*_y;
        // mult_data [3] = 0;

        // // second row
        // mult_data[4] = 2*_x*_y + 2*_w*_z;
        // mult_data[5] = 1 - 2*x_squared - 2*z_squared;
        // mult_data[6] = 2*_y*_z - 2*_w*_x;
        // mult_data[7] = 0;

        // // third row
        // mult_data[8] = 2*_x*_z - 2*_w*_y;
        // mult_data[9] = 2*_w*_x + 2*_y*_z;
        // mult_data[10] = 1 - 2*x_squared - 2*y_squared;
        // mult_data[11] = 0;
        
        // // fourth row
        // mult_data[12] = 0;
        // mult_data[13] = 0;
        // mult_data[14] = 0;
        // mult_data[15] = 1;
    }
    Quaternion(){}
    ~Quaternion(){}


    const float* gl_mult_matrix() const
    {
        return mult_data;
    }
    
    // dot product
    float operator* (const Quaternion& q) const
    {
        return q.x()*_x + q.y()*_y + q.z()*_z + q.w()*_w;
    }

    // cross product
    Quaternion operator% (const Quaternion& q) const
    {
        return Quaternion(
            _w*q.x() + _x*q.w() + _y*q.z() - _z*q.y(),
            _w*q.y() - _x*q.z() + _y*q.w() + _z*q.x(),
            _w*q.z() + _x*q.y() - _y*q.x() + _z*q.w(),
            _w*q.w() - _x*q.x() - _y*q.y() - _z*q.z());
    }

    Quaternion operator% (const Point3& p) const
    {
        return Quaternion(
            _w*p.x + _y*p.z - _z*p.y,
            _w*p.y - _x*p.z + _z*p.x,
            _w*p.z + _x*p.y - _y*p.x,
            - _x*p.x - _y*p.y - _z*p.z);
    }

    bool operator==(const Quaternion& q) const
    {
        return ((q.x() == _x) && (q.y() == _y) &&
            (q.z() == _z) && (q.w() == _w));
    }
    
    Quaternion operator- () const
    {
        return Quaternion(-_x,-_y,-_z,-_w);
    }
    
    static const Quaternion& identity()
    {
        static Quaternion identity (0,0,0,1);
        return identity;
    }

    void normalize ()
    {
        float len = sqrt(_x*_x + _y*_y + _z*_z + _w*_w);

        _x /= len;
        _y /= len;
        _z /= len;
        _w /= len;
    }
    
    Quaternion conjugate() const
    {
        return Quaternion(-_x,-_y,-_z,_w);
    }
    

    float x() const
    {
        return _x;
    }
    float y() const
    {
        return _y;
    }
    float z() const
    {
        return _z;
    }
    float w() const
    {
        return _w;
    }
    
    
    
private:
    float _x,_y,_z,_w;
    float mult_data[16];
};

#endif
