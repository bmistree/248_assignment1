#ifndef __OBJ_ELEMENT_HPP__
#define __OBJ_ELEMENT_HPP__

#include "Util.hpp"
#include <string>
#include <GL/gl.h>
#include <vector>
#include <unordered_map>

#define OBJ_FILE_COMMENT_START '#'

class Face;

class Vertex
{
public:
    typedef uint64_t VertId;
    typedef std::unordered_map<VertId, Vertex*> VertMap;
    typedef VertMap::iterator VertMapIter;
    typedef VertMap::const_iterator VertMapCIter;
    static bool construct_from_line(VertMap& vmap,std::string line);
    
    VertId vid() const
    {
        return _vid;
    }

    const Point4& pt() const
    {
        return _pt;
    }

    void add_face(Face* f) 
    {
        _face_vec.push_back(f);
    }
    
private:
    Vertex(VertMap& vmap,float x, float y, float z, float w=1.0);
    ~Vertex();
    VertId _vid;
    Point4 _pt;
    std::vector<Face*> _face_vec;
    
    static VertId _next_vid()
    {
        static VertId vid = 0;
        return ++vid;
    }
    
};

class TextureCoordinate 
{
public:
    typedef uint64_t TextCoordId;
    typedef std::unordered_map<TextCoordId,TextureCoordinate*> TextCoordinateMap;
    typedef TextCoordinateMap::iterator TextCoordinateMapIter;
    typedef TextCoordinateMap::const_iterator TextCoordinateMapCIter;
    
    ~TextureCoordinate();
    static bool construct_from_line(TextCoordinateMap& tc_map,std::string line);
    
    TextCoordId tcid() const
    {
        return _tcid;
    }
    const Point3&  pt() const
    {
        return _pt;
    }

private:
    TextureCoordinate(TextCoordinateMap& tc_map,GLfloat u, GLfloat v, GLfloat w=0.0);
    TextCoordId _tcid;
    Point3 _pt;

    static TextCoordId _next_tcid()
    {
        static TextCoordId tcid = 0;
        return ++tcid;
    }
};

class VertexNormal 
{
public:
    typedef uint64_t VertNormalId;
    typedef std::unordered_map<VertNormalId,VertexNormal*> VertNormalMap;
    typedef VertNormalMap::iterator VertNormalMapIter;
    typedef VertNormalMap::const_iterator VertNormalMapCIter;
    
    ~VertexNormal();
    static bool construct_from_line(VertNormalMap& vnmap,std::string line);

    VertNormalId vnid() const
    {
        return _vnid;
    }
    
    const Point3& pt() const
    {
        return _pt;
    }
    
     
private:
    VertexNormal(VertNormalMap& vnmap,const GLfloat& x, const GLfloat& y, const GLfloat& z);
    VertNormalId _vnid;    
    Point3 _pt;

    static VertNormalId _next_vnid()
    {
        static VertNormalId vnid = 0;
        return ++vnid;
    }
};


struct FaceVertexData
{
    FaceVertexData(Vertex* _vert, VertexNormal* _vert_normal,
        TextureCoordinate* _tc_coord)
     : vert(_vert), vert_normal(_vert_normal),tc_coord(_tc_coord)
    {}
    
    Vertex* vert;
    
    // may be NULL
    VertexNormal* vert_normal;
    TextureCoordinate* tc_coord;
};
typedef std::vector<FaceVertexData*> FaceVertDataVec;
typedef FaceVertDataVec::iterator FaceVertDataVecIter;
typedef FaceVertDataVec::const_iterator FaceVertDataVecCIter;


class Face 
{
public:
    typedef uint64_t FaceId;
    typedef std::unordered_map<FaceId,Face*> FaceMap;
    typedef FaceMap::iterator FaceMapIter;
    typedef FaceMap::const_iterator FaceMapCIter;
    
    static bool construct_from_line(
        Vertex::VertMap& vmap,TextureCoordinate::TextCoordinateMap& tcmap,
        VertexNormal::VertNormalMap& vnmap,Face::FaceMap& fmap, std::string line);

    ~Face();

    FaceId fid() const
    {
        return _fid;
    }

    FaceVertDataVecCIter vert_iter_begin() const
    {
        return _vert_data_vec.begin();
    }

    FaceVertDataVecCIter vert_iter_end() const
    {
        return _vert_data_vec.end();
    }
    
    
private:
    Face(FaceMap& fmap, const FaceVertDataVec& fvdv);
    FaceId _fid;
    FaceVertDataVec _vert_data_vec;

    static FaceId _next_fid()
    {
        static FaceId fid = 0;
        return ++fid;
    }
    
};

#endif

