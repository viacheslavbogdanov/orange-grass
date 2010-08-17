/*
 *  ogtexture.cpp
 *  OrangeGrass
 *
 *  Created by Viacheslav Bogdanov on 07.11.09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#include "ogtexture.h"


COGTexture::COGTexture () : m_TextureId (0),
							m_pTexture (NULL)
{
	m_pTexture = (CTexture*)malloc(sizeof(CTexture));
	memset(m_pTexture, 0, sizeof(CTexture));
}


COGTexture::~COGTexture ()
{
	if (m_pTexture)
	{
		m_pTexture->ReleaseTexture(m_TextureId);
		free(m_pTexture);
		m_pTexture = NULL;
	}
	m_TextureId = 0;
}


// load from file.
bool COGTexture::Load ()
{
	if (m_LoadState == OG_RESSTATE_UNKNOWN)
	{
		return false;
	}

	if(!m_pTexture->LoadTextureFromPVR(m_ResourceFile.c_str(), &m_TextureId))
	{
        printf("Failed to load %s", m_ResourceFile.c_str());
		return false;
	}

	m_LoadState = OG_RESSTATE_LOADED;
	
	return true;
}


// Unload resource.
void COGTexture::Unload ()
{
	if (m_LoadState != OG_RESSTATE_LOADED)
	{
		return;
	}

	memset(m_pTexture, 0, sizeof(CTexture));
	m_TextureId = 0;

	m_LoadState = OG_RESSTATE_DEFINED;
}


// apply texture
void COGTexture::Apply () const
{
	glBindTexture(GL_TEXTURE_2D, m_TextureId);
}
