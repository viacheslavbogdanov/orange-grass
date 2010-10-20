/*
 *  ogmodel.mm
 *  OrangeGrass
 *
 *  Created by Viacheslav Bogdanov on 08.11.09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "OrangeGrass.h"
#include "ogmodel.h"


COGModel::COGModel() :	m_pMesh(NULL),
						m_pTexture(NULL),
                        m_pMaterial(NULL)
{
    m_pRenderer = GetRenderer();
	m_pReader = GetSettingsReader();
}


COGModel::~COGModel()
{
	m_pMesh = NULL;
	m_pTexture = NULL;	
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
	}

	Cfg modelcfg;
	if (!LoadConfig(modelcfg))
	{
        OG_LOG_ERROR("Failed to load model from config");
		return false;
	}

	m_pMesh = GetResourceMgr()->GetMesh(modelcfg.mesh_alias);
	m_pTexture = GetResourceMgr()->GetTexture(modelcfg.texture_alias);
	m_pMaterial = GetMaterialManager()->GetMaterial(modelcfg.material_type);

	std::list<Cfg::Anim>::const_iterator anim_iter = modelcfg.anim_list.begin();
	for (; anim_iter != modelcfg.anim_list.end(); ++anim_iter)
    {
		const COGModel::Cfg::Anim& anim = (*anim_iter);
        IOGAnimation* pAnim = new IOGAnimation();
        pAnim->name = anim.anim_alias;
        pAnim->start_frame = (unsigned int)anim.anim_start;
        pAnim->end_frame = (unsigned int)anim.anim_end;
        m_pAnimations[pAnim->name] = pAnim;
    }

	std::list<Cfg::ActPoint>::const_iterator pt_iter = modelcfg.point_list.begin();
	for (; pt_iter != modelcfg.point_list.end(); ++pt_iter)
    {
		const COGModel::Cfg::ActPoint& pt = (*pt_iter);
        IOGActivePoint* pPt = new IOGActivePoint();
		pPt->alias = pt.alias;
		pPt->pos = pt.pos;
        m_pActivePoints[pPt->alias] = pPt;
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
		_cfg.mesh_alias = m_pReader->ReadStringParam(pMeshNode, "alias");
		m_pReader->CloseGroupNode(pMeshNode);
	}

	IOGGroupNode* pMaterialNode = m_pReader->OpenGroupNode(pSource, NULL, "Material");
	if (pMaterialNode != NULL)
	{
		_cfg.texture_alias = m_pReader->ReadStringParam(pMaterialNode, "texture");
		_cfg.material_type = m_pReader->ReadStringParam(pMaterialNode, "type");
		m_pReader->CloseGroupNode(pMaterialNode);
	}

	IOGGroupNode* pAnimationNode = m_pReader->OpenGroupNode(pSource, NULL, "Animation");
	while (pAnimationNode != NULL)
	{
		COGModel::Cfg::Anim anim;
		anim.anim_alias = m_pReader->ReadStringParam(pAnimationNode, "name");
		anim.anim_start = m_pReader->ReadIntParam(pAnimationNode, "start_frame");
		anim.anim_end = m_pReader->ReadIntParam(pAnimationNode, "end_frame");
		_cfg.anim_list.push_back(anim);
		pAnimationNode = m_pReader->ReadNextNode(pAnimationNode);
	}

	IOGGroupNode* pPointNode = m_pReader->OpenGroupNode(pSource, NULL, "ActivePoints");
	while (pPointNode != NULL)
	{
		COGModel::Cfg::ActPoint pt;
		pt.alias = m_pReader->ReadStringParam(pPointNode, "alias");
		pt.pos = m_pReader->ReadVec3Param(pPointNode, "x", "y", "z");
		_cfg.point_list.push_back(pt);
		pPointNode = m_pReader->ReadNextNode(pPointNode);
	}

	m_pReader->CloseSource(pSource);
	return true;
}


// Unload resource.
void COGModel::Unload ()
{
	if (m_LoadState != OG_RESSTATE_LOADED)
	{
		return;
	}

	OG_SAFE_DELETE(m_pMaterial);

    std::map<std::string, IOGAnimation*>::iterator iter= m_pAnimations.begin();
	for (; iter != m_pAnimations.end(); ++iter)
	{
		OG_SAFE_DELETE(iter->second);
	}
    m_pAnimations.clear();

    std::map<std::string, IOGActivePoint*>::iterator point_iter= m_pActivePoints.begin();
	for (; point_iter != m_pActivePoints.end(); ++point_iter)
	{
		OG_SAFE_DELETE(point_iter->second);
	}
    m_pActivePoints.clear();

	m_LoadState = OG_RESSTATE_DEFINED;
}


// Update.
void COGModel::Update (unsigned long _ElapsedTime)
{
}


// Render mesh.
void COGModel::Render (const MATRIX& _mWorld, unsigned int _Frame)
{
    m_pRenderer->SetMaterial(m_pMaterial);
    m_pRenderer->SetTexture(m_pTexture);
	m_pMesh->Render (_mWorld, _Frame);
}

		
// Render.
void COGModel::Render (const MATRIX& _mWorld, unsigned int _Part, unsigned int _Frame)
{
    m_pRenderer->SetMaterial(m_pMaterial);
    m_pRenderer->SetTexture(m_pTexture);
	m_pMesh->RenderPart (_mWorld, _Part, _Frame);
}


// Get num renderable parts.
unsigned int COGModel::GetNumRenderables () const
{
    return m_pMesh->GetNumRenderables();
}


// Get combined AABB
const IOGAabb& COGModel::GetAABB () const
{
	return m_pMesh->GetAABB();
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
IOGActivePoint* COGModel::GetActivePoint (const std::string& _Alias)
{
	return m_pActivePoints[_Alias];
}
