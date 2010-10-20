/*
 *  OGActorAirbot.h
 *  OrangeGrass
 *
 *  Created by Viacheslav Bogdanov on 11.11.09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef OGACTORAIRBOT_H_
#define OGACTORAIRBOT_H_

#include "ogactorbot.h"
#include "ogbotflightworker.h"
#include "ogbotfallingworker.h"


class COGActorAirBot : public COGActorBot
{
public:
	COGActorAirBot();
	virtual ~COGActorAirBot();

	// Create actor.
	virtual bool Create (
		IOGActorParams* _pParams,
		const Vec3& _vPos,
		const Vec3& _vRot,
        const Vec3& _vScale);

	// Adding to actor manager event handler.
	virtual void OnAddedToManager ();

	// Set active state
	virtual void Activate (bool _bActive);

	// Update actor.
	virtual void Update (unsigned long _ElapsedTime);

    // collision event handler
	virtual bool OnCollision (const IOGCollision& _Collision);

protected:

    COGBotFlightWorker		m_FlightWorker;
    COGBotFallingWorker		m_FallingWorker;
};


#endif