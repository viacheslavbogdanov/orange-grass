/*
 * OrangeGrass
 * Copyright (C) 2009 Vyacheslav Bogdanov.
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * as published by the Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program.  If not, see
 * <http://www.gnu.org/licenses/lgpl-3.0-standalone.html>.
 */
#include "ogmesh.h"
#include "OrangeGrass.h"


COGMesh::COGMesh ()
    : m_aabb(NULL)
    , m_buffer(NULL)
    , m_pCfg(NULL)
    , m_type(OG_SUBMESH_DUMMY)
    , m_pTexture(NULL)
    , m_pMaterial(NULL)
{
    m_pRenderer = GetRenderer();
}


COGMesh::~COGMesh()
{
    Unload();
}

// Load resource
bool COGMesh::Load (
    OGMeshCfg* _pCfg,
    const char* _pName,
    SubMeshType _Type,
    unsigned int _Part,
    const void* _pVertexData, 
    unsigned int _NumVertices, 
    unsigned int _NumFaces,
    unsigned int _Stride, 
    const void* _pIndexData, 
    unsigned int _NumIndices)
{
    m_pCfg = _pCfg;
    m_name = std::string(_pName);
    m_type = _Type;
    m_part = _Part;
    m_buffer = m_pRenderer->CreateVertexBuffer(_pVertexData, _NumVertices, 
        _NumFaces, _Stride, _pIndexData, _NumIndices);
    m_aabb = new IOGAabb();

    m_pTexture = GetResourceMgr()->GetTexture(OG_RESPOOL_GAME, m_pCfg->material_cfg.texture_alias.c_str());
    m_pMaterial = m_pRenderer->CreateMaterial();
    m_pMaterial->LoadConfig(&m_pCfg->material_cfg);

    return true;
}


// Unload resource.
void COGMesh::Unload ()
{
    m_pCfg = NULL;
    OG_SAFE_DELETE(m_buffer);
    OG_SAFE_DELETE(m_aabb);
    OG_SAFE_DELETE(m_pMaterial);
    m_pTexture = NULL;
    m_faces.clear();
}


// Render mesh.
void COGMesh::Render (const OGMatrix& _mWorld, OGRenderPass _Pass)
{
    OGBlendType blend = m_pMaterial->GetBlend();
    OGShaderID shaderID = m_pCfg->shaderID;
    if (_Pass == OG_RENDERPASS_SHADOWMAP)
        shaderID = m_pCfg->shadowShaderID;//OG_SHADER_SHADOWMODEL;
    m_pRenderer->RenderStatic(m_pTexture, m_pMaterial, m_buffer, _mWorld, blend, shaderID, _Pass);
}


// calculate geometry
void COGMesh::CalculateGeometry (const OGMatrix& _initialMat)
{
    OGVec3 v, vMinCorner, vMaxCorner;
    OGVec3* pPtr = (OGVec3*)m_buffer->GetVertexData();

    vMinCorner.x = vMaxCorner.x = pPtr->x; 
    vMinCorner.y = vMaxCorner.y = pPtr->y; 
    vMinCorner.z = vMaxCorner.z = pPtr->z;
    MatrixVecMultiply(vMinCorner, vMinCorner, _initialMat);
    MatrixVecMultiply(vMaxCorner, vMaxCorner, _initialMat);

    if(m_buffer->IsIndexed())
    {
        unsigned int numIndices = m_buffer->GetNumIndices();
        m_faces.clear();
        m_faces.reserve(numIndices / 3);
        for (unsigned int n = 0; n < numIndices; n+=3)
        {
            OGFace face;
            for (int k = 0; k < 3; ++k)
            {
                unsigned short ind = *(((unsigned short*)m_buffer->GetIndexData()) + n + k);
                face.vertices[k] = *((OGVec3*)((unsigned char*)(pPtr)+m_buffer->GetStride() * ind));

                OGVec3& v_out = face.vertices[k];
                MatrixVecMultiply(v_out, face.vertices[k], _initialMat);

                if (v_out.x < vMinCorner.x) vMinCorner.x = v_out.x;
                if (v_out.y < vMinCorner.y) vMinCorner.y = v_out.y;
                if (v_out.z < vMinCorner.z) vMinCorner.z = v_out.z;
                if (v_out.x > vMaxCorner.x) vMaxCorner.x = v_out.x;
                if (v_out.y > vMaxCorner.y) vMaxCorner.y = v_out.y;
                if (v_out.z > vMaxCorner.z) vMaxCorner.z = v_out.z;
            }
            m_faces.push_back(face);
        }
    }

    m_aabb->SetMinMax(vMinCorner, vMaxCorner);
}


// Get ray intersection
bool COGMesh::GetRayIntersection (const OGVec3& _vRayPos, const OGVec3& _vRayDir, OGVec3* _pOutPos)
{
    float t, u, v;
    unsigned int numFaces = m_faces.size();
    for (unsigned int i = 0; i < numFaces; ++i)
    {
        OGVec3 p0 = m_faces[i].vertices[0];
        OGVec3 p1 = m_faces[i].vertices[1];
        OGVec3 p2 = m_faces[i].vertices[2];
        if (CheckTriangleIntersection (_vRayPos, _vRayDir, p0, p1, p2, &t, &u, &v))
        {
            *_pOutPos = Barycentric2World(u, v, p0, p1, p2);
            return true;
        }
    }
    return false;
}
