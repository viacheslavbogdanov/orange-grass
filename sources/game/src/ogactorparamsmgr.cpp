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
#include "Game.h"
#include "ogactorparamsmgr.h"


COGActorParamsMgr::COGActorParamsMgr ()
{
	m_pReader = GetSettingsReader();
}


COGActorParamsMgr::~COGActorParamsMgr ()
{
	std::map<std::string, IOGActorParams*>::iterator iter = m_ParamsList.begin();
	for( ; iter != m_ParamsList.end(); ++iter )
	{
		OG_SAFE_DELETE (iter->second);
	}	

	std::map<std::string, IOGWeaponParams*>::iterator witer = m_WeaponParamsList.begin();
	for( ; witer != m_WeaponParamsList.end(); ++witer )
	{
		OG_SAFE_DELETE (witer->second);
	}	

	std::map<std::string, IOGBonusParams*>::iterator biter = m_BonusParamsList.begin();
	for( ; biter != m_BonusParamsList.end(); ++biter )
	{
		OG_SAFE_DELETE (biter->second);
	}	
}


// load from config file.
bool COGActorParamsMgr::Init ()
{
	if (!InitActorParams())
		return false;

	if (!InitWeaponParams())
		return false;

	if (!InitBonusParams())
		return false;

	return true;
}


// get params by alias.
IOGActorParams* COGActorParamsMgr::GetParams (const std::string& _Alias)
{
	IOGActorParams* pParam = m_ParamsList[_Alias];
	if (pParam)
	{
		return pParam;
	}
    return NULL;
}


// get weapon params by alias.
IOGWeaponParams* COGActorParamsMgr::GetWeaponParams (const std::string& _Alias)
{
	IOGWeaponParams* pParam = m_WeaponParamsList[_Alias];
	if (pParam)
	{
		return pParam;
	}
    return NULL;
}


// get bonus params by alias.
IOGBonusParams* COGActorParamsMgr::GetBonusParams (const std::string& _Alias)
{
	IOGBonusParams* pParam = m_BonusParamsList[_Alias];
	if (pParam)
	{
		return pParam;
	}
    return NULL;
}


// load from config file.
bool COGActorParamsMgr::InitBonusParams ()
{
	IOGSettingsSource* pSource = m_pReader->OpenSource(GetResourceMgr()->GetFullPath("bonus.xml"));
	if (!pSource)
		return false;

	IOGGroupNode* pRoot = m_pReader->OpenGroupNode(pSource, NULL, "Bonuses");
	IOGGroupNode* pBonusNode = m_pReader->OpenGroupNode(pSource, pRoot, "Bonus");
	for (; pBonusNode != NULL; )
	{
		std::string alias = m_pReader->ReadStringParam(pBonusNode, "alias");
		std::string file = m_pReader->ReadStringParam(pBonusNode, "file");
		file = GetResourceMgr()->GetFullPath(file);
		LoadBonusParamsConfig(alias, file);

		pBonusNode = m_pReader->ReadNextNode(pBonusNode);
	}
	m_pReader->CloseGroupNode(pRoot);
	m_pReader->CloseSource(pSource);
	return true;
}


// load from config file.
bool COGActorParamsMgr::InitWeaponParams ()
{
	IOGSettingsSource* pSource = m_pReader->OpenSource(GetResourceMgr()->GetFullPath("weapon.xml"));
	if (!pSource)
		return false;

	IOGGroupNode* pRoot = m_pReader->OpenGroupNode(pSource, NULL, "Weapons");
	IOGGroupNode* pWeaponNode = m_pReader->OpenGroupNode(pSource, pRoot, "Weapon");
	for (; pWeaponNode != NULL; )
	{
		std::string alias = m_pReader->ReadStringParam(pWeaponNode, "alias");
		std::string file = m_pReader->ReadStringParam(pWeaponNode, "file");
		file = GetResourceMgr()->GetFullPath(file);
		LoadWeaponParamsConfig(alias, file);

		pWeaponNode = m_pReader->ReadNextNode(pWeaponNode);
	}
	m_pReader->CloseGroupNode(pRoot);
	m_pReader->CloseSource(pSource);
	return true;
}


// load from config file.
bool COGActorParamsMgr::InitActorParams ()
{
	IOGSettingsSource* pSource = m_pReader->OpenSource(GetResourceMgr()->GetFullPath("actors.xml"));
	if (!pSource)
		return false;

	IOGGroupNode* pRoot = m_pReader->OpenGroupNode(pSource, NULL, "Actors");
	IOGGroupNode* pActorNode = m_pReader->OpenGroupNode(pSource, pRoot, "Actor");
	for (; pActorNode != NULL; )
	{
		std::string alias = m_pReader->ReadStringParam(pActorNode, "alias");
		std::string file = m_pReader->ReadStringParam(pActorNode, "file");
		file = GetResourceMgr()->GetFullPath(file);
		LoadParamsConfig(alias, file);

		pActorNode = m_pReader->ReadNextNode(pActorNode);
	}
	m_pReader->CloseGroupNode(pRoot);
	m_pReader->CloseSource(pSource);
	return true;
}


// Load actor params configuration
bool COGActorParamsMgr::LoadParamsConfig (const std::string& _Alias, const std::string& _Path)
{
	IOGSettingsSource* pSource = m_pReader->OpenSource(_Path);
	if (!pSource)
		return false;

	IOGActorParams* pParam = new IOGActorParams;
	pParam->alias = _Alias;

	IOGGroupNode* pRoot = m_pReader->OpenGroupNode(pSource, NULL, "Params");
	IOGGroupNode* pPhysicsNode = m_pReader->OpenGroupNode(pSource, pRoot, "Physics");
	if (pPhysicsNode != NULL)
	{
		pParam->physics.fMaxSpeed = m_pReader->ReadFloatParam(pPhysicsNode, "max_speed");
		pParam->physics.fAcceleration = m_pReader->ReadFloatParam(pPhysicsNode, "acceleration");
        pParam->physics.fStrafeMaxSpeed = m_pReader->ReadFloatParam(pPhysicsNode, "max_strafe_speed");
        pParam->physics.fRollSpeed = m_pReader->ReadFloatParam(pPhysicsNode, "roll_speed");
        pParam->physics.fMaxRollAngle = m_pReader->ReadFloatParam(pPhysicsNode, "max_roll_angle");
        pParam->physics.fPitchSpeed = m_pReader->ReadFloatParam(pPhysicsNode, "pitch_speed");
        pParam->physics.fMaxPitchAngle = m_pReader->ReadFloatParam(pPhysicsNode, "max_pitch_angle");
		pParam->physics.fTorque = m_pReader->ReadFloatParam(pPhysicsNode, "torque");
        pParam->physics.type = ParsePhysicsType(m_pReader->ReadStringParam(pPhysicsNode, "type"));
		m_pReader->CloseGroupNode(pPhysicsNode);
	}

	IOGGroupNode* pActorNode = m_pReader->OpenGroupNode(pSource, pRoot, "Actor");
	if (pActorNode != NULL)
	{
		pParam->model_alias = m_pReader->ReadStringParam(pActorNode, "model");
		pParam->model_destruction = m_pReader->ReadStringParam(pActorNode, "model_destruction");
		pParam->type = ParseActorType(m_pReader->ReadStringParam(pActorNode, "type"));
		pParam->icon = m_pReader->ReadStringParam(pActorNode, "icon");
		m_pReader->CloseGroupNode(pActorNode);
	}

	IOGGroupNode* pLogicNode = m_pReader->OpenGroupNode(pSource, pRoot, "Logic");
	if (pLogicNode != NULL)
	{
		pParam->gameplay.team = (OGTeam)m_pReader->ReadIntParam(pLogicNode, "team");
		pParam->gameplay.max_hitpoints = m_pReader->ReadIntParam(pLogicNode, "max_hitpoints");
		pParam->gameplay.hitpoints = pParam->gameplay.max_hitpoints;
        pParam->gameplay.weapon = m_pReader->ReadStringParam(pLogicNode, "weapon");
		m_pReader->CloseGroupNode(pLogicNode);
	}

	m_ParamsList[_Alias] = pParam;

	m_pReader->CloseGroupNode(pRoot);
	m_pReader->CloseSource(pSource);

	return true;
}


// Load weapon params configuration
bool COGActorParamsMgr::LoadWeaponParamsConfig (const std::string& _Alias, const std::string& _Path)
{
	IOGSettingsSource* pSource = m_pReader->OpenSource(_Path);
	if (!pSource)
		return false;

	IOGWeaponParams* pParam = new IOGWeaponParams;
	pParam->alias = _Alias;

	IOGGroupNode* pRoot = m_pReader->OpenGroupNode(pSource, NULL, "Weapon");
	IOGGroupNode* pNode = m_pReader->OpenGroupNode(pSource, pRoot, "Params");
	if (pNode != NULL)
	{
		pParam->actor = m_pReader->ReadStringParam(pNode, "actor");
		pParam->icon_texture = m_pReader->ReadStringParam(pNode, "icon_texture");
		pParam->hitpoints = (unsigned int)m_pReader->ReadIntParam(pNode, "hitpoints");
		pParam->cooldown = (unsigned int)m_pReader->ReadIntParam(pNode, "cooldown");
		pParam->pos = ParseWeaponPositionType(m_pReader->ReadStringParam(pNode, "position"));
		m_pReader->CloseGroupNode(pNode);
	}

	m_WeaponParamsList[_Alias] = pParam;

	m_pReader->CloseGroupNode(pRoot);
	m_pReader->CloseSource(pSource);

	return true;
}


// Load bonus params configuration
bool COGActorParamsMgr::LoadBonusParamsConfig (const std::string& _Alias, const std::string& _Path)
{
	IOGSettingsSource* pSource = m_pReader->OpenSource(_Path);
	if (!pSource)
		return false;

	IOGBonusParams* pParam = new IOGBonusParams;
	pParam->alias = _Alias;

	IOGGroupNode* pRoot = m_pReader->OpenGroupNode(pSource, NULL, "Bonus");
	IOGGroupNode* pNode = m_pReader->OpenGroupNode(pSource, pRoot, "Params");
	if (pNode != NULL)
	{
		pParam->actor = m_pReader->ReadStringParam(pNode, "actor");
		pParam->icon_texture = m_pReader->ReadStringParam(pNode, "icon_texture");
		pParam->value = (unsigned int)m_pReader->ReadIntParam(pNode, "value");
		pParam->cooldown = (unsigned int)m_pReader->ReadIntParam(pNode, "cooldown");
		pParam->type = ParseBonusType(m_pReader->ReadStringParam(pNode, "type"));
		m_pReader->CloseGroupNode(pNode);
	}

	m_BonusParamsList[_Alias] = pParam;

	m_pReader->CloseGroupNode(pRoot);
	m_pReader->CloseSource(pSource);

	return true;
}


// get params list.
void COGActorParamsMgr::GetParamsList (std::list<IOGActorParams*>& _List)
{
	_List.clear();
	std::map<std::string, IOGActorParams*>::iterator iter = m_ParamsList.begin();
	for( ; iter != m_ParamsList.end(); ++iter )
	{
		_List.push_back(iter->second);
	}	
}
