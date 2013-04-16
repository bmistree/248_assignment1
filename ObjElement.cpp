#include "ObjElement.hpp"
#include <sstream>
#include <GL/gl.h>
#include <GL/glut.h>
#include <stdio.h>
#include <iostream>
#include <cassert>
#include <boost/regex.hpp>
#include <OpenVolumeMesh/Mesh/PolyhedralMesh.hh>
#include <OpenVolumeMesh/Geometry/VectorT.hh>
#include <math.h>

/**
   Returns copy of string with no whitespace at beginning or end of string.
 */
static void trim(std::string& line)
{
    std::size_t index_str_front = line.find_first_not_of(" \t");
    if (index_str_front != std::string::npos)
    {
        std::size_t index_str_end = line.find_last_not_of(" \t");
        line.replace(index_str_end + 1,line.size(),"");
        line.replace(0,index_str_front,"");
    }
}

/**
   Comment begins at '#' and ends at end of line.  Removes everything in the
   comment from begin to end of line.
 */
static void remove_comment(std::string& line)
{
    std::size_t index_comment_begin = line.find_first_of(OBJ_FILE_COMMENT_START);
    if (index_comment_begin != std::string::npos)
        line.replace(index_comment_begin,line.size(),"");
}


OpenVolumeMesh::VertexHandle Vertex::construct_from_line(
    OpenVolumeMesh::GeometricPolyhedralMeshV4f* obj_mesh, Vertex::VertexMap& vmap,
    std::string line)
{
    static VertexId vid = 0;
    OpenVolumeMesh::VertexHandle vertex_handle;
    
    remove_comment(line);
    trim(line);

    // using 2 here because, we must check at least second index to ensure that
    // not a texture coordinate or normal
    if (line.size() < 2)
        return vertex_handle;
    
    if ((line[0] == 'v') &&

        // ensure that not a texture or normal coordinate, both of which also
        // begin with 'v'.  FIXME: this is sort of gross to hard-code.  Maybe
        // figure out a better approach later.
        (line[1] != 't') &&
        (line[1] != 'n'))
    {
        // start from 1 to remove command character, 'v'.
        std::string to_tokenize = line.substr(1);
        std::istringstream tokenizer(to_tokenize);
        float x,y,z,w;
        w = 1.0;
        tokenizer >> x;
        tokenizer >> y;
        tokenizer >> z;
        if (! tokenizer.eof())
            tokenizer >> w;

        vertex_handle = obj_mesh->add_vertex(OpenVolumeMesh::Geometry::Vec4f(x,y,z,w));
        vmap[++vid] = vertex_handle;
    }
    return vertex_handle;
}



TextureCoordinate::TextureCoordinate(GLfloat _u, GLfloat _v, GLfloat _w)
 : u(_u),
   v(_v),
   w(_w)
{
    static TextureCoordId _tid = 0;
    tid = ++_tid;
}

TextureCoordinate::~TextureCoordinate()
{}
void TextureCoordinate::pretty_print() const
{
    printf("List of vertices: u=%f v=%f w=%f",u,v,w);
}

bool TextureCoordinate::construct_from_line(
    OpenVolumeMesh::GeometricPolyhedralMeshV4f* obj_mesh,
    TextureCoordinate::TextureCoordinateMap& obj_tc_map,
    std::string line)
{
    remove_comment(line);
    trim(line);

    // using 2 here because, we must check at least second index to ensure that
    // not a texture coordinate or normal
    if (line.size() < 2)
        return false;
    
    if ((line[0] == 'v') && (line[1] == 't'))
    {
        // start from 1 to remove command characters, 'vt'.
        std::string to_tokenize = line.substr(2);
        std::istringstream tokenizer(to_tokenize);
        float u,v,w;
        w = 0.0;
        tokenizer >> u;
        tokenizer >> v;
        if (! tokenizer.eof())
            tokenizer >> w;

        TextureCoordinate* tc = new TextureCoordinate(
            (GLfloat)u, (GLfloat)v, (GLfloat)w);
            
        obj_tc_map[tc->get_tc_id()] = tc;
        return true;
    }
    return false;
}



/************* FACE *************/

bool is_empty(OpenVolumeMesh::VertexHandle& a, OpenVolumeMesh::VertexHandle& b,
    OpenVolumeMesh::GeometricPolyhedralMeshV4f* obj_mesh)
{
    OpenVolumeMesh::Geometry::Vec4f c = obj_mesh->vertex(a) - obj_mesh->vertex(b);
    return (c[0] == 0) && (c[1] == 0) && (c[2] == 0);
}


OpenVolumeMesh::FaceHandle Face::construct_from_line(
    OpenVolumeMesh::GeometricPolyhedralMeshV4f* obj_mesh,
    TextureCoordinate::TextureCoordinateMap& obj_tc_map,
    TextureCoordinate::FaceTextureCoordinateMap& open_tc_map, 
    VertexNormal::VertNormalMap& obj_vnmap,VertexNormal::FaceVertNormalMap& open_vnmap,
    const Vertex::VertexMap& vmap, std::string line)
{
    OpenVolumeMesh::FaceHandle did_not_find;
    
    remove_comment(line);
    trim(line);

    // using 2 here because, we must check at least second index to ensure that
    // not a texture coordinate or normal
    if (line.size() < 1)
        return did_not_find;

    std::vector<OpenVolumeMesh::VertexHandle> vertices;
    std::vector<Vertex::VertexId> v_ids;
    std::vector<TextureCoordinate::TextureCoordId> tc_ids;
    std::vector<VertexNormal::VertexNormalId> vn_ids;

    
    VertexNormal::VertexNormalId vnid;
    TextureCoordinate::TextureCoordId tcid;
    bool had_normal = false;
    if (line[0] == 'f')
    {
        // start from 1 to remove command character, 'f'.
        std::string to_tokenize = line.substr(1);
        std::istringstream tokenizer(to_tokenize);
        if (to_tokenize.find("/") == std::string::npos)
        {
            while (! tokenizer.eof())
            {
                Vertex::VertexId vid;
                tokenizer >> vid;
                Vertex::VertexMapCIter citer = vmap.find(vid);
                vertices.push_back(citer->second);
            }
        }
        else
        {
            // Face has / format for each grouping:
            // a/b/c  d/e/f g/h/i ...
            boost::regex regex("(\\d+)/(\\d+)/(\\d+)");
            std::string::const_iterator to_search_from, end;
            to_search_from = to_tokenize.begin();
            end = to_tokenize.end();
            boost::match_results<std::string::const_iterator> matches;

            while(boost::regex_search(to_search_from,end,matches,regex))
            {
                Vertex::VertexId vid;
                vid = atoi(
                    std::string(matches[1].first,matches[1].second).c_str());

                tcid = atoi(
                    std::string(matches[2].first,matches[2].second).c_str());

                vnid = atoi(
                    std::string(matches[3].first,matches[3].second).c_str());
                had_normal = true;

                // using minus 1 here to synchronize numbering from obj with
                // numbering from openvolumemesh handles
                v_ids.push_back(vid-1);
                Vertex::VertexMapCIter citer = vmap.find(vid);
                vertices.push_back(citer->second);
                tc_ids.push_back(tcid);
                vn_ids.push_back(vnid);
                to_search_from = matches[3].second;
            }
        }

        OpenVolumeMesh::FaceHandle ovm_id = obj_mesh->add_face(vertices);
        if (had_normal)
        {
            for (int i = 0; i < tc_ids.size(); ++i)
            {
                open_vnmap[ovm_id][v_ids[i]] = obj_vnmap[vn_ids[i]];
                open_tc_map[ovm_id][v_ids[i]] = obj_tc_map[tc_ids[i]];
            }
        }
        return ovm_id;
    }
    return did_not_find;
}

/************* Vertex Normal **********/

VertexNormal::~VertexNormal()
{}

VertexNormal::VertexNormal(const GLfloat &x, const GLfloat& y, const GLfloat&z)
 : vn(x,y,z)
{
    static VertexNormalId _vnid = 0;
    _vnid += 1;
    vnid = _vnid;
}

static void b_normalize(OpenVolumeMesh::Geometry::Vec3f& normal)
{
    float len = sqrt(normal[0]*normal[0] + normal[1]*normal[1] + normal[2]*normal[2]);
    float mult = 1./len;
    normal = mult * normal;
}


void VertexNormal::calculate_normals(
    VertNormalMap& vnmap,
    OpenVolumeMesh::GeometricPolyhedralMeshV4f* obj_mesh)
{
    // for each vertex, run through all the faces its a part of.  Calculate
    
    for (OpenVolumeMesh::VertexIter viter = obj_mesh->vertices_begin();
         viter != obj_mesh->vertices_end(); ++viter)
    {
        OpenVolumeMesh::VertexHandle vhandle = *viter;

        
        // get all edges associated with vertex handle and put neighbors into
        // neighbor_handles
        std::vector<OpenVolumeMesh::VertexHandle> neighbor_handles;
        for(OpenVolumeMesh::VertexOHalfEdgeIter vohiter = obj_mesh->voh_iter(vhandle);
            vohiter.valid(); ++vohiter)
        {            
            OpenVolumeMesh::HalfEdgeHandle heh = *vohiter;            
            const OpenVolumeMesh::OpenVolumeMeshEdge& edge = obj_mesh->halfedge(heh);
            const OpenVolumeMesh::VertexHandle& from_vertex = edge.from_vertex();
            const OpenVolumeMesh::VertexHandle& to_vertex = edge.to_vertex();

            OpenVolumeMesh::VertexHandle neighbor_vhandle = from_vertex;
            if (from_vertex == vhandle)
                neighbor_vhandle = to_vertex;

            neighbor_handles.push_back(neighbor_vhandle);
        }

        OpenVolumeMesh::Geometry::Vec3f average_normal (0,0,0);
        
        OpenVolumeMesh::VertexHandle current_handle;
        OpenVolumeMesh::VertexHandle next_handle;
        int num_valid = 0;
        for (uint64_t i = 0; i < neighbor_handles.size() - 1; ++i)
        {
            current_handle = neighbor_handles[i];
            next_handle = neighbor_handles[i+1];
            bool valid;
            OpenVolumeMesh::Geometry::Vec3f inter =
                calc_normal(vhandle,current_handle,next_handle,obj_mesh,valid);
            
            if (valid)
            {
                ++num_valid;
                average_normal += inter;
            }
        }

        // current_handle = neighbor_handles[neighbor_handles.size() -1];
        // next_handle = neighbor_handles[0];
        //average_normal += calc_normal(vhandle,current_handle,next_handle,obj_mesh);

        next_handle = neighbor_handles[neighbor_handles.size() -1];
        current_handle = neighbor_handles[0];
        bool valid;
        OpenVolumeMesh::Geometry::Vec3f inter=
            calc_normal(vhandle,current_handle,next_handle,obj_mesh,valid);
        if (valid)
        {
            average_normal += inter;
            ++num_valid;
        }

        
        // float num_normals = neighbor_handles.size() -1;
        // average_normal *= (1./num_normals);
        float num_normals = (float)num_valid;
        average_normal *= (1./num_normals);
        b_normalize(average_normal);
        
        VertexNormal* vn = new VertexNormal(
            average_normal[0],average_normal[1],average_normal[2]);
        vnmap[vhandle] = vn;
    }
}

OpenVolumeMesh::Geometry::Vec3f VertexNormal::calc_normal(
    OpenVolumeMesh::VertexHandle vhandle0, OpenVolumeMesh::VertexHandle vhandle1,
    OpenVolumeMesh::VertexHandle vhandle2,
    OpenVolumeMesh::GeometricPolyhedralMeshV4f* obj_mesh, bool& valid)
{
    OpenVolumeMesh::Geometry::Vec4f _vec0 = obj_mesh->vertex(vhandle0);
    OpenVolumeMesh::Geometry::Vec4f _vec1 = obj_mesh->vertex(vhandle1);
    OpenVolumeMesh::Geometry::Vec4f _vec2 = obj_mesh->vertex(vhandle2);

    OpenVolumeMesh::Geometry::Vec3f vec0(_vec0[0],_vec0[1],_vec0[2]);
    OpenVolumeMesh::Geometry::Vec3f vec1(_vec1[0],_vec1[1],_vec1[2]);
    OpenVolumeMesh::Geometry::Vec3f vec2(_vec2[0],_vec2[1],_vec2[2]);

    // Stack overflow showed me how to calculate normals
    // http://stackoverflow.com/questions/1966587/given-3-pts-how-do-i-calculate-the-normal-vector
    //OpenVolumeMesh::Geometry::Vec3f dir = (vec0 - vec1) % (vec2 - vec1);
    //OpenVolumeMesh::Geometry::Vec3f dir = (vec1 - vec0) % (vec2 - vec0);
    OpenVolumeMesh::Geometry::Vec3f dir = (vec2 - vec0) % (vec1 - vec0);

    
    float len = sqrt(dir[0]*dir[0] + dir[1]*dir[1] + dir[2]*dir[2]);
    valid = true;
    if (len == 0)
    {
        valid = false;
        // handles case of degnerate triangle.
        // dir[0] = 0;
        // dir[1] = 0;
        // dir[2] = 1;
        dir[0] = 1;
        dir[1] = 0;
        dir[2] = 0;
        
        return dir;
    }
    
    float normalization = 1./len;
    OpenVolumeMesh::Geometry::Vec3f normalized = dir*normalization;
    
    // std::cout<<"\nThis was normal: "<< normalized;
    return dir*normalization;
}


bool VertexNormal::construct_from_line(VertNormalMap& vnmap,std::string line)
{
    remove_comment(line);
    trim(line);

    // using 2 here because, we must check at least second index to ensure that
    // not a texture coordinate or normal
    if (line.size() < 2)
        return false;

    if ((line[0] == 'v') && (line[1] == 'n'))
    {
        // start from 1 to remove command characters, 'vn'.
        std::string to_tokenize = line.substr(2);
        std::istringstream tokenizer(to_tokenize);
        float x,y,z;
        tokenizer >> x;
        tokenizer >> y;
        tokenizer >> z;        

        VertexNormal* vn = new VertexNormal((GLfloat)x, (GLfloat)y, (GLfloat)z);
        vnmap[vn->get_vnid()] = vn;
        return true;
    }
    return false;
}

