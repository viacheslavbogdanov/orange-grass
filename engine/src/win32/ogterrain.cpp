/*
 *  ogterrain.cpp
 *  OrangeGrass
 *
 *  Created by Viacheslav Bogdanov on 12.11.09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#include "OrangeGrass.h"
#include "ogterrain.h"
#include "IOGMath.h"


COGTerrain::COGTerrain () :	m_pMesh(NULL),
                            m_pMaterial(NULL)
{
    m_pRenderer = GetRenderer();
	m_pReader = GetSettingsReader();
}


COGTerrain::~COGTerrain()
{
	m_pMesh = NULL;
    OG_SAFE_DELETE(m_pMaterial);
}


// Load terrain.
bool COGTerrain::Load ()
{
	switch (m_LoadState)
	{
	case OG_RESSTATE_UNKNOWN:
		return false;

	case OG_RESSTATE_LOADED:
        return true;    
	}

	Cfg cfg;
	if (!LoadConfig(cfg))
	{
		return false;
	}

    m_pMesh = GetResourceMgr()->GetMesh(cfg.mesh_alias);
    m_pMaterial = GetMaterialManager()->GetMaterial(OG_MAT_SOLID);

	std::vector<Cfg::TextureCfg>::const_iterator iter;
	for (iter = cfg.texture_cfg_list.begin(); iter != cfg.texture_cfg_list.end(); ++iter)
	{
		IOGTexture* pTexture = GetResourceMgr()->GetTexture((*iter).alias);
		m_TextureList.push_back(pTexture);
	}

	m_LoadState = OG_RESSTATE_LOADED;
	return true;
}


// Load terrain configuration
bool COGTerrain::LoadConfig (COGTerrain::Cfg& _cfg)
{
	IOGSettingsSource* pSource = m_pReader->OpenSource(m_ResourceFile);
	if (!pSource)
	{
		OG_LOG_ERROR("Failed to load terrain config file %s", m_ResourceFile.c_str());
		return false;
	}

	IOGGroupNode* pMeshNode = m_pReader->OpenGroupNode(pSource, NULL, "Mesh");
	if (pMeshNode != NULL)
	{
		_cfg.mesh_alias = m_pReader->ReadStringParam(pMeshNode, "alias");
		m_pReader->CloseGroupNode(pMeshNode);
	}

	IOGGroupNode* pMaterialNode = m_pReader->OpenGroupNode(pSource, NULL, "Material");
	if (pMaterialNode != NULL)
	{
		IOGGroupNode* pTextureNode = m_pReader->OpenGroupNode(pSource, pMaterialNode, "Texture");
		for (; pTextureNode != NULL; )
		{
			Cfg::TextureCfg txcfg;
			txcfg.alias = m_pReader->ReadStringParam(pTextureNode, "alias");
			_cfg.texture_cfg_list.push_back(txcfg);
			pTextureNode = m_pReader->ReadNextNode(pTextureNode);
		}
		m_pReader->CloseGroupNode(pMaterialNode);
	}

	m_pReader->CloseSource(pSource);
	return true;
}


// Unload terrain.
void COGTerrain::Unload ()
{
	if (m_LoadState != OG_RESSTATE_LOADED)
	{
		return;
	}

	m_pMaterial = NULL;
	m_TextureList.clear();

	m_LoadState = OG_RESSTATE_DEFINED;
}


// Render terrain.
void COGTerrain::Render (const MATRIX& _mWorld, unsigned int _Frame)
{
    m_pRenderer->SetMaterial(m_pMaterial);
    m_pRenderer->SetTexture(m_TextureList[0]);
    unsigned int numParts = m_pMesh->GetNumRenderables();
    for (unsigned int i = 0; i < numParts; ++i)
    {
        m_pMesh->RenderPart (_mWorld, i, 0);
    }
}


// Render.
void COGTerrain::Render (const MATRIX& _mWorld, unsigned int _Part, unsigned int _Frame)
{
    m_pRenderer->SetMaterial(m_pMaterial);
    m_pRenderer->SetTexture(m_TextureList[_Part]);
	m_pMesh->RenderPart (_mWorld, _Part, 0);
}


// Update mesh animation.
void COGTerrain::Update (unsigned long _ElapsedTime)
{
}


// Get num renderable parts.
unsigned int COGTerrain::GetNumRenderables () const
{
    return m_pMesh->GetNumRenderables();
}


// Get ray intersection
bool COGTerrain::GetRayIntersection (const Vec3& _vRayPos, const Vec3& _vRayDir, Vec3* _pOutPos)
{
    if (m_pMesh)
    {
        return m_pMesh->GetRayIntersection(_vRayPos, _vRayDir, _pOutPos);
    }
    return false;
}


// Get combined AABB
const IOGAabb& COGTerrain::GetAABB () const
{
	return m_pMesh->GetAABB();
}