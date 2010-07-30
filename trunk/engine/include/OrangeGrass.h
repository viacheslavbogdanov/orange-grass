#ifndef ORANGEGRASS_H_
#define ORANGEGRASS_H_

#include "OpenGL2.h"
#include "IOGCoreHelpers.h"
#include "IOGResourceMgr.h"
#include "IOGLevelManager.h"
#include "IOGSceneGraph.h"
#include "IOGActorManager.h"
#include "IOGSprite.h"
#include "IOGCamera.h"
#include "IOGLight.h"


IOGResourceMgr* GetResourceMgr ();
IOGLevelManager* GetLevelManager ();
IOGSceneGraph* GetSceneGraph ();
IOGActorManager* GetActorManager ();
IOGCamera* GetCamera ();
IOGLight* GetLight ();
IOGSprite* CreateSprite (const char* _pAlias);

#endif