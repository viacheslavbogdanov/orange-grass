/*
 *  ogphysicalobject.cpp
 *  OrangeGrass
 *
 *  Created by Viacheslav Bogdanov on 12.11.09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#include "OrangeGrass.h"
#include "ogphysicalobject.h"


COGPhysicalObject::COGPhysicalObject () :	m_Type (OG_PHYSICS_NONE),
											m_bUpdated(false)
{
    m_vScaling = Vec3(1);
}


COGPhysicalObject::~COGPhysicalObject ()
{
}


// get world transform.
const MATRIX& COGPhysicalObject::GetWorldTransform () const
{
	return m_mWorld;
}


// set world transform.
void COGPhysicalObject::SetWorldTransform (
	const Vec3& _vPos, 
	const Vec3& _vRot, 
	const Vec3& _vScale)
{
    m_vPosition = _vPos;
    m_vRotation = _vRot;
    m_vScaling = _vScale;

	m_bUpdated = false;
}


// get position.
const Vec3& COGPhysicalObject::GetPosition () const
{
	return m_vPosition;
}


// get rotation.
const Vec3& COGPhysicalObject::GetRotation () const
{
	return m_vRotation;
}


// get scaling.
const Vec3& COGPhysicalObject::GetScaling () const
{
	return m_vScaling;
}


// set position.
void COGPhysicalObject::SetPosition (const Vec3& _vPos)
{
	m_vPosition = _vPos;
	m_bUpdated = false;
}


// set rotation.
void COGPhysicalObject::SetRotation (const Vec3& _vRot)
{
	m_vRotation = _vRot;
	m_bUpdated = false;
}


// set scaling.
void COGPhysicalObject::SetScaling (const Vec3& _vScale)
{
	m_vScaling = _vScale;
	m_bUpdated = false;
}


// strafe.
void COGPhysicalObject::Strafe (float _fDir)
{
	m_bUpdated = false;
}


// get physics type.
OGPhysicsType COGPhysicalObject::GetPhysicsType () const
{
	return m_Type;
}


// get OBB.
const IOGObb& COGPhysicalObject::GetOBB () const
{
    return m_Obb;
}


// Update transforms.
void COGPhysicalObject::Update (unsigned long _ElapsedTime)
{
	if (m_bUpdated)
		return;

	WorldMatrixFromTransforms(m_mWorld, m_vPosition, m_vRotation, m_vScaling);
	m_Obb.UpdateTransform(m_mWorld);
	m_bUpdated = true;
}