/*
 *  GameScreenController.mm
 *  OrangeGrass
 *
 *  Created by Viacheslav Bogdanov on 11.11.09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#include "OpenGL2.h"
#include "GameScreenController.h"
#include "OrangeGrass.h"
#include "common.h"


CGameScreenController::CGameScreenController() :	m_pResourceMgr(NULL),
													m_pSg(NULL),
													m_pCamera(NULL),
                                                    m_pPlayer(NULL),
													m_State(CSTATE_NO),
                                                    m_Type(SCRTYPE_GAME),
													m_pCurLevel(NULL),
													m_fFOV(1.0f),
													m_fCameraTargetDistance(150.0f),
													m_fCameraFwdSpeed(0.02f),
													m_fCameraStrafeSpeed(0.02f)
{
}


CGameScreenController::~CGameScreenController()
{
	m_pResourceMgr = NULL;
	m_pSg = NULL;
	m_pCamera = NULL;
    m_pPlayer = NULL;

	m_State = CSTATE_NO;
	m_pCurLevel = NULL;
}


// Initialize controller
bool CGameScreenController::Init ()
{
	m_pResourceMgr = GetResourceMgr();
	m_pSg = GetSceneGraph();
	m_pCamera = m_pSg->GetCamera();

	m_fFOV = 0.67f;
	m_fCameraTargetDistance = 60.0f;
	m_fCameraFwdSpeed = 0.02f;
	m_fCameraStrafeSpeed = 0.01f;
	m_fFinishPointSqDistance = 10000.0f;

	GetPhysics()->SetCameraFwdSpeed(m_fCameraFwdSpeed);
	GetPhysics()->SetCameraStrafeSpeed(m_fCameraStrafeSpeed);

    m_pCurLevel = GetLevelManager()->LoadLevel(std::string("level_0"));
    m_pSg->GetLight()->Apply();
    m_pPlayer = GetActorManager()->GetPlayersActor();

#ifdef WIN32
    MatrixPerspectiveFovRH(m_mProjection, m_fFOV, float(SCR_WIDTH)/float(SCR_HEIGHT), 4.0f, 200.0f, false);
#else
    MatrixPerspectiveFovRH(m_mProjection, m_fFOV, float(SCR_HEIGHT)/float(SCR_WIDTH), 4.0f, 200.0f, true);
#endif

	UpdateCamera();
	GetPhysics()->UpdateAll(1);

    glClearColor(0.3f, 0.3f, 0.4f, 1.0f);
    glEnable(GL_TEXTURE_2D);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    return true;
}


// Update controller
void CGameScreenController::Update (unsigned long _ElapsedTime)
{
	if (m_State != CSTATE_ACTIVE)
		return;
    
	UpdateCamera();

    GetPhysics()->Update(_ElapsedTime);
    GetActorManager()->Update(_ElapsedTime);

	if (CheckFinishCondition())
	{
		Deactivate();
	}
}


// Render controller scene
void CGameScreenController::RenderScene ()
{
	if (m_State != CSTATE_ACTIVE)
    {
		return;
    }
    
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(m_mProjection.f);

    glMatrixMode(GL_MODELVIEW);
    m_mView = m_pCamera->Update();
    glLoadMatrixf(m_mView.f);

    m_pSg->GetLight()->Apply();

	GLfloat fogColor[4]= {0.8f, 0.9f, 0.5f, 1.0f};

	glFogi(GL_FOG_MODE, GL_LINEAR);
	glFogfv(GL_FOG_COLOR, fogColor);
	glFogf(GL_FOG_DENSITY, 0.15f);
	glHint(GL_FOG_HINT, GL_DONT_CARE);
	glFogf(GL_FOG_START, 100.0f);
	glFogf(GL_FOG_END, 250.0f);
	glEnable(GL_FOG);

    GetRenderer()->StartRenderingMeshes();
    if (m_pCurLevel)
        m_pCurLevel->GetTerrain()->Render(m_mView);

    m_pSg->Render(m_mView);
	
	glDisable(GL_FOG);

    GetRenderer()->FinishRenderingMeshes();
    GetRenderer()->Reset();
}


// Activate
void CGameScreenController::Activate ()
{
	m_State = CSTATE_ACTIVE;
    GetInput()->RegisterReceiver(this);
}


// deactivate
void CGameScreenController::Deactivate ()
{
	m_State = CSTATE_INACTIVE;
    GetInput()->UnregisterReceiver(this);
}


// Control vector change event handler.
void CGameScreenController::OnVectorChanged (const Vec3& _vVec)
{
}


// Check if finish condition is satisfied.
bool CGameScreenController::CheckFinishCondition ()
{
	const Vec3& vFinishPoint = m_pCurLevel->GetFinishPosition();
	const Vec3& vCurPoint = m_pCamera->GetPosition();
	if (Dist2DSq(vCurPoint, vFinishPoint) <= m_fFinishPointSqDistance)
	{
		return true;
	}
	return false;
}


// Update camera.
void CGameScreenController::UpdateCamera ()
{
    if (m_pCamera && m_pCurLevel)
    {
        const Vec3& vTarget = m_pPlayer->GetPhysicalObject()->GetPosition() + Vec3(0, 0, -15);
        Vec3 vDir  = Vec3(0, 0.6f, 0.4f).normalized();
        Vec3 vUp = vDir.cross (Vec3(1, 0, 0));
        Vec3 vPos = vTarget + (vDir*m_fCameraTargetDistance);

        m_pCamera->Setup (vPos, vTarget, vUp);
	}
}
