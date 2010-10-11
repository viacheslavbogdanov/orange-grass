/*
 *  IOGModel.h
 *  OrangeGrass
 *
 *  Created by Viacheslav Bogdanov on 11.11.09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef IOGMODEL_H_
#define IOGMODEL_H_

#include "IOGRenderable.h"
#include "IOGTexture.h"
#include "IOGMesh.h"


class IOGModel : public IOGRenderable
{
public:
	virtual ~IOGModel() {}

	// Get model alias
	virtual const std::string& GetAlias () const = 0;

	// Get texture
	virtual IOGTexture* GetTexture () = 0;
};


#endif