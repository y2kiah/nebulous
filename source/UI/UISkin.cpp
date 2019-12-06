/*----==== UISKIN.CPP ====----
	Author:		Jeff Kiah
	Orig.Date:	08/13/2009
	Rev.Date:	08/14/2009
----------------------------*/

#include "UISkin.h"
#include "../Utility/tinyxml253/tinyxml.h"

namespace UI {

////////// class UISkin //////////

/*---------------------------------------------------------------------
	onLoad is called automatically by the resource caching system when
	a resource is first loaded from disk and added to the cache.
---------------------------------------------------------------------*/
bool UISkin::onLoad(const BufferPtr &dataPtr, bool async)
{
	TiXmlDocument doc;
	if (doc.Parse((const char *)dataPtr.get())) {
		if (doc.Error()) {
			debugPrintf("UISkin: Error: in \"%s\", row %i, col %i: %s\n", name().c_str(), doc.ErrorRow(), doc.ErrorCol(), doc.ErrorDesc());
			return false;
		}
		// this was a good parse
		// get document root element
		TiXmlHandle hDoc(&doc);
		TiXmlElement *pElem = hDoc.FirstChildElement().Element();
		if (!pElem) {
			debugPrintf("UISkin: Error: in \"%s\", no root element\n", name().c_str());
			return false;
		}
		TiXmlHandle hRoot(pElem);
		
		// get FixedSkins values
		pElem = hRoot.FirstChild("FixedSkins").FirstChild().Element();
		for (pElem; pElem; pElem = pElem->NextSiblingElement()) {
			debugPrintf("%s...\n", pElem->Value());
		}

		// print the entire document
		ifDebug(doc.Print());
	
		return true;
	}
	debugPrintf("UISkin: Error: null returned from Parse\n");
	return false;
}

} // namespace UI