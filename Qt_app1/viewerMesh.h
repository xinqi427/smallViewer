#ifndef VIEWERMESH_H
#define VIEWERMESH_H

#include <vector>
#include <unordered_set>
#include <queue>
#include <string>
#include <functional>
#include <algorithm>
#include "Mesh/mesh.h"

#ifndef EPS 
#define EPS 1e-7
#endif

using CPoint = typename MeshLib::CPoint;
using CPoint2 = typename MeshLib::CPoint2;

class CViewerVertex
{
public:


};

class CViewerEdge
{
public:

};

class CViewerFace
{
public:


};

class CViewerHalfEdge
{
public:

};

using CMesh = MeshLib::CBaseMesh<CViewerVertex, CViewerEdge, CViewerFace, CViewerHalfEdge>;
using CVertex = typename CMesh::CVertex;
using CEdge = typename CMesh::CEdge;
using CFace = typename CMesh::CFace;
using CHalfEdge = typename CMesh::CHalfEdge;

class ViewerMesh
{
public: 
    ViewerMesh();
    ~ViewerMesh();

    int input_obj(std::string fname);
    int normalize();

    CMesh * &m_mesh() { return pMesh; }
    
    bool mesh_with_uv = false;
    bool mesh_with_normal = false;

private:
    CMesh * pMesh;

};

#endif