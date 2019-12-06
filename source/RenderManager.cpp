/*----==== RENDERMANAGER.CPP ====----
	Author: Jeff Kiah
	Date:	4/13/2007
-----------------------------------*/

#include "RenderManager.h"


///// FUNCTIONS /////

RenderManager::RenderManager(const std::string &_renderName) :
	renderOptions(0),
	renderName(_renderName),
	numSubmitted(0)
{
	renderObjects.reserve(20);
	freeRenderObjectIDs.reserve(20);
	submittedObjects.reserve(20);
}

