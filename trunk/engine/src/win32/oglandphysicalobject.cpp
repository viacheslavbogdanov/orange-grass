/*
 *  IOGLandPhysicalObject.h
 *  OrangeGrass
 *
 *  Created by Viacheslav Bogdanov on 12.11.09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#include "OrangeGrass.h"
#include "oglandphysicalobject.h"


COGLandPhysicalObject::COGLandPhysicalObject () : COGPhysicalObject()
{
}


COGLandPhysicalObject::~COGLandPhysicalObject ()
{
}


// create object
void COGLandPhysicalObject::Create (const IOGAabb& _Aabb, const IOGPhysicalParams& _Params)
{
    m_Params = _Params;
	m_Type = OG_PHYSICS_LANDBOT;
	m_Aabb = _Aabb;
    m_Obb.Create(m_Aabb);
}
