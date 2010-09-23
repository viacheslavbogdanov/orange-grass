/*
 *  OGActorLandbot.h
 *  OrangeGrass
 *
 *  Created by Viacheslav Bogdanov on 11.11.09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef OGACTORLANDBOT_H_
#define OGACTORLANDBOT_H_

#include "OGActor.h"


class COGActorLandBot : public COGActor
{
public:
	COGActorLandBot();
	virtual ~COGActorLandBot();

	// Create actor.
	virtual bool Create (
		IOGActorParams* _pParams,
		const Vec3& _vPos,
		const Vec3& _vRot,
        const Vec3& _vScale);
};


#endif
