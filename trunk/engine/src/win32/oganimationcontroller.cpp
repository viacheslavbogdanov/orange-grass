/*
 *  oganimationcontroller.cpp
 *  OrangeGrass
 *
 *  Created by Viacheslav Bogdanov on 08.11.09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#include "OrangeGrass.h"
#include "oganimationcontroller.h"


COGAnimationController::COGAnimationController() :  m_pCurAnimation(NULL),
                                                    m_fProgress(0.0f),
                                                    m_bPlaying(false)
{
}


COGAnimationController::~COGAnimationController ()
{
}


// start animation.
void COGAnimationController::StartAnimation (IOGAnimation* _pAnimation)
{
	if (_pAnimation == NULL)
		return;

	m_pCurAnimation = _pAnimation;
	m_bPlaying = true;
	m_fProgress = (float)m_pCurAnimation->start_frame;
}


// stop animation.
void COGAnimationController::StopAnimation ()
{
	m_pCurAnimation = NULL;
	m_bPlaying = false;
	m_fProgress = 0.0f;
}


// pause animation.
void COGAnimationController::PauseAnimation ()
{
	m_bPlaying = false;
}


// resume animation.
void COGAnimationController::ResumeAnimation ()
{
	m_bPlaying = true;
}


// update animation.
void COGAnimationController::UpdateAnimation (unsigned long _ElapsedTime)
{
	if (m_bPlaying)
	{
        float fNewProgress = m_fProgress + 5;
        if (fNewProgress < m_pCurAnimation->end_frame - 1)
        {
            m_fProgress = fNewProgress;
        }
        else
        {
            m_fProgress = (float)m_pCurAnimation->start_frame + (fNewProgress - m_pCurAnimation->end_frame - 1);
        }
	}
}