//	----==== ISPHERENODE.CPP ====----
//
//	Author:			Jeffrey Kiah
//	Date:			7/4/2007
//	Description:	A single node in an ISphereTree, a hierarchical bounding
//					sphere tree with integer precision. This class is owned by
//					the objects that use it with a "has-a" relationship. The
//					construction and destruction of ISphereNode objects is the job
//					of the owning object, not the ISphereTree.
//
//	---------------------------------

#include "ISphereNode.h"


///// class ISphereNode /////