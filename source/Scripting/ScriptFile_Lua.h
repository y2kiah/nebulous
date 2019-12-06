/*----==== SCRIPTFILE_LUA.H ====----
	Author:		Jeff Kiah
	Orig.Date:	08/25/2009
	Rev.Date:	08/25/2009
----------------------------------*/

#pragma once

#include "../Resource/ResHandle.h"

///// STRUCTURES /////

/*=============================================================================
class ScriptFile_Lua
	This class derived from Resource enables Lua script files (.lua or .luc) to
	be loaded from any IResourceSource using the resource caching system. The
	onLoad method executes the script, much like calling Lua's dofile() method.
	This object stores the script locally, passing itself to the script manager
	for execution from onLoad. It's unlikely that there will be any need to
	keep a persistent copy of this object in memory after the script has been
	executed.
=============================================================================*/
class ScriptFile_Lua : public Resource {
	private:
		///// VARIABLES /////
		

	public:
		static const ResCacheType	sCacheType = ResCache_Script;

		///// FUNCTIONS /////
		/*---------------------------------------------------------------------
			onLoad is called automatically by the resource caching system when
			a resource is first loaded from disk and added to the cache. This
			function executes Lua's dostring() method on the buffer passed in.
			The contents of the buffer are stored locally.
		---------------------------------------------------------------------*/
		virtual bool onLoad(const BufferPtr &dataPtr, bool async);

		/*---------------------------------------------------------------------
			constructor with this signature is required for the resource system
		---------------------------------------------------------------------*/
		explicit ScriptFile_Lua(const string &name, uint sizeB, const ResCachePtr &resCachePtr) :
			Resource(name, sizeB, resCachePtr)
		{}
		/*---------------------------------------------------------------------
			default constructor used to create resources without caching, or
			for cache injection method
		---------------------------------------------------------------------*/
		explicit ScriptFile_Lua() {}

		virtual ~ScriptFile_Lua() {}
};