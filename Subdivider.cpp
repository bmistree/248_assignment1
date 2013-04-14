#include "Subdivider.hpp"
#include <OpenVolumeMesh/Mesh/PolyhedralMesh.hh>

OpenVolumeMesh::GeometricPolyhedralMeshV4f* Subdivider::subdivide(
    OpenVolumeMesh::GeometricPolyhedralMeshV4f* to_subdivide)
{
    OpenVolumeMesh::GeometricPolyhedralMeshV4f* subdivided_mesh =
        new OpenVolumeMesh::GeometricPolyhedralMeshV4f();

    OpenVolumeMesh::GeometricPolyhedralMeshV4f other;
    
    // maps from handles in to_subdivide to handles in subdivided
    VertHandleMap translation_map;

    // maps to midpoints...keys are each edges vertex handles.  value is the
    // vertex handle for the midpoint vertex between each vertex.  (Note: to
    // ensure that have not created midpoint yet between a,b, should search
    // the map for a,b as well as b,a.)
    MidpointMap midpoint_map;

    EvenMap subdivided_is_even_map;
    VecMap subdivided_original_position_map;

    for (OpenVolumeMesh::FaceIter iter = to_subdivide->faces_begin();
         iter != to_subdivide->faces_end(); ++iter)
    {
        const OpenVolumeMesh::OpenVolumeMeshFace& face = to_subdivide->face(*iter);
        subdivide_face(face,to_subdivide,subdivided_mesh,translation_map,midpoint_map,
            subdivided_is_even_map,subdivided_original_position_map);
    }
    return subdivided_mesh;
}


void Subdivider::subdivide_face(
    const OpenVolumeMesh::OpenVolumeMeshFace& face,
    OpenVolumeMesh::GeometricPolyhedralMeshV4f* to_subdivide,
    OpenVolumeMesh::GeometricPolyhedralMeshV4f* subdivided_mesh,

    VertHandleMap& translation_map,

    MidpointMap& midpoint_map,
    EvenMap& subdivided_is_even_map,
    VecMap& subdivided_original_position_map)
{
    const std::vector<OpenVolumeMesh::HalfEdgeHandle>& half_edge_handles = face.halfedges();

    OpenVolumeMesh::VertexHandle
        to_subdivide_top, to_subdivide_bottom_left, to_subdivide_bottom_right;

    OpenVolumeMesh::VertexHandle
        subdivided_top_vertex_handle, subdivided_bottom_left_vertex_handle,
        subdivided_bottom_right_vertex_handle;

    OpenVolumeMesh::VertexHandle
        subdivided_midpoint_top_bottom_left, subdivided_midpoint_top_bottom_right,
        subdivided_midpoint_bottom_left_bottom_right;
    

    // grab old vertices from to_subdivide mesh.  NOTE: assuming triangles.
    int counter = 0;
    for (std::vector<OpenVolumeMesh::HalfEdgeHandle>::const_iterator he_iter = half_edge_handles.begin();
         he_iter != half_edge_handles.end(); ++he_iter)
    {
        const OpenVolumeMesh::OpenVolumeMeshEdge& edge = to_subdivide->halfedge(*he_iter);
        const OpenVolumeMesh::VertexHandle& from_vertex = edge.from_vertex();

        if (counter == 0)
            to_subdivide_bottom_left = from_vertex;
        else if (counter == 1)
            to_subdivide_top = from_vertex;
        else
            to_subdivide_bottom_right = from_vertex;

        ++counter;
    }

    // search for top, bottom left, bottom right vertices in subdivided
    // mesh.  If they do not exist, create them.
    
    // maps from handles in to_subdivide to handles in subdivided
    subdivided_bottom_left_vertex_handle = find_or_insert_even_vertex(
        to_subdivide,subdivided_mesh,translation_map,to_subdivide_bottom_left,
        subdivided_is_even_map,subdivided_original_position_map);
    subdivided_bottom_right_vertex_handle = find_or_insert_even_vertex(
        to_subdivide,subdivided_mesh,translation_map,to_subdivide_bottom_right,
        subdivided_is_even_map,subdivided_original_position_map);
    subdivided_top_vertex_handle = find_or_insert_even_vertex(
        to_subdivide,subdivided_mesh,translation_map,to_subdivide_top,
        subdivided_is_even_map,subdivided_original_position_map);

    
    // midpoint bottom-left to top
    subdivided_midpoint_top_bottom_left =
        find_or_insert_midpoint(to_subdivide,subdivided_mesh,midpoint_map,
            to_subdivide_top,to_subdivide_bottom_left,
            subdivided_is_even_map,subdivided_original_position_map);

    subdivided_midpoint_top_bottom_right =
        find_or_insert_midpoint(to_subdivide,subdivided_mesh,midpoint_map,
            to_subdivide_top,to_subdivide_bottom_right,
            subdivided_is_even_map,subdivided_original_position_map);

    subdivided_midpoint_bottom_left_bottom_right =
        find_or_insert_midpoint(to_subdivide,subdivided_mesh,midpoint_map,
            to_subdivide_bottom_left,to_subdivide_bottom_right,
            subdivided_is_even_map,subdivided_original_position_map);


    create_new_faces(
        subdivided_mesh,
        subdivided_bottom_left_vertex_handle,subdivided_bottom_right_vertex_handle,
        subdivided_top_vertex_handle,subdivided_midpoint_top_bottom_left,
        subdivided_midpoint_top_bottom_right, subdivided_midpoint_bottom_left_bottom_right,
        subdivided_is_even_map,subdivided_original_position_map);
}

void Subdivider::create_new_faces(
    OpenVolumeMesh::GeometricPolyhedralMeshV4f* subdivided_mesh,
    OpenVolumeMesh::VertexHandle bottom_left, OpenVolumeMesh::VertexHandle bottom_right,
    OpenVolumeMesh::VertexHandle top, OpenVolumeMesh::VertexHandle midpoint_top_bottom_left,
    OpenVolumeMesh::VertexHandle midpoint_top_bottom_right,
    OpenVolumeMesh::VertexHandle midpoint_bottom_left_bottom_right,

    EvenMap& subdivided_is_even_map,
    VecMap& subdivided_original_position_map)
{
    // FIXME: fill in.
}



OpenVolumeMesh::VertexHandle Subdivider::find_or_insert_even_vertex(
    OpenVolumeMesh::GeometricPolyhedralMeshV4f* to_subdivide,
    OpenVolumeMesh::GeometricPolyhedralMeshV4f* subdivided_mesh,

    VertHandleMap& translation_map,

    OpenVolumeMesh::VertexHandle& to_subdivide_v_handle,

    EvenMap& subdivided_is_even_map,

    VecMap& subdivided_original_position_map)
{

    VertHandleMap::iterator finder;
    finder = translation_map.find(to_subdivide_v_handle);
    if (finder == translation_map.end())
    {
        // could not find vertex.  insert it.
        OpenVolumeMesh::Geometry::Vec4f vertex_vec =
            to_subdivide->vertex(to_subdivide_v_handle);
        OpenVolumeMesh::VertexHandle subdivided_vertex_handle = subdivided_mesh->add_vertex(vertex_vec);
        
        // notate that the vertex is even
        subdivided_is_even_map[subdivided_vertex_handle] = true;
        subdivided_original_position_map[subdivided_vertex_handle] = vertex_vec;
        
        translation_map[to_subdivide_v_handle] = subdivided_vertex_handle;
        return subdivided_vertex_handle;
    }

    return finder->second;
}
    

OpenVolumeMesh::VertexHandle Subdivider::find_or_insert_midpoint(
    OpenVolumeMesh::GeometricPolyhedralMeshV4f* to_subdivide,
    OpenVolumeMesh::GeometricPolyhedralMeshV4f* subdivided_mesh,

    MidpointMap& midpoint_map,

    const OpenVolumeMesh::VertexHandle& v_handle_a,
    const OpenVolumeMesh::VertexHandle& v_handle_b,

    EvenMap& subdivided_is_even_map,
    VecMap& subdivided_original_position_map)
{
    MidpointMap::iterator midpoint_finder;
    
    std::pair<OpenVolumeMesh::VertexHandle,OpenVolumeMesh::VertexHandle> to_search_for(
        v_handle_a,v_handle_b);

    midpoint_finder = midpoint_map.find(to_search_for);

    if (midpoint_finder == midpoint_map.end())
    {
        std::pair<OpenVolumeMesh::VertexHandle,OpenVolumeMesh::VertexHandle> to_search_for(
            v_handle_b,v_handle_a);
        
        midpoint_finder = midpoint_map.find(to_search_for);
    }

    if (midpoint_finder == midpoint_map.end())
    {
        // create new midpoint vertex and insert it into map
        const OpenVolumeMesh::Geometry::Vec4f& a_vertex_vec =
            to_subdivide->vertex(v_handle_a);

        const OpenVolumeMesh::Geometry::Vec4f& b_vertex_vec =
            to_subdivide->vertex(v_handle_b);

        OpenVolumeMesh::Geometry::Vec4f midpoint(
            (a_vertex_vec[0] + b_vertex_vec[0])/2.0,
            (a_vertex_vec[1] + b_vertex_vec[1])/2.0,
            (a_vertex_vec[2] + b_vertex_vec[2])/2.0,
            (a_vertex_vec[3] + b_vertex_vec[3])/2.0);

        OpenVolumeMesh::VertexHandle midpoint_handle =
            to_subdivide->add_vertex(midpoint);
        
        // notate that the vertex is odd
        subdivided_is_even_map[midpoint_handle] = false;
        subdivided_original_position_map[midpoint_handle] = midpoint;
            
        return midpoint_handle;
    }

    return midpoint_finder->second;
}
