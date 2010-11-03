/*
 *  ogplasmaeffect.cpp
 *  OrangeGrass
 *
 *  Created by Viacheslav Bogdanov on 08.11.09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#include "ogplasmaeffect.h"
#include "OrangeGrass.h"


COGPlasmaEffect::~COGPlasmaEffect()
{
}


// Initialize effect.
void COGPlasmaEffect::Init(OGEffectType _Type)
{
	m_pTexture = GetResourceMgr()->GetTexture("plasma_01");
    m_pMaterial = GetMaterialManager()->GetMaterial(OG_MAT_TEXTUREALPHABLEND);

	Vec4 color = Vec4(1.0f, 0.0f, 0.0f, 0.2f);

	for (int i = 0; i < MAX_PLASMA_PARTILES; ++i)
	{
		COGBillboard& particle = m_Particles[i];
		particle.scale = ((float)MAX_PLASMA_PARTILES - i) / 4.5f;
		particle.pVertices = &m_Vertices[i * 4];
		particle.pVertices[0].t = Vec2(1.0f, 0.0f);
		particle.pVertices[0].c = color;
		particle.pVertices[1].t = Vec2(0.0f, 0.0f);
		particle.pVertices[1].c = color;
		particle.pVertices[2].t = Vec2(1.0f, 1.0f);
		particle.pVertices[2].c = color;
		particle.pVertices[3].t = Vec2(0.0f, 1.0f);
		particle.pVertices[3].c = color;
	}
    m_AABB.SetMinMax(Vec3(-1,-1,-1), Vec3(1,1,1));
}


// Update.
void COGPlasmaEffect::Update (unsigned long _ElapsedTime)
{
	if (m_Status == OG_EFFECTSTATUS_INACTIVE)
		return;

	for (int i = 0; i < MAX_PLASMA_PARTILES; ++i)
	{
		m_Particles[i].offset = m_Direction * (float)i;
	}
}


// Render.
void COGPlasmaEffect::Render (const MATRIX& _mWorld, unsigned int _Frame)
{
	if (m_Status == OG_EFFECTSTATUS_INACTIVE)
		return;

    m_pRenderer->SetModelMatrix(_mWorld);
	m_pRenderer->SetMaterial(m_pMaterial);
	m_pRenderer->SetTexture(m_pTexture);

	for (int i = 0; i < MAX_PLASMA_PARTILES; ++i)
	{
		COGBillboard& particle = m_Particles[i];
		Vec3 vSUp = m_vCameraUp * particle.scale;
		Vec3 vSRight = m_vCameraRight * particle.scale;
		particle.pVertices[0].p = vSRight + vSUp + particle.offset;
		particle.pVertices[1].p = -vSRight + vSUp + particle.offset;
		particle.pVertices[2].p = vSRight - vSUp + particle.offset;
		particle.pVertices[3].p = -vSRight - vSUp + particle.offset;

		m_pRenderer->DrawEffectBuffer(&particle.pVertices[0], 0, 4);
	}

    //m_pRenderer->DrawEffectBuffer(m_Vertices, 0, PLASMA_VERTICES);
}


// Start.
void COGPlasmaEffect::Start ()
{
	m_Status = OG_EFFECTSTATUS_STARTED;
    Update(0);
}


// Stop.
void COGPlasmaEffect::Stop ()
{
	m_Status = OG_EFFECTSTATUS_INACTIVE;
}