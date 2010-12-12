/*
 *  OGEmitterRotatingSparks.h
 *  OrangeGrass
 *
 *  Created by Viacheslav Bogdanov on 11.11.09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef OGEMITTERROTATINGSPARKS_H_
#define OGEMITTERROTATINGSPARKS_H_

#include "ogemitter.h"
#include <vector>


class COGEmitterRotatingSparks : public COGEmitter
{
public:
	COGEmitterRotatingSparks();
	virtual ~COGEmitterRotatingSparks();

	// Initialize emitter.
	virtual void Init(IOGGroupNode* _pNode);

	// Update.
	virtual void Update (unsigned long _ElapsedTime);

	// Render.
	virtual void Render (const MATRIX& _mWorld, const Vec3& _vLook, const Vec3& _vUp, const Vec3& _vRight);

	// Start.
	virtual void Start ();

	// Stop.
	virtual void Stop ();

protected:

    struct ParticleFormat
    {
        bool    bDirty;
	    float	scale;
        float   angle;
        float   tilt;
        int     axis;
	    Vec3	offset;
	    BBVert	pVertices[4];
    };

protected:

	std::vector<ParticleFormat>	m_BBList;

public:

    std::string     m_Texture;
    unsigned int    m_MappingId;
	unsigned int	m_NumParticles;
	float			m_fAlphaInc;
	float           m_fScaleInc;
	float           m_fInitialScale;
	Vec4			m_color;
};


#endif