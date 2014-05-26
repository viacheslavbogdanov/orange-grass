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
#ifndef OGEMITTERPULSEGLOW_H_
#define OGEMITTERPULSEGLOW_H_

#include "ogemitter.h"
#include <vector>


class COGEmitterPulseGlow : public COGEmitter
{
public:
    COGEmitterPulseGlow();
    virtual ~COGEmitterPulseGlow();

    // Initialize emitter.
    virtual void Init(IOGGroupNode* _pNode);

    // Update.
    virtual void Update (unsigned long _ElapsedTime);

    // Render.
    virtual void Render (const OGMatrix& _mWorld, const OGVec3& _vLook, const OGVec3& _vUp, const OGVec3& _vRight, OGRenderPass _Pass);

    // Start.
    virtual void Start ();

    // Stop.
    virtual void Stop ();

    // Get effect type.
    virtual OGEmitterType GetType() const { return s_Type; }

    // Get effect type.
    virtual const std::string& GetAlias() const { return s_Alias; }

protected:

    struct ParticleFormat
    {
        bool    bDirty;
        float   scale;
        TBBVertexEntry verts;
    };

protected:

    ParticleFormat  m_Glow;
    int             m_GlowPulse;

    std::string     m_Texture;
    unsigned int    m_MappingId;
    float           m_fGlowAlphaInc;
    float           m_fAlphaInc;
    float           m_fInitialScale;
    OGVec4          m_color;

    IOGVertexBuffers*   m_pVBO;

public:

    static std::string     s_Alias;
    static OGEmitterType   s_Type;
};


#endif
