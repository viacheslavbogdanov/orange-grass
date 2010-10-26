/*
 *  IOGActor.h
 *  OrangeGrass
 *
 *  Created by Viacheslav Bogdanov on 11.11.09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef IOGACTOR_H_
#define IOGACTOR_H_

#include "IOGSgNode.h"
#include "IOGActorParams.h"
#include "IOGPhysicalObject.h"
#include "IOGWeapon.h"


enum OGActorStatus
{
	OG_ACTORSTATUS_ALIVE,
	OG_ACTORSTATUS_FALLING,
	OG_ACTORSTATUS_DEAD
};


class IOGActor
{
public:
	virtual ~IOGActor() {}

	// Update actor.
	virtual void Update (unsigned long _ElapsedTime) = 0;

	// Update actors in editor.
	virtual void UpdateEditor (unsigned long _ElapsedTime) = 0;

	// Adding to actor manager event handler.
	virtual void OnAddedToManager () = 0;

	// Get actor type.
	virtual OGActorType GetType () const = 0;

	// Get scene graph node.
	virtual IOGSgNode* GetSgNode () = 0;

	// Get physical object.
	virtual IOGPhysicalObject* GetPhysicalObject () = 0;

	// Get actor params.
	virtual IOGActorParams* GetParams () = 0;

	// Get model alias
	virtual const std::string& GetAlias () const = 0;

	// Set weapon parameters.
	virtual void SetWeapon (IOGWeaponParams* _pWeaponParams) = 0;

	// Get weapon
	virtual IOGWeapon* GetWeapon () = 0;

	// Get active state
	virtual bool IsActive () const = 0;

	// Set active state
	virtual void Activate (bool _bActive) = 0;

	// Check actor's OBB intersection with ray.
	virtual bool CheckIntersection (
        const Vec3& _vRayStart,
        const Vec3& _vRayDir ) const = 0;
};


#endif
