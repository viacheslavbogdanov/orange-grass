/*
 *  GameScreenController.h
 *  OrangeGrass
 *
 *  Created by Viacheslav Bogdanov on 11.11.09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef GAMESCREENCONTROLLER_H_
#define GAMESCREENCONTROLLER_H_

#include "IScreenController.h"
#include "IOGResourceMgr.h"
#include "IOGSceneGraph.h"
#include "IOGLevelManager.h"
#include "IOGInputReceiver.h"
#include "IOGActorManager.h"
#include "IOGRenderer.h"


class CGameScreenController : public IScreenController, public IOGInputReceiver
{
public:
	CGameScreenController();
	virtual ~CGameScreenController();
		
	// Initialize controller
	virtual bool Init ();
		
	// Get controller state
	virtual ControllerState GetState () const { return m_State; }

	// Get controller type
    virtual ScreenType GetType () const { return m_Type; }

	// Update controller
	virtual void Update (unsigned long _ElapsedTime);
		
	// Render controller scene
	virtual void RenderScene ();
		
	// Activate
	virtual void Activate ();
		
	// deactivate
	virtual void Deactivate ();

    // Control vector change event handler.
	virtual void OnVectorChanged (const Vec3& _vVec);

	// Touch event handler.
	virtual void OnTouch (const Vec2& _vPos);

private:

    // Check if finish condition is satisfied.
	bool CheckFinishCondition ();

    // Update camera.
	void UpdateCamera ();

private:
		
	IOGResourceMgr*	m_pResourceMgr;
	IOGSceneGraph*	m_pSg;
	IOGRenderer*	m_pRenderer;
	IOGCamera*		m_pCamera;
    IOGActor*       m_pPlayer;

	ControllerState	m_State;
    ScreenType      m_Type;
    IOGLevel*	    m_pCurLevel;
	MATRIX			m_mProjection; 
	MATRIX			m_mView;

	float			m_fFOV;
	float			m_fCameraTargetDistance;
	float			m_fCameraFwdSpeed;
	float			m_fCameraStrafeSpeed;
	float			m_fFinishPointSqDistance;

	unsigned long	m_ElapsedTime;
};

#endif