/*----==== SCENE.H ====----
	Author:		Jeff Kiah
	Orig.Date:	03/09/2010
	Rev.Date:	03/16/2010
-------------------------*/

#pragma once

#include <vector>
#include <list>
#include <memory>
#include "../Math/TMatrix4x4.h"

using std::vector;	using std::list;
using std::shared_ptr;

///// DECLARATIONS /////

class Scene;
class SceneNode;
typedef shared_ptr<Scene>			ScenePtr;
typedef shared_ptr<SceneNode>		SceneNodePtr;
typedef vector<SceneNodePtr>		SceneNodeList;

///// STRUCTURES /////

/*=============================================================================
struct SceneNodeProperties
	Contains attributes of the base class SceneNode, not including variables
	related to scene graph structure
=============================================================================*/
struct SceneNodeProperties {
	Matrix4x4f		localTransform;		// transformation relative to parent
	Matrix4x4f		toWorld;			// transformation to world space
	Matrix4x4f		fromWorld;			// inverse of mToWorld

	int				id;
	string			name;
};

/*=============================================================================
class SceneNode
=============================================================================*/
class SceneNode {
	friend class Scene;
	protected:
		///// VARIABLES /////
		SceneNodeProperties		mProperties;
		SceneNodeList			mChildNodes;
		SceneNodePtr			mParentNode;
		ScenePtr				mScenePtr;

		///// FUNCTIONS /////
		/*---------------------------------------------------------------------
			Iterates through child nodes and calls preRender, render, render-
			Children, and postRender. If preRender returns false, render and
			renderChildren are not called. If isVisible returns false, render
			is not called.
		---------------------------------------------------------------------*/
		virtual void	renderChildren();

	public:
		const SceneNodeProperties &	getProperties() const { return mProperties; }

		/*---------------------------------------------------------------------
		---------------------------------------------------------------------*/
		virtual void	update(float deltaMillis) = 0;

		/*---------------------------------------------------------------------
			Return true to continue with render, false to cancel render
		---------------------------------------------------------------------*/
		virtual bool	preRender() = 0;

		/*---------------------------------------------------------------------
		---------------------------------------------------------------------*/
		virtual void	render() = 0;

		/*---------------------------------------------------------------------
		---------------------------------------------------------------------*/
		virtual void	postRender() = 0;

		/*---------------------------------------------------------------------
		---------------------------------------------------------------------*/
		virtual bool	isVisible() = 0;

		explicit SceneNode() {}
		virtual ~SceneNode() {}
};

/*=============================================================================
class Scene
=============================================================================*/
class Scene {
	private:
		SceneNodePtr	mRootNode;

	public:
		void	render();

		explicit Scene() {}
		~Scene() {}
};