/* ----==== MEMORYMANAGER.H ====---- */

#pragma once
/*
#include "../Utility Code/Typedefs.h"
#include "Heap.h"


///// MACROS /////
#define DEFINE_HEAP(a,b)	Heap * a::sHeap = 0;\
							void * a::operator new(size_t size) {\
								if (!sHeap) sHeap = MemoryManager::createHeap(b);\
								return ::operator new(size, sHeap);\
							}\
							void a::operator delete(void *p) {\
								::operator delete(p);\
							}

#define DECLARE_HEAP		public:\
								static void * operator new(size_t size);\
								static void operator delete(void *p);\
							private:\
								static Heap * sHeap;
									

///// STRUCTURES /////
*/
/*=============================================================================
	class MemoryManager

	MemoryManager class description
=============================================================================*/
/*class MemoryManager {
	private:
		static unsigned int	mAllocCount;
		static Heap			mDefaultHeap;

		explicit MemoryManager() {}

	public:
		static unsigned int	getAllocBookmark()	{ return mAllocCount++; }
		static Heap *		getDefaultHeap()	{ return &mDefaultHeap; }
		static Heap *		createHeap(const char *name)	{ return &mDefaultHeap; }//return new Heap(name, 0); }

		~MemoryManager() {  }
};

///// DECLARATIONS /////

// override global new with heap specified
inline void * operator new(size_t size, Heap *pHeap)
{
#ifdef _DEBUG
	_ASSERTE(pHeap);
	return pHeap->allocate(size);
#else
	return malloc(size);
#endif
}

// override global new with default heap
inline void * operator new(size_t size)
{
	return operator new (size, MemoryManager::getDefaultHeap());
}

inline void * operator new[](size_t size)
{
	debugPrintf("new[] called\n");
	//return operator new(size);
	malloc(size);
}

// override global delete
inline void operator delete(void *pMemBlock)
{
#ifdef _DEBUG
	if (pMemBlock) Heap::deallocate(pMemBlock);
#else
	free(pMemBlock);
#endif
}

inline void operator delete[](void *pMemBlock)
{
	debugPrintf("delete[] called\n");
	//operator delete(pMemBlock);
	free(pMemBlock);
}

// TEMP
class GameObject {
	// body
	private:
		DECLARE_HEAP;
	public:
};*/