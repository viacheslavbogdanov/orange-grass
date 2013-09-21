/*
*  ogmodel.cpp
*  OrangeGrass
*
*  Created by Viacheslav Bogdanov on 08.11.09.
*  Copyright 2009 __MyCompanyName__. All rights reserved.
*
*/

#include "OrangeGrass.h"
#include "ogmodel.h"


COGModel::COGModel() 
    : m_pTexture(NULL)
    , m_pMaterial(NULL)
    , m_pScene(NULL)
    , m_NumParts(0)
{
    m_pRenderer = GetRenderer();
    m_pReader = GetSettingsReader();
}


COGModel::~COGModel()
{
    Unload();
}


// Load model.
bool COGModel::Load ()
{
    switch (m_LoadState)
    {
    case OG_RESSTATE_UNKNOWN:
        return false;

    case OG_RESSTATE_LOADED:
        return true;

    case OG_RESSTATE_DEFINED:
        break;
    }

    Cfg modelcfg;
    if (!LoadConfig(modelcfg))
    {
        OG_LOG_ERROR("Failed to load model from config");
        return false;
    }

    m_pScene = new CPVRTModelPOD();
    if (!m_pScene->ReadFromFile(modelcfg.mesh_file.c_str()))
    {
        OG_LOG_ERROR("Failed to load mesh from file %s", modelcfg.mesh_file.c_str());
        return false;
    }
    m_pScene->SetFrame(0);

    m_NumParts = 0;
    m_Meshes.reserve(m_pScene->nNumMeshNode);

    m_SolidParts.reserve(m_pScene->nNumMeshNode);
    m_TransparentParts.reserve(m_pScene->nNumMeshNode);

    for(unsigned int i = 0; i < m_pScene->nNumMeshNode; ++i)
    {
        SPODNode* pNode = &m_pScene->pNode[i];
        SPODMesh& Mesh = m_pScene->pMesh[pNode->nIdx];

        std::string subMeshName = std::string(pNode->pszName);
        SubMeshType sbmtype = ParseSubMeshType(subMeshName);
        switch (sbmtype)
        {
        case OG_SUBMESH_ACTPOINT:
            {
                OGVec3* pPtr = (OGVec3*)Mesh.pInterleaved;
                OGActivePoint pt;
                pt.pos = *pPtr;
                pt.part = i;
                m_ActivePoints[subMeshName] = pt;
                continue;
            }
            break;

        case OG_SUBMESH_BODY: 
            m_SolidParts.push_back(m_NumParts); 
            break;
        case OG_SUBMESH_PROPELLER: 
            m_TransparentParts.push_back(m_NumParts); 
            break;
        }

        COGMesh* pMesh = new COGMesh();
        if (!pMesh->Load(subMeshName.c_str(), sbmtype, i, Mesh.pInterleaved, Mesh.nNumVertex, 
            Mesh.nNumFaces, Mesh.sVertex.nStride, Mesh.sFaces.pData, PVRTModelPODCountIndices(Mesh)))
        {
            OG_LOG_ERROR("Failed to load model's mesh %s", modelcfg.mesh_file.c_str());
            OG_SAFE_DELETE(pMesh);
            return false;
        }

        OGMatrix mModel;
        m_pScene->GetWorldMatrix(mModel, m_pScene->pNode[i]);
        pMesh->CalculateGeometry(mModel);
        m_AABB.EmbraceAABB(pMesh->GetAABB());

        m_Meshes.push_back(pMesh);
        ++m_NumParts;
    }

    m_pTexture = GetResourceMgr()->GetTexture(OG_RESPOOL_GAME, modelcfg.material.texture_alias);
    m_pMaterial = m_pRenderer->CreateMaterial();
    m_pMaterial->LoadConfig(&modelcfg.material);

    std::list<Cfg::Anim>::const_iterator anim_iter = modelcfg.anim_list.begin();
    for (; anim_iter != modelcfg.anim_list.end(); ++anim_iter)
    {
        const COGModel::Cfg::Anim& anim = (*anim_iter);
        IOGAnimation* pAnim = new IOGAnimation();
        pAnim->name = anim.anim_alias;
        pAnim->start_frame = (unsigned int)anim.anim_start;
        pAnim->end_frame = (unsigned int)anim.anim_end;
        pAnim->speed = (unsigned int)anim.speed;
        pAnim->looped = (anim.looped == 0) ? false : true;
        m_pAnimations[pAnim->name] = pAnim;
    }

    m_LoadState = OG_RESSTATE_LOADED;
    return true;
}


// Load model configuration
bool COGModel::LoadConfig (COGModel::Cfg& _cfg)
{
    IOGSettingsSource* pSource = m_pReader->OpenSource(m_ResourceFile);
    if (!pSource)
    {
        OG_LOG_ERROR("Failed to load model config file %s", m_ResourceFile.c_str());
        return false;
    }

    IOGGroupNode* pMeshNode = m_pReader->OpenGroupNode(pSource, NULL, "Mesh");
    if (pMeshNode != NULL)
    {
        _cfg.mesh_file = GetResourceMgr()->GetFullPath(m_pReader->ReadStringParam(pMeshNode, "file"));
        m_pReader->CloseGroupNode(pMeshNode);
    }

    IOGGroupNode* pMaterialNode = m_pReader->OpenGroupNode(pSource, NULL, "Material");
    if (pMaterialNode != NULL)
    {
        _cfg.material.texture_alias = m_pReader->ReadStringParam(pMaterialNode, "texture");
        _cfg.material.blend_type = ParseBlendType(m_pReader->ReadStringParam(pMaterialNode, "blend"));
        IOGGroupNode* pAmbientNode = m_pReader->OpenGroupNode(pSource, pMaterialNode, "Ambient");
        if (pAmbientNode)
        {
            _cfg.material.material_ambient = m_pReader->ReadVec4Param(pAmbientNode, "r", "g", "b", "a");
            m_pReader->CloseGroupNode(pAmbientNode);
        }
        IOGGroupNode* pDiffuseNode = m_pReader->OpenGroupNode(pSource, pMaterialNode, "Diffuse");
        if (pDiffuseNode)
        {
            _cfg.material.material_diffuse = m_pReader->ReadVec4Param(pDiffuseNode, "r", "g", "b", "a");
            m_pReader->CloseGroupNode(pDiffuseNode);
        }
        IOGGroupNode* pSpecularNode = m_pReader->OpenGroupNode(pSource, pMaterialNode, "Specular");
        if (pSpecularNode)
        {
            _cfg.material.material_specular = m_pReader->ReadVec4Param(pSpecularNode, "r", "g", "b", "a");
            m_pReader->CloseGroupNode(pSpecularNode);
        }

        m_pReader->CloseGroupNode(pMaterialNode);
    }

    IOGGroupNode* pAnimationsNode = m_pReader->OpenGroupNode(pSource, NULL, "Animations");
    if (pAnimationsNode)
    {
        IOGGroupNode* pAnimationNode = m_pReader->OpenGroupNode(pSource, pAnimationsNode, "Animation");
        while (pAnimationNode != NULL)
        {
            COGModel::Cfg::Anim anim;
            anim.anim_alias = m_pReader->ReadStringParam(pAnimationNode, "name");
            anim.anim_start = m_pReader->ReadIntParam(pAnimationNode, "start_frame");
            anim.anim_end = m_pReader->ReadIntParam(pAnimationNode, "end_frame");
            anim.speed = m_pReader->ReadIntParam(pAnimationNode, "speed");
            anim.looped = m_pReader->ReadIntParam(pAnimationNode, "looped");
            _cfg.anim_list.push_back(anim);
            pAnimationNode = m_pReader->ReadNextNode(pAnimationNode);
        }
        m_pReader->CloseGroupNode(pAnimationsNode);
    }

    m_pReader->CloseSource(pSource);
    return true;
}


// Save params
bool COGModel::SaveParams ()
{
    IOGSettingsSource* pSource = m_pReader->OpenSource(m_ResourceFile);
    if (!pSource)
    {
        OG_LOG_ERROR("Failed to load model config file %s", m_ResourceFile.c_str());
        return false;
    }

    IOGGroupNode* pMaterialNode = m_pReader->OpenGroupNode(pSource, NULL, "Material");
    if (pMaterialNode != NULL)
    {
        IOGGroupNode* pAmbientNode = m_pReader->OpenGroupNode(pSource, pMaterialNode, "Ambient");
        if (pAmbientNode)
        {
            m_pReader->WriteVec4Param(pAmbientNode, "r", "g", "b", "a", m_pMaterial->GetAmbient());
            m_pReader->CloseGroupNode(pAmbientNode);
        }
        IOGGroupNode* pDiffuseNode = m_pReader->OpenGroupNode(pSource, pMaterialNode, "Diffuse");
        if (pDiffuseNode)
        {
            m_pReader->WriteVec4Param(pDiffuseNode, "r", "g", "b", "a", m_pMaterial->GetDiffuse());
            m_pReader->CloseGroupNode(pDiffuseNode);
        }
        IOGGroupNode* pSpecularNode = m_pReader->OpenGroupNode(pSource, pMaterialNode, "Specular");
        if (pSpecularNode)
        {
            m_pReader->WriteVec4Param(pSpecularNode, "r", "g", "b", "a", m_pMaterial->GetSpecular());
            m_pReader->CloseGroupNode(pSpecularNode);
        }
    }

    m_pReader->SaveSource(pSource, m_ResourceFile);
    return true;
}


// Unload resource.
void COGModel::Unload ()
{
    if (m_LoadState != OG_RESSTATE_LOADED)
    {
        return;
    }

    m_SolidParts.clear();
    m_TransparentParts.clear();
    m_ActivePoints.clear();

    OG_SAFE_DELETE(m_pScene);
    OG_SAFE_DELETE(m_pMaterial);

    std::for_each(m_Meshes.begin(), m_Meshes.end(), [](IOGMesh* m) { m->Unload(); OG_SAFE_DELETE(m); });
    m_Meshes.clear();

    std::for_each(m_pAnimations.begin(), m_pAnimations.end(), [](std::pair<const std::string, IOGAnimation*> m) { OG_SAFE_DELETE(m.second); });
    m_pAnimations.clear();

    m_LoadState = OG_RESSTATE_DEFINED;
}


// Render mesh.
void COGModel::Render (const OGMatrix& _mWorld, unsigned int _Frame)
{
    m_pScene->SetFrame((float)_Frame);
    m_pRenderer->SetMaterial(m_pMaterial);
    m_pRenderer->SetTexture(m_pTexture);
    std::for_each(m_SolidParts.begin(), m_SolidParts.end(), [&](unsigned int i) 
    {
        IOGMesh* pMesh = m_Meshes[i];

        const SPODNode& node = m_pScene->pNode[pMesh->GetPart()];

        // Get the node model matrix
        OGMatrix mNodeWorld;
        m_pScene->GetWorldMatrix(mNodeWorld, node);

        // Multiply on the global world transform
        OGMatrix mModel;
        MatrixMultiply(mModel, mNodeWorld, _mWorld);

        pMesh->Render(mModel);
    });
}


// Render solid parts of the mesh.
void COGModel::RenderSolidParts (const OGMatrix& _mWorld, unsigned int _Frame)
{
    m_pScene->SetFrame((float)_Frame);
    m_pRenderer->SetMaterial(m_pMaterial);
    m_pRenderer->SetTexture(m_pTexture);
    std::for_each(m_SolidParts.begin(), m_SolidParts.end(), [&](unsigned int i) 
    {
        IOGMesh* pMesh = m_Meshes[i];

        const SPODNode& node = m_pScene->pNode[pMesh->GetPart()];

        // Get the node model matrix
        OGMatrix mNodeWorld;
        m_pScene->GetWorldMatrix(mNodeWorld, node);

        // Multiply on the global world transform
        OGMatrix mModel;
        MatrixMultiply(mModel, mNodeWorld, _mWorld);

        pMesh->Render(mModel);
    });
}


// Render transparent parts of the mesh.
void COGModel::RenderTransparentParts (const OGMatrix& _mWorld, unsigned int _Frame, float _fSpin)
{
    m_pScene->SetFrame((float)_Frame);
    m_pRenderer->SetMaterial(m_pMaterial);
    m_pRenderer->SetTexture(m_pTexture);
    m_pRenderer->SetBlend(OG_BLEND_ALPHABLEND);
    OGMatrix mSpin, mNodeWorld, mModel;
    std::for_each(m_TransparentParts.begin(), m_TransparentParts.end(), [&](unsigned int i) 
    {
        IOGMesh* pMesh = m_Meshes[i];
        const SPODNode& node = m_pScene->pNode[pMesh->GetPart()];
        MatrixRotationY(mSpin, _fSpin);
        m_pScene->GetWorldMatrix(mNodeWorld, node);
        MatrixMultiply(mNodeWorld, mSpin, mNodeWorld);
        MatrixMultiply(mModel, mNodeWorld, _mWorld);

        pMesh->Render(mModel);
    });
}


// Check if has submeshes of the following type
bool COGModel::HasSubmeshesOfType(SubMeshType _Type) const
{
    bool bResult = false;
    std::for_each(m_Meshes.begin(), m_Meshes.end(), [&](IOGMesh* m)
    { 
        if (m->GetType() == _Type)
            bResult = true;
    });
    return bResult;
}


// Get num renderable parts.
unsigned int COGModel::GetNumRenderables () const
{
    return (unsigned int)m_Meshes.size();
}


// Get part's transformed OBB after applying animation
bool COGModel::GetTransformedOBB (IOGObb& _obb, unsigned int _Part, unsigned int _Frame, const OGMatrix& _mWorld) const
{
    const IOGMesh* pMesh = m_Meshes[_Part];
    m_pScene->SetFrame((float)_Frame);
    const SPODNode& node = m_pScene->pNode[pMesh->GetPart()];
    OGMatrix mNodeWorld, mModel;
    m_pScene->GetWorldMatrix(mNodeWorld, node);
    MatrixMultiply(mModel, mNodeWorld, _mWorld);
    _obb.Create(pMesh->GetAABB());
    _obb.UpdateTransform(mModel);
    return true;
}


// Get combined AABB
const IOGAabb& COGModel::GetAABB () const
{
    return m_AABB;
}


// Get model alias
const std::string& COGModel::GetAlias () const
{
    return m_ResourceAlias;
}


// Get animation
IOGAnimation* COGModel::GetAnimation (const std::string& _Alias)
{
    return m_pAnimations[_Alias];
}


// Get active point
bool COGModel::GetActivePoint (IOGActivePoint& _point, const std::string& _Alias, unsigned int _Frame)
{
    auto iter = m_ActivePoints.find(_Alias);
    if (iter != m_ActivePoints.end())
    {
        m_pScene->SetFrame((float)_Frame);
        // Gets the node model matrix
        OGMatrix mNodeWorld;
        m_pScene->GetWorldMatrix(mNodeWorld, m_pScene->pNode[iter->second.part]);

        MatrixVecMultiply(_point.pos, iter->second.pos, mNodeWorld);
        _point.alias = _Alias;
        return true;
    }

    return false;
}


// Get ray intersection
bool COGModel::GetRayIntersection (const OGVec3& _vRayPos, const OGVec3& _vRayDir, OGVec3* _pOutPos)
{
    for (auto it = m_Meshes.begin(); it != m_Meshes.end(); ++it)
    {
        if ((*it)->GetRayIntersection(_vRayPos, _vRayDir, _pOutPos))
            return true;
    }
    return false;
}
