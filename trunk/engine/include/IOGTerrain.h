/*
 *  IOGTerrain.h
 *  OrangeGrass
 *
 *  Created by Viacheslav Bogdanov on 12.11.09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef IOGTERRAIN_H_
#define IOGTERRAIN_H_

#include "IOGRenderable.h"
#include "IOGTexture.h"
#include "IOGMesh.h"


class IOGTerrain
{
public:
	virtual ~IOGTerrain() {}

	// Render terrain.
	virtual void Render (const MATRIX& _mWorld) = 0;

	// Render all.
	virtual void RenderAll (const MATRIX& _mWorld) = 0;

    // Get ray intersection
    virtual bool GetRayIntersection (const Vec3& _vRayPos, const Vec3& _vRayDir, Vec3* _pOutPos) = 0;

    // Get mesh geometry
    virtual const std::vector<OGFace>& GetGeometry () const = 0;

	// Get combined AABB
	virtual const IOGAabb& GetAABB () const = 0;
};


#endif