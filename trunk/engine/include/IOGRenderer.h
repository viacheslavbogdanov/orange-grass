/*
 *  IOGRenderer.h
 *  OrangeGrass
 *
 *  Created by Viacheslav Bogdanov on 07.11.09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef IOGRENDERER_H_
#define IOGRENDERER_H_

#include "IOGTexture.h"
#include "IOGMaterial.h"
#include "IOGLight.h"
#include "IOGCamera.h"
#include "IOGFog.h"
#include "IOGVertexBuffers.h"


enum OGRenderMode
{
	OG_RENDERMODE_GEOMETRY,
	OG_RENDERMODE_EFFECTS,
	OG_RENDERMODE_SPRITES,
	OG_RENDERMODE_SHADOWMAP,
	OG_RENDERMODE_TEXT
};


class IOGRenderer
{
public:
	virtual ~IOGRenderer () {}
	
	// initialize renderer.
	virtual bool Init () = 0;
	
	// set viewport.
	virtual void SetViewport (
		unsigned int _Width, 
		unsigned int _Height,
		float _fZNear,
		float _fZFar,
		float _fFOV	) = 0;

	// add rendering command.
	virtual void SetTexture (IOGTexture* _pTexture) = 0;

	// add rendering command.
	virtual void SetMaterial (IOGMaterial* _pMaterial) = 0;

	// set model matrix.
	virtual void SetModelMatrix (const MATRIX& _mModel) = 0;

	// set view matrix.
	virtual void SetViewMatrix (const MATRIX& _mView) = 0;

	// add rendering command.
	virtual void RenderMesh (void* _pMesh) = 0;

	// clear frame buffer with the given color
	virtual void ClearFrame (const Vec4& _vClearColor) = 0;

	// Get scene light.
	virtual IOGLight* GetLight () = 0;

	// Create vertex buffer for mesh.
	virtual IOGVertexBuffers* CreateVertexBuffer (const void* _pMeshData) = 0;

	// Enable scene light.
	virtual void EnableLight (bool _bEnable) = 0;

	// Get main camera.
	virtual IOGCamera* GetCamera () = 0;

	// Get fog.
	virtual IOGFog* GetFog () = 0;

	// Enable scene fog.
	virtual void EnableFog (bool _bEnable) = 0;

	// start rendering mode.
	virtual void StartRenderMode(OGRenderMode _Mode) = 0;

	// finish rendering mode.
	virtual void FinishRenderMode() = 0;
	
	// reset renderer pipeline.
	virtual void Reset () = 0;

    // Unproject screen coords.
    virtual Vec3 UnprojectCoords (int _X, int _Y) = 0;

    // Display string.
    virtual void DisplayString (const Vec2& _vPos, float _fScale, unsigned int Colour, const char * const pszFormat, ...) = 0;

    // Draw effects buffer.
    virtual void DrawEffectBuffer (void* _pBuffer, int _StartId, int _NumVertices) = 0;

    // Draw sprite buffer.
    virtual void DrawSpriteBuffer (void* _pBuffer, int _StartId, int _NumVertices) = 0;
};

#endif