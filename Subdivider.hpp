#ifndef __SUBDIVIDER_HPP__
#define __SUBDIVIDER_HPP__

#include <unordered_map>
#include <vector>
#include "Util.hpp"
#include <OpenVolumeMesh/Mesh/PolyhedralMesh.hh>


class VertexHandleHasher{
public:
    size_t operator() (const OpenVolumeMesh::VertexHandle& vh) const
    {
        return hasher(vh);
    }

    static size_t hasher (const OpenVolumeMesh::VertexHandle& vh)
    {
        return vh.idx();
    }
    
};

class MidpointHasher
{
public:
    size_t operator() (
        const std::pair<OpenVolumeMesh::VertexHandle,OpenVolumeMesh::VertexHandle>& p) const
    {
        return VertexHandleHasher::hasher(p.first) * VertexHandleHasher::hasher(p.second);
    }
};


typedef std::unordered_map<
    OpenVolumeMesh::VertexHandle,OpenVolumeMesh::VertexHandle,
    VertexHandleHasher> VertHandleMap;

typedef std::unordered_map<
    std::pair<OpenVolumeMesh::VertexHandle,OpenVolumeMesh::VertexHandle>,
    OpenVolumeMesh::VertexHandle, MidpointHasher> MidpointMap;

typedef std::unordered_map<OpenVolumeMesh::VertexHandle,bool,
    VertexHandleHasher> EvenMap;

typedef std::unordered_map<
    OpenVolumeMesh::VertexHandle,OpenVolumeMesh::Geometry::Vec4f,
    VertexHandleHasher> VecMap;



class Subdivider
{
public:
    static OpenVolumeMesh::GeometricPolyhedralMeshV4f* subdivide(
        OpenVolumeMesh::GeometricPolyhedralMeshV4f* to_subdivide);

private:

    static void subdivide_face(
        const OpenVolumeMesh::OpenVolumeMeshFace& face,
        OpenVolumeMesh::GeometricPolyhedralMeshV4f* to_subdivide,
        OpenVolumeMesh::GeometricPolyhedralMeshV4f* subdivided_mesh,
        VertHandleMap& translation_map,
        MidpointMap& midpoint_map,
        EvenMap& subdivided_is_even_map,
        VecMap& subdivided_original_position_map);

    
    static void create_new_faces(
        OpenVolumeMesh::GeometricPolyhedralMeshV4f* subdivided_mesh,
        OpenVolumeMesh::VertexHandle bottom_left, OpenVolumeMesh::VertexHandle bottom_right,
        OpenVolumeMesh::VertexHandle top, OpenVolumeMesh::VertexHandle midpoint_top_bottom_left,
        OpenVolumeMesh::VertexHandle midpoint_top_bottom_right,
        OpenVolumeMesh::VertexHandle midpoint_bottom_left_bottom_right,
        
        EvenMap& subdivided_is_even_map,
        VecMap& subdivided_original_position_map);

    static OpenVolumeMesh::VertexHandle find_or_insert_even_vertex(
        OpenVolumeMesh::GeometricPolyhedralMeshV4f* to_subdivide,
        OpenVolumeMesh::GeometricPolyhedralMeshV4f* subdivided_mesh,

        VertHandleMap& translation_map,
        
        OpenVolumeMesh::VertexHandle& to_subdivide_v_handle,
        EvenMap& subdivided_is_even_map,
        VecMap& subdivided_original_position_map);

        
    static OpenVolumeMesh::VertexHandle find_or_insert_midpoint(
        OpenVolumeMesh::GeometricPolyhedralMeshV4f* to_subdivide,
        OpenVolumeMesh::GeometricPolyhedralMeshV4f* subdivided_mesh,

        MidpointMap& midpoint_map,

        const OpenVolumeMesh::VertexHandle& v_handle_a,
        const OpenVolumeMesh::VertexHandle& v_handle_b,

        EvenMap& subdivided_is_even_map,
        VecMap& subdivided_original_position_map);

    static void adjust_subdivided_vertex_positions(
        OpenVolumeMesh::GeometricPolyhedralMeshV4f* subdivided_mesh,
        const EvenMap& subdivided_is_even_map,
        const VecMap& subdivided_original_position_map);

    static void handle_even_vertex(
        OpenVolumeMesh::GeometricPolyhedralMeshV4f* subdivided_mesh,
        const OpenVolumeMesh::VertexHandle& vhandle,
        const VecMap& subdivided_original_position_map);
    
    
};
#endif

