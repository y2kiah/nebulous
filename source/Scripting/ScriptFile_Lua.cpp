/*----==== SCRIPTFILE_LUA.CPP ====----
	Author:		Jeffrey Kiah
	Orig.Date:	08/25/2009
	Rev.Date:	08/25/2009
------------------------------------*/

#include "ScriptFile_Lua.h"

////////// class ScriptFile_Lua //////////

/*---------------------------------------------------------------------
	onLoad is called automatically by the resource caching system when
	a resource is first loaded from disk and added to the cache. This
	function executes lua_dostring() on the buffer passed in. The
	contents of the buffer are not stored locally.
---------------------------------------------------------------------*/
bool ScriptFile_Lua::onLoad(const BufferPtr &dataPtr, bool async)
{
	

	return true;
}
