/*
 *  ogsgnode.cpp
 *  OrangeGrass
 *
 *  Created by Viacheslav Bogdanov on 12.11.09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#include "OrangeGrass.h"
#include "ogsgnode.h"
#include "IOGMath.h"
#include "oganimationcontroller.h"


COGSgNode::COGSgNode () :   m_pRenderable(NULL),
                            m_pPhysics(NULL),
                            m_pAnimator(NULL)
{
    m_AnimFrame = 0;
}


COGSgNode::COGSgNode (IOGRenderable* _pRenderable,
                      IOGPhysicalObject* _pPhysics) :   m_pRenderable(_pRenderable),
                                                        m_pPhysics(_pPhysics),
                                                        m_pAnimator(NULL)
{
    m_pAnimator = new COGAnimationController();
    m_AnimFrame = 0;
}


COGSgNode::~COGSgNode () 
{
    m_pRenderable = NULL;
    m_pPhysics = NULL;
    OG_SAFE_DELETE(m_pAnimator);
}


// get world transform.
const MATRIX& COGSgNode::GetWorldTransform () const
{
    return m_pPhysics->GetWorldTransform();
}


// Get OBB
const IOGObb& COGSgNode::GetOBB () const
{
    return m_pPhysics->GetOBB();
}


// update transform.
void COGSgNode::Update (unsigned long _ElapsedTime)
{
    if (m_pAnimator->GetCurrentAnimation())
    {
        m_pAnimator->UpdateAnimation(_ElapsedTime);
        m_AnimFrame = (unsigned int)m_pAnimator->GetCurrentAnimationProgress();
    }
    m_pRenderable->Update(_ElapsedTime);
}


// render.
void COGSgNode::Render ()
{
    const MATRIX& mWorld = m_pPhysics->GetWorldTransform();
    m_pRenderable->Render(mWorld, m_AnimFrame);
}


// get renderable.
IOGRenderable* COGSgNode::GetRenderable ()
{
    return m_pRenderable;
}


// get physics.
IOGPhysicalObject* COGSgNode::GetPhysics ()
{
    return m_pPhysics;
}


// start animation.
void COGSgNode::StartAnimation (const std::string& _Alias)
{
    m_AnimFrame = 0;
    IOGAnimation* pAnim = m_pRenderable->GetAnimation(_Alias);
    if (pAnim)
        m_pAnimator->StartAnimation(pAnim);
}