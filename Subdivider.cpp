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

    adjust_subdivided_vertex_positions(
        subdivided_mesh,subdivided_is_even_map,subdivided_original_position_map);
    
    return subdivided_mesh;
}

void Subdivider::adjust_subdivided_vertex_positions(
    OpenVolumeMesh::GeometricPolyhedralMeshV4f* subdivided_mesh,
    const EvenMap& subdivided_is_even_map,
    const VecMap& subdivided_original_position_map)
{
    for (OpenVolumeMesh::VertexIter viter = subdivided_mesh->vertices_begin();
         viter != subdivided_mesh->vertices_end(); ++viter)
    {
        OpenVolumeMesh::VertexHandle vhandle = *viter;

        bool is_even = subdivided_is_even_map.find(vhandle)->second;
        
        if (is_even)
        {
            // vertex is even
            handle_even_vertex(subdivided_mesh,vhandle,subdivided_original_position_map);
        }
        else
        {
            // vertex is odd
            handle_odd_vertex(
                subdivided_mesh,vhandle,subdivided_is_even_map,subdivided_original_position_map);
        }
    }
}

void Subdivider::handle_odd_vertex(
    OpenVolumeMesh::GeometricPolyhedralMeshV4f* subdivided_mesh,
    const OpenVolumeMesh::VertexHandle& vhandle,
    const EvenMap& subdivided_is_even_map,
    const VecMap& subdivided_original_position_map)
{
    // each odd vertex should only have two immediate neighbors.  get each.
    OpenVolumeMesh::VertexHandle neighbor_0_vhandle,neighbor_1_vhandle;
    
    uint64_t num_neighbors = 0;
    uint64_t real_num_neighbors = 0;
    for(OpenVolumeMesh::VertexOHalfEdgeIter vohiter = subdivided_mesh->voh_iter(vhandle);
        vohiter.valid(); ++vohiter)
    {
        ++real_num_neighbors;
        OpenVolumeMesh::HalfEdgeHandle heh = *vohiter;
        const OpenVolumeMesh::OpenVolumeMeshEdge& edge = subdivided_mesh->halfedge(heh);
        const OpenVolumeMesh::VertexHandle& from_vertex = edge.from_vertex();
        const OpenVolumeMesh::VertexHandle& to_vertex = edge.to_vertex();

        OpenVolumeMesh::VertexHandle neighbor_vhandle = from_vertex;
        if (from_vertex == vhandle)
            neighbor_vhandle = to_vertex;

        // ignore odd vertices
        bool is_even = subdivided_is_even_map.find(neighbor_vhandle)->second;
        if (! is_even)
            continue;

        // should only have two even neighbors
        if (num_neighbors == 0)
            neighbor_0_vhandle = neighbor_vhandle;
        else
            neighbor_1_vhandle = neighbor_vhandle;

        ++ num_neighbors;
    }

    if (num_neighbors != 2)
        assert(false);
    
    OpenVolumeMesh::Geometry::Vec4f immediate_neighbor_0_vec = 
        subdivided_original_position_map.find(neighbor_0_vhandle)->second;
    OpenVolumeMesh::Geometry::Vec4f immediate_neighbor_1_vec = 
        subdivided_original_position_map.find(neighbor_1_vhandle)->second;


    OpenVolumeMesh::Geometry::Vec4f immediate_neighbor_vec (
        ODD_IMMEDIATE_NEIGHBOR_FACTOR * (
            immediate_neighbor_0_vec[0] + immediate_neighbor_1_vec[0]),

        ODD_IMMEDIATE_NEIGHBOR_FACTOR * (
            immediate_neighbor_0_vec[1] + immediate_neighbor_1_vec[1]),

        ODD_IMMEDIATE_NEIGHBOR_FACTOR * (
            immediate_neighbor_0_vec[2] + immediate_neighbor_1_vec[2]),

        ODD_IMMEDIATE_NEIGHBOR_FACTOR * (
            immediate_neighbor_0_vec[3] + immediate_neighbor_1_vec[3]));

    
    // handle distant neighbors    
    OpenVolumeMesh::Geometry::Vec4f distant_neighbor_0_vec, distant_neighbor_1_vec;
    distant_neighbor_vec(
        subdivided_mesh,vhandle,neighbor_0_vhandle, neighbor_1_vhandle,
        distant_neighbor_0_vec, distant_neighbor_1_vec,
        subdivided_is_even_map,subdivided_original_position_map);
    
    
    OpenVolumeMesh::Geometry::Vec4f distant_neighbor_vec(
        ODD_DISTANT_NEIGHBOR_FACTOR*(
            distant_neighbor_0_vec[0]+ distant_neighbor_1_vec[0] ),

        ODD_DISTANT_NEIGHBOR_FACTOR*(
            distant_neighbor_0_vec[1]+ distant_neighbor_1_vec[1] ),

        ODD_DISTANT_NEIGHBOR_FACTOR*(
            distant_neighbor_0_vec[2]+ distant_neighbor_1_vec[2] ),

        ODD_DISTANT_NEIGHBOR_FACTOR*(
            distant_neighbor_0_vec[3]+ distant_neighbor_1_vec[3] ));
        
    subdivided_mesh->set_vertex(
        vhandle,distant_neighbor_vec + immediate_neighbor_vec);
}
    

void Subdivider::distant_neighbor_vec(
    OpenVolumeMesh::GeometricPolyhedralMeshV4f* subdivided_mesh,
    const OpenVolumeMesh::VertexHandle& vhandle,
    const OpenVolumeMesh::VertexHandle& neighbor_0_vhandle,
    const OpenVolumeMesh::VertexHandle& neighbor_1_vhandle,
    OpenVolumeMesh::Geometry::Vec4f& dist_0_vec,
    OpenVolumeMesh::Geometry::Vec4f& dist_1_vec,
    const EvenMap& subdivided_is_even_map,
    const VecMap& subdivided_original_position_map)
{
    // iterate through the neighbors of one neighbor and then iterate through
    // the neighbors of the other.  Any that are in common get returned.

    VertexBoolMap n0_neighbors,n1_neighbors;
    get_even_neighbor_vertices(
        subdivided_mesh,neighbor_0_vhandle,subdivided_is_even_map,n0_neighbors);
    get_even_neighbor_vertices(
        subdivided_mesh,neighbor_1_vhandle,subdivided_is_even_map,n1_neighbors);

    std::vector<OpenVolumeMesh::VertexHandle> common_handles;
    for (VertexBoolMap::const_iterator citer_n0 = n0_neighbors.begin();
         citer_n0 != n0_neighbors.end(); ++citer_n0)
    {
        OpenVolumeMesh::VertexHandle vh = citer_n0->first;
        
        // do not care about the current vertex position
        if (vh == vhandle)
            continue;
        
        if (n1_neighbors.find(vh) != n1_neighbors.end())
            common_handles.push_back(vh);
    }

    if (common_handles.size() != 2)
        assert(false);


    dist_0_vec = subdivided_original_position_map.find(
        common_handles[0])->second;
    dist_1_vec = subdivided_original_position_map.find(
        common_handles[1])->second;    
}


void Subdivider::get_even_neighbor_vertices(
    OpenVolumeMesh::GeometricPolyhedralMeshV4f* subdivided_mesh,
    const OpenVolumeMesh::VertexHandle& vhandle,
    const EvenMap& subdivided_is_even_map,
    VertexBoolMap& vbm)
{
    // Essentially, not actually looking for neighbors, looking for what
    // neighbors would have been before added midpoints (ie, odd vertices).
    // So, if we encounter a midpoint, look for all of its even neighbors
    // and use those.  Note: it will only have two even neighbors: vhandle
    // and vhandle's original even neighbor that midpoint now lies between.
    
    for(OpenVolumeMesh::VertexOHalfEdgeIter vohiter = subdivided_mesh->voh_iter(vhandle);
        vohiter.valid(); ++vohiter)
    {
        OpenVolumeMesh::HalfEdgeHandle heh = *vohiter;
        const OpenVolumeMesh::OpenVolumeMeshEdge& edge = subdivided_mesh->halfedge(heh);
        const OpenVolumeMesh::VertexHandle& from_vertex = edge.from_vertex();
        const OpenVolumeMesh::VertexHandle& to_vertex = edge.to_vertex();
        
        OpenVolumeMesh::VertexHandle neighbor_vhandle = from_vertex;
        if (from_vertex == vhandle)
            neighbor_vhandle = to_vertex;
        
        // FIXME: very ugly the way I'm doing this.  Better to recurse.
        if (! subdivided_is_even_map.find(neighbor_vhandle)->second)
        {            
            // means that the neighbor is odd: get all of the neighbor's
            // neighbors, which will be both odd and even.  Run through the
            // neighbors vertices to find even.

            for (OpenVolumeMesh::VertexOHalfEdgeIter _vohiter = subdivided_mesh->voh_iter(neighbor_vhandle);
                 _vohiter.valid(); ++_vohiter)
            {
                OpenVolumeMesh::HalfEdgeHandle _heh = *_vohiter;
                const OpenVolumeMesh::OpenVolumeMeshEdge& _edge = subdivided_mesh->halfedge(_heh);
                const OpenVolumeMesh::VertexHandle& _from_vertex = _edge.from_vertex();
                const OpenVolumeMesh::VertexHandle& _to_vertex = _edge.to_vertex();

                OpenVolumeMesh::VertexHandle _neighbor_vhandle = _from_vertex;
                if (_from_vertex == neighbor_vhandle)
                    _neighbor_vhandle = _to_vertex;

                if (_neighbor_vhandle == vhandle)
                    continue;
                
                if (subdivided_is_even_map.find(_neighbor_vhandle)->second)
                {
                    neighbor_vhandle = _neighbor_vhandle;
                    break;
                }
            }
        }
        vbm[neighbor_vhandle] = true;
    }
}





void Subdivider::handle_even_vertex(
    OpenVolumeMesh::GeometricPolyhedralMeshV4f* subdivided_mesh,
    const OpenVolumeMesh::VertexHandle& vhandle,
    const VecMap& subdivided_original_position_map)
{
    uint64_t num_neighbors = 0;
    OpenVolumeMesh::Geometry::Vec4f neighbor_vec(0,0,0,0);
    
    for(OpenVolumeMesh::VertexOHalfEdgeIter vohiter = subdivided_mesh->voh_iter(vhandle);
        vohiter.valid(); ++vohiter)
    {
        ++ num_neighbors;
        OpenVolumeMesh::HalfEdgeHandle heh = *vohiter;
        const OpenVolumeMesh::OpenVolumeMeshEdge& edge = subdivided_mesh->halfedge(heh);
        const OpenVolumeMesh::VertexHandle& from_vertex = edge.from_vertex();
        const OpenVolumeMesh::VertexHandle& to_vertex = edge.to_vertex();

        OpenVolumeMesh::VertexHandle neighbor_vhandle = from_vertex;
        if (from_vertex == vhandle)
            neighbor_vhandle = to_vertex;

        OpenVolumeMesh::Geometry::Vec4f vertex_vec =
            subdivided_original_position_map.find(neighbor_vhandle)->second;


        neighbor_vec[0] += vertex_vec[0];
        neighbor_vec[1] += vertex_vec[1];
        neighbor_vec[2] += vertex_vec[2];
        neighbor_vec[3] += vertex_vec[3];
    }

    float beta = 3./16.;
    if (num_neighbors > 3)
        beta = (3./(8. * ((float)num_neighbors)));
    
    neighbor_vec[0] *= beta;
    neighbor_vec[1] *= beta;
    neighbor_vec[2] *= beta;
    neighbor_vec[3] *= beta;

    // calculate new position of even vertex in pos_vec
    
    OpenVolumeMesh::Geometry::Vec4f pos_vec =
        subdivided_original_position_map.find(vhandle)->second;
    
    float anti_beta = 1.0 - (((float)num_neighbors)*beta);
    pos_vec[0] *= anti_beta;
    pos_vec[1] *= anti_beta;
    pos_vec[2] *= anti_beta;
    pos_vec[3] *= anti_beta;

    pos_vec[0] += neighbor_vec[0];
    pos_vec[1] += neighbor_vec[1];
    pos_vec[2] += neighbor_vec[2];
    pos_vec[3] += neighbor_vec[3];

    subdivided_mesh->set_vertex(vhandle,pos_vec);
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
    std::vector<OpenVolumeMesh::VertexHandle> vertices;
    
    // Add a new face in bottom left of triangle
    vertices.push_back(bottom_left);
    vertices.push_back(midpoint_top_bottom_left);
    vertices.push_back(midpoint_bottom_left_bottom_right);
    subdivided_mesh->add_face(vertices);
    vertices.clear();
    
    // Add a new face in top of triangle
    vertices.push_back(midpoint_top_bottom_left);
    vertices.push_back(top);
    vertices.push_back(midpoint_top_bottom_right);
    subdivided_mesh->add_face(vertices);
    vertices.clear();

    // Add a new face in bottom right of triangle
    vertices.push_back(midpoint_top_bottom_right);
    vertices.push_back(bottom_right);
    vertices.push_back(midpoint_bottom_left_bottom_right);
    subdivided_mesh->add_face(vertices);
    vertices.clear();

    // Add a new face in the middle of the triangle
    vertices.push_back(midpoint_top_bottom_left);
    vertices.push_back(midpoint_top_bottom_right);
    vertices.push_back(midpoint_bottom_left_bottom_right);
    subdivided_mesh->add_face(vertices);
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
        const OpenVolumeMesh::Geometry::Vec4f& vertex_vec =
            to_subdivide->vertex(to_subdivide_v_handle);
        
        OpenVolumeMesh::VertexHandle subdivided_vertex_handle =
            subdivided_mesh->add_vertex(vertex_vec);

        
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
            subdivided_mesh->add_vertex(midpoint);

        
        // notate that the vertex is odd
        subdivided_is_even_map[midpoint_handle] = false;
        subdivided_original_position_map[midpoint_handle] = midpoint;

        midpoint_map[to_search_for] = midpoint_handle;
        return midpoint_handle;
    }

    return midpoint_finder->second;
}
