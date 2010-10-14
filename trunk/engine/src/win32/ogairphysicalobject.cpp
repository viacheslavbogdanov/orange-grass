/*
 *  OGAirPhysicalObject.h
 *  OrangeGrass
 *
 *  Created by Viacheslav Bogdanov on 12.11.09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#include "OrangeGrass.h"
#include "ogairphysicalobject.h"


COGAirPhysicalObject::COGAirPhysicalObject ()
{
}


COGAirPhysicalObject::~COGAirPhysicalObject ()
{
}


// create object
void COGAirPhysicalObject::Create (const IOGAabb& _Aabb, 
                                   IOGPhysicalParams* _pParams,
                                   void* _pParentActor)
{
    m_pParams = _pParams;
	m_Type = OG_PHYSICS_AIRBOT;
	m_Aabb = _Aabb;
    m_pActor = (IOGActor*)_pParentActor;
    m_Obb.Create(m_Aabb);
}
