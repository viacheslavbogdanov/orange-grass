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
#ifndef OGPHYSICALOBJECT_H_
#define OGPHYSICALOBJECT_H_

#include "IOGPhysicalObject.h"
#include "IOGActor.h"


class COGPhysicalObject : public IOGPhysicalObject
{
public:
	COGPhysicalObject ();	
	virtual ~COGPhysicalObject ();

	// create object
	virtual void Create (
        const IOGAabb& _Aabb, 
        IOGPhysicalParams* _pParams, 
        void* _pParentActor) = 0;

    // add collision handler.
    virtual void AddCollisionListener (IOGCollisionListener* _pListener);

    // check collision.
    virtual bool CheckCollision (IOGPhysicalObject* _pObject);

    // respond on a collision.
    virtual bool RespondOnCollision (const IOGCollision& _Collision);

	// Set active state.
	virtual void Activate (bool _bActive);

	// get world transform.
	virtual const OGMatrix& GetWorldTransform () const;

	// set world transform.
	virtual void SetWorldTransform (
		const OGVec3& _vPos, 
		const OGVec3& _vRot, 
		const OGVec3& _vScale);

	// get position.
	virtual const OGVec3& GetPosition () const;

	// get rotation.
	virtual const OGVec3& GetRotation () const;

	// get scaling.
	virtual const OGVec3& GetScaling () const;

	// get direction.
	virtual const OGVec3& GetDirection () const;

	// set position.
	virtual void SetPosition (const OGVec3& _vPos);

	// set rotation.
	virtual void SetRotation (const OGVec3& _vRot);

	// set scaling.
	virtual void SetScaling (const OGVec3& _vScale);

	// strafe.
	virtual void Strafe (float _fDir);

	// accelerate.
	virtual void Accelerate (float _fDir);

	// move.
	virtual void Move (const OGVec3& _vDir);

	// fall.
	virtual void Fall () {}

	// orient on point.
	virtual bool Orient (const OGVec3& _vPoint);

	// strabilize object.
	virtual bool Stabilize ();

	// get physics type.
	virtual OGPhysicsType GetPhysicsType () const;

	// get actor.
	virtual void* GetActor () {return m_pActor;}

	// get OBB.
	virtual const IOGObb& GetOBB () const;

	// Update transforms.
	virtual void Update (unsigned long _ElapsedTime);

protected:

	// Update directions.
	virtual void UpdateDirections ();

	// Bound object position to be in level space.
	virtual bool BoundPosition ();

	// Stabilize rotation.
	virtual bool StabilizeRotation ();

protected:

	OGPhysicsType	m_Type;
    OGMatrix          m_mWorld;

	OGVec3            m_vPosition;
    OGVec3            m_vRotation;
    OGVec3            m_vScaling;
    OGVec3            m_vPrevPosition;

	OGVec3            m_vUp;
    OGVec3            m_vLook;
    OGVec3            m_vRight;

	OGVec3            m_vMove;
	OGVec3            m_vAcceleration;
    OGVec3            m_vTorque;
    float			m_fStrafe;

	IOGAabb			m_Aabb;
	IOGObb			m_Obb;

	IOGPhysicalParams*		m_pParams;
    IOGCollisionListener*   m_pListener;
    IOGActor*				m_pActor;

	bool			m_bUpdated;
	bool			m_bActive;
};

#endif
