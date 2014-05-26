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
#ifndef IOGSHADERMANAGER_H_
#define IOGSHADERMANAGER_H_

#include "IOGShader.h"
#include <string>


class IOGShaderManager
{
public:
    virtual ~IOGShaderManager () {}

    // initialize shaders.
    virtual bool Init () = 0;

    // destroy shaders.
    virtual void Destroy () = 0;

    // get shader.
    virtual IOGShader* GetShader (OGShaderID _ShaderId) = 0;

    // get shader.
    virtual IOGShader* GetShader (const std::string& _ShaderAlias) = 0;
};


#endif