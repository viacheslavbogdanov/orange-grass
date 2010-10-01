/*
 *  IOGAnimation.h
 *  OrangeGrass
 *
 *  Created by Viacheslav Bogdanov on 11.11.09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef IOGANIMATION_H_
#define IOGANIMATION_H_

#include "Mathematics.h"
#include <string>


struct IOGAnimation
{
	std::string name;
	unsigned int start_frame;
	unsigned int end_frame;
};

#endif