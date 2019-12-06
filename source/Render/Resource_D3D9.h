/*----==== RESOURCE_D3D9.H ====----
	Author:		Jeff Kiah
	Orig.Date:	06/21/2009
	Rev.Date:	06/23/2009
---------------------------------*/

#pragma once

#include <list>
#include "../Resource/ResHandle.h"

using std::list;

///// STRUCTURES /////

struct IDirect3DDevice9;

class Resource_D3D9;
typedef list<Resource_D3D9*>	Resource_D3D9List;

/*=============================================================================
class Resource_D3D9
=============================================================================*/
class Resource_D3D9 : public Resource {
	friend class RenderManager_D3D9;	// for access to sUnmanagedResList
	private:
		///// VARIABLES /////
		static Resource_D3D9List	sUnmanagedList;	// this static list holds pointers to all unmanaged resources
													// (not marked by setIsManaged(true)). RenderManager_D3D9 will
													// call restoreVolatileResource() on each element in the list
													// in response to a lost device.
		Resource_D3D9List::iterator	mListIter;		// iterator to element in sUnmanagedList or end() if not in list
													// maintained for quick erase()
		bool	mIsManaged;		// if resource uses D3DPOOL_MANAGED or D3DPOOL_SYSTEMMEM, this would be true

		///// FUNCTIONS /////
		/*---------------------------------------------------------------------
			These functions add or remove a this pointer to the resource in the
			unmanaged list. Also maintains the mListIter value.
		---------------------------------------------------------------------*/
		bool addToUnmanagedList() {
			if (mListIter != sUnmanagedList.end()) return false;
			sUnmanagedList.push_front(this);
			mListIter = sUnmanagedList.begin();
			return true;
		}
		bool removeFromUnmanagedList() {
			if (mListIter == sUnmanagedList.end()) return false;
			sUnmanagedList.erase(mListIter);
			mListIter = sUnmanagedList.end();
			return true;
		}

	protected:
		///// VARIABLES /////
		static IDirect3DDevice9 *	spD3DDevice;	// make weak smart pointer

		///// FUNCTIONS /////
		/*---------------------------------------------------------------------
			Called from a derived class constructor or initialization function
			to mark the resource as managed/unmanaged. Default is unmanaged.
			Also maintains presence in the unmanaged list for handling of lost
			device.
		---------------------------------------------------------------------*/
		void setIsManaged(bool b = true) {
			if (mIsManaged == b) return;
			mIsManaged = b;
			if (mIsManaged) {
				removeFromUnmanagedList();	// remove resource from list if managed
			} else {
				addToUnmanagedList();		// add the resource to the list if unmanaged
			}
		}

		/*---------------------------------------------------------------------
			These are called in response to a lost device for non-managed
			resources. An Implementation is expected to free the unmanaged
			resource (call Release()), and restore the resource respectively.
		---------------------------------------------------------------------*/
		virtual void	onDeviceLost() = 0;
		virtual void	onDeviceRestore() = 0;

	public:
		/*---------------------------------------------------------------------
			If resource uses managed pool, these will not call the private
			virtual functions, avoiding the call overhead. These are called
			from the RenderManager_D3D9 initVolatileResources and freeVolatile-
			Resources functions.
		---------------------------------------------------------------------*/
		void	freeVolatileResource() { if (!mIsManaged) onDeviceLost(); }
		void	initVolatileResource() { if (!mIsManaged) onDeviceRestore(); }

		/*---------------------------------------------------------------------
			constructor with this signature is required for the resource system
		---------------------------------------------------------------------*/
		explicit Resource_D3D9(const string &name, uint sizeB, const ResCachePtr &resCachePtr) :
			Resource(name, sizeB, resCachePtr),
			mIsManaged(false),
			mListIter(sUnmanagedList.end())
		{
			addToUnmanagedList();
		}
		/*---------------------------------------------------------------------
			default constructor used to create resources without caching, or
			for cache injection method
		---------------------------------------------------------------------*/
		explicit Resource_D3D9() :
			Resource(),
			mIsManaged(false),
			mListIter(sUnmanagedList.end())
		{
			addToUnmanagedList();
		}

		~Resource_D3D9() {
			removeFromUnmanagedList();
		}
};