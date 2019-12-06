/*----==== SCENE.CPP ====----
	Author:		Jeff Kiah
	Orig.Date:	03/16/2010
	Rev.Date:	03/16/2010
---------------------------*/

#include "Scene.h"

////////// class SceneNode //////////

/*---------------------------------------------------------------------
	Iterates through child nodes and calls preRender, render, render-
	Children, and postRender. If preRender returns false, render and
	renderChildren are not called. If isVisible returns false, render
	is not called.
---------------------------------------------------------------------*/
void SceneNode::renderChildren()
{
	SceneNodeList::const_iterator i, ei = mChildNodes.end();
	for (i = mChildNodes.begin(); i != ei; ++i) {
		SceneNode &thisNode = *((*i).get());
		if (thisNode.preRender()) {
			if (thisNode.isVisible()) {
				thisNode.render();
			}
			thisNode.renderChildren();
		}
		thisNode.postRender();
	}
}