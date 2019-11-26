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
#ifndef GAMESCREENCONTROLLER_H_
#define GAMESCREENCONTROLLER_H_

#include "IOGScreenController.h"
#include "IOGResourceMgr.h"
#include "IOGSceneGraph.h"
#include "IOGLevelManager.h"
#include "IOGInputReceiver.h"
#include "IOGActorManager.h"
#include "IOGRenderer.h"
#include "IOGGlobalVarsTable.h"
#include "IOGSpritePool.h"
#include "IOGSettingsReader.h"


class CGameScreenController :   public IOGScreenController, 
                                public IOGInputReceiver,
                                public IOGGameEventsHandler
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
	virtual bool OnVectorChanged (const OGVec3& _vVec);

	// Touch event handler.
	virtual bool OnTouch (const OGVec2& _vPos, IOGTouchParam _param);

	// Level finish point handler
	virtual void OnReachedFinishPoint ();

	// Level finish handler
	virtual void OnLevelFinish ();

private:

    // Update camera.
	void UpdateCamera ();

private:
		
	IOGGlobalVarsTable* m_pGlobalVars;
	IOGSettingsReader*	m_pReader;
	IOGResourceMgr*		m_pResourceMgr;
	IOGSceneGraph*		m_pSg;
	IOGRenderer*		m_pRenderer;
	IOGCamera*			m_pCamera;
    IOGActor*			m_pPlayer;

    IOGGuiLifebar*      m_pLifeHUD;
    IOGGuiBonusbar*	    m_pSpecHUD;
    IOGGuiWeaponPanel*	m_pWeaponHUD;

	ControllerState	m_State;
    ScreenType      m_Type;
    IOGLevel*	    m_pCurLevel;
	OGMatrix			m_mProjection; 
	OGMatrix			m_mView;

	float			m_fFOV;
	float			m_fZNear;
	float			m_fZFar;
	int				m_ScrWidth;
	int				m_ScrHeight;
	float			m_fCameraTargetDistance;
	float			m_fCameraMargins;
	OGVec3			m_vCameraDir;
	OGVec3			m_vCameraOffset;

	unsigned long	m_ElapsedTime;
    bool            m_bFinishLine;
};

#endif
