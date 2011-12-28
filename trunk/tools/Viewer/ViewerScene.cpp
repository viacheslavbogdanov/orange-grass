/*
 *  ViewerScene.cpp
 *  OrangeGrass
 *
 *  Created by Viacheslav Bogdanov on 11.11.09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#include "OpenGL2.h"
#include "ViewerScene.h"
#include "OrangeGrass.h"
#include <IOGGraphicsHelpers.h>
#include "IOGMath.h"


CViewerScene::CViewerScene()
{
	m_pCurActor = NULL;
	m_fCameraDistance = 200.0f;
	m_bShowAABB = false;
	m_bShowGrid = true;
    m_bInited = false;
	m_ResX = m_ResY = 0;
}


CViewerScene::~CViewerScene()
{
    OG_SAFE_DELETE(m_pCurActor);
}


// Initialize scene
bool CViewerScene::Init ()
{
    if (m_bInited)
        return true;

	glewInit();

	m_pRenderer = GetRenderer();
	m_pRenderer->SetViewport(m_ResX, m_ResY, 4.0f, 4500.0f, 0.67f);

	m_pResourceMgr = GetResourceMgr();
	m_pSg = GetSceneGraph();
	m_pCamera = m_pRenderer->GetCamera();
	m_pActorMgr = GetActorManager();

    OGVec3 vTarget (0, 0, 0);
	OGVec3 vDir (0, -1.0f, 0.4f);
	vDir = vDir.normalize();
	OGVec3 vUp = vDir.cross (OGVec3(1, 0, 0));
	m_pCamera->Setup (vTarget - (vDir*m_fCameraDistance), vTarget, vUp);

	OGVec3 vLightDir = OGVec3(0,1,0);
	OGVec4 vLightColor = OGVec4(1,1,1,1);
	IOGLight* pMainLight = m_pRenderer->GetLightMgr()->CreateLight();
	pMainLight->type = OG_LIGHT_DIRECTIONAL;
	pMainLight->vPosition = vLightDir;
	pMainLight->fIntensity = 100.0f;
	pMainLight->vAmbientColor = OGVec4(vLightColor.x, vLightColor.y, vLightColor.z, 1.0f);
	pMainLight->vDiffuseColor = OGVec4(vLightColor.x, vLightColor.y, vLightColor.z, 1.0f);
	pMainLight->vSpecularColor = OGVec4(vLightColor.x, vLightColor.y, vLightColor.z, 1.0f);

	glDisable(GL_CULL_FACE);
	glEnable(GL_NORMALIZE);

	if (m_pResourceMgr->Load(OG_RESPOOL_GAME) == false)
	{
        return false;
	}

	if (GetActorParamsMgr()->Init())
	{
		std::list<IOGActorParams*> ActorsParamsList;
		GetActorParamsMgr()->GetParamsList(ActorsParamsList);
		std::list<IOGActorParams*>::const_iterator iter = ActorsParamsList.begin();
		for (; iter != ActorsParamsList.end(); ++iter)
		{
			if ((*iter)->type == OG_ACTOR_MISSILE ||
				(*iter)->type == OG_ACTOR_PLASMAMISSILE ||
				(*iter)->type == OG_ACTOR_GAUSSRAY )
			{
				continue;
			}

			CommonToolEvent<ResLoadEventData> cmd(EVENTID_RESLOAD);
			cmd.SetEventCustomData(ResLoadEventData(wxT((*iter)->alias.c_str()), 
				wxT(ActorTypeToGroupName((*iter)->type).c_str()),
				wxT((*iter)->icon.c_str())));
			GetEventHandlersTable()->FireEvent(EVENTID_RESLOAD, &cmd);
		}
	}
	else
	{
        return false;
	}

	SetupModel(NULL);
    m_bInited = true;

	return true;
}


// Setup viewport
void CViewerScene::SetViewport (int _Width, int _Height)
{
	m_ResX = _Width;
	m_ResY = _Height;

	glViewport(0, 0, m_ResX, m_ResY);
	if (m_bInited)
	{
		m_pRenderer->SetViewport(m_ResX, m_ResY, 4.0f, 4500.0f, 0.67f);
	}
}


// Update controller
void CViewerScene::Update (unsigned long _ElapsedTime)
{
	GetPhysics()->UpdateAll(0);
    m_pActorMgr->UpdateEditor(33);
    m_pSg->Update(33);
	m_pCamera->Update();
	m_mView = m_pCamera->GetViewMatrix();
}


// Render controller scene
void CViewerScene::RenderScene ()
{
	m_pRenderer->ClearFrame(OGVec4(0.3f, 0.3f, 0.4f, 1.0f));

	RenderHelpers();

	m_pRenderer->EnableLight(true);
    m_pRenderer->StartRenderMode(OG_RENDERMODE_GEOMETRY);
    m_pSg->RenderAll(m_pCamera);
	m_pRenderer->FinishRenderMode();
	m_pRenderer->EnableLight(false);
    m_pRenderer->StartRenderMode(OG_RENDERMODE_EFFECTS);
    m_pSg->RenderAllEffects(m_pCamera);
	m_pRenderer->FinishRenderMode();

	m_pRenderer->StartRenderMode(OG_RENDERMODE_TEXT);
	unsigned long Verts; 
	unsigned long Faces;
	unsigned long TextureSwitches;
	unsigned long VBOSwitches;
	unsigned long DrawCalls;
	GetStatistics()->GetStatistics(Verts, Faces, TextureSwitches, 
		VBOSwitches, DrawCalls);
	m_pRenderer->DisplayString(OGVec2(85.0f, 2.0f), 0.4f, 0x7FFFFFFF, "Vertices: %d", Verts);
	m_pRenderer->DisplayString(OGVec2(85.0f, 6.0f), 0.4f, 0x7FFFFFFF, "Faces: %d", Faces);
	m_pRenderer->DisplayString(OGVec2(85.0f,10.0f), 0.4f, 0x7FFFFFFF, "Textures: %d", TextureSwitches);
	m_pRenderer->DisplayString(OGVec2(85.0f,14.0f), 0.4f, 0x7FFFFFFF, "VBO: %d", VBOSwitches);
	m_pRenderer->DisplayString(OGVec2(85.0f,18.0f), 0.4f, 0x7FFFFFFF, "DP: %d", DrawCalls);
	GetStatistics()->Reset();
	m_pRenderer->FinishRenderMode();

	m_pRenderer->Reset();

	glFlush();
}


// Render scene helpers.
void CViewerScene::RenderHelpers()
{
	glUseProgram(0);
    m_pRenderer->GetProjectionMatrix(m_mProjection);
	m_pRenderer->GetViewMatrix(m_mView);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(m_mProjection.f);

    glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(m_mView.f);
    glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);

	if (m_bShowAABB)
	{
        if (m_pCurActor)
        {
            DrawOBB(m_pCurActor->GetPhysicalObject()->GetOBB());
        }
	}

	if (m_bShowGrid)
	{
		DrawCoordGrid(50, 2, 10);
	}

    glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
}


// Setup model.
void CViewerScene::SetupModel(const char* _pModelAlias)
{
    if (m_pCurActor)
    {
        m_pActorMgr->DestroyActor(m_pCurActor);
    }

	if (_pModelAlias != NULL)
	{
		m_pCurActor = m_pActorMgr->CreateActor(
            _pModelAlias,
            OGVec3(0,0,0), 
            OGVec3(0,0,0), 
            OGVec3(1,1,1));
        m_pActorMgr->AddActor(m_pCurActor);
        m_pCurActor->Activate(true);

        IOGModel* pModel = m_pResourceMgr->GetModel(OG_RESPOOL_GAME, m_pCurActor->GetParams()->model_alias);
        CommonToolEvent<MtlLoadEventData> cmd(EVENTID_MTLLOAD);
        cmd.SetEventCustomData(MtlLoadEventData(
            pModel->GetMaterial()->GetAmbient().x,
            pModel->GetMaterial()->GetDiffuse().x,
            pModel->GetMaterial()->GetSpecular().x));
        GetEventHandlersTable()->FireEvent(EVENTID_MTLLOAD, &cmd);
	}
	else
	{
	}
}


// Adjust model material.
void CViewerScene::AdjustMaterial(MtlType _type, float _val)
{
    if (m_pCurActor)
    {
        IOGModel* pModel = m_pResourceMgr->GetModel(OG_RESPOOL_GAME, m_pCurActor->GetParams()->model_alias);
        switch(_type)
        {
        case MTLTYPE_DIF:
            pModel->GetMaterial()->SetDiffuse(OGVec4(_val, _val, _val, 1.0f));
            break;
        case MTLTYPE_AMB:
            pModel->GetMaterial()->SetAmbient(OGVec4(_val, _val, _val, 1.0f));
            break;
        case MTLTYPE_SPC:
            pModel->GetMaterial()->SetSpecular(OGVec4(_val, _val, _val, 1.0f));
            break;
        }
    }
}


// Camera zoom
void CViewerScene::CameraZoom (float _fFactor)
{
	m_fCameraDistance -= _fFactor;
	CameraRotateHor (0);
}


// Camera rotate
void CViewerScene::CameraRotate (float _fAngleH, float _fAngleV)
{
}


// Camera rotate horizontally
void CViewerScene::CameraRotateHor (float _fAngle)
{
	OGVec3 vTarget (0, 0, 0);

	OGMatrix mR;
	MatrixRotationY(mR, _fAngle);
	OGVec3 vDir, vRight;
	MatrixVec3Multiply(vDir, m_pCamera->GetDirection(), mR);
	MatrixVec3Multiply(vRight, m_pCamera->GetRight(), mR);
	vDir.normalize();
	OGVec3 vUp = vDir.cross (vRight);
	m_pCamera->Setup (vTarget - (vDir*m_fCameraDistance), vTarget, vUp);
}


// Camera rotate vertically
void CViewerScene::CameraRotateVer (float _fAngle)
{
	OGVec3 vTarget (0, 0, 0);
	
	OGMatrix mR;
	MatrixRotationAxis(mR, _fAngle, m_pCamera->GetRight().x, m_pCamera->GetRight().y, m_pCamera->GetRight().z);
	OGVec3 vDir, vRight;
	MatrixVec3Multiply(vDir, m_pCamera->GetDirection(), mR);
	MatrixVec3Multiply(vRight, m_pCamera->GetRight(), mR);
	vDir.normalize();
	OGVec3 vUp = vDir.cross (vRight);
	m_pCamera->Setup (vTarget - (vDir*m_fCameraDistance), vTarget, vUp);
}


// Convert actor type to group name.
std::string CViewerScene::ActorTypeToGroupName(OGActorType type)
{
	switch (type)
	{
	case OG_ACTOR_STATIC:
		return std::string("Static");

	case OG_ACTOR_NONE:
	case OG_ACTOR_PLASMAMISSILE:
	case OG_ACTOR_MISSILE:
	case OG_ACTOR_GAUSSRAY:
		return std::string("Others");

	case OG_ACTOR_LANDBOT:
	case OG_ACTOR_AIRBOT:
	case OG_ACTOR_PLAYER:
		return std::string("Units");
	
	case OG_ACTOR_BONUS:
		return std::string("Bonuses");
	}

	return std::string("Others");
}


// Save changes.
void CViewerScene::SaveChanges()
{
    if (m_pCurActor)
    {
        IOGModel* pModel = m_pResourceMgr->GetModel(OG_RESPOOL_GAME, m_pCurActor->GetParams()->model_alias);
        pModel->SaveParams();
    }
}
