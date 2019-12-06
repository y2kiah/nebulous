//	----==== ISPHERETREE.H ====----
//
//	Author:			Jeffrey Kiah
//	Date:			7/4/2007
//	Description:	Container for a hierarchical bounding sphere tree
//					with integer precision.
//
//	-------------------------------

#pragma once

#include <vector>
#include "ISphereNode.h"


///// STRUCTURES /////

/*=============================================================================
	class  ISphereTree

=============================================================================*/
class ISphereTree {
	private:
		std::vector<ISphereNode*>	nodes;

	public:
		void createISphereNode(const Vector3i pos, uint radius) {

		}


		// Constructor / Destructor
		explicit ISphereTree() {}
		~ISphereTree() {}
};