/* ----==== HEAP.CPP ====---- */
/*
#include <crtdbg.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include "Heap.h"

///// DEFINES /////
#define MEMSYSTEM_SIGNATURE		0xDEADC0DE
#define MEMSYSTEM_ENDMARKER		0xDEADC0DE


///// STATIC VARIABLES /////
unsigned int Heap::sAllocNum = 0;

///// STRUCTURES /////
struct AllocHeader {
	unsigned int	signature;
	unsigned int	allocNum;
	size_t			size;
	Heap *			pHeap;
	AllocHeader *	pNext;
	AllocHeader *	pPrev;
};

///// FUNCTIONS /////
void * Heap::allocate(size_t sizeBytes)
{
	size_t requestedBytes = sizeBytes + sizeof(AllocHeader);// + sizeof(int);
	char *pMem = (char *)malloc(requestedBytes);
	// init header
	AllocHeader *pHeader = (AllocHeader *)pMem;
	pHeader->signature = MEMSYSTEM_SIGNATURE;
	pHeader->allocNum = sAllocNum++;
	pHeader->size = sizeBytes;
	pHeader->pHeap = this;
	pHeader->pNext = mFirstAlloc;	// push to top of list
	pHeader->pPrev = 0;

	// update local heap
	mLocalSize += sizeBytes;
	if (mLocalSize > mLocalPeakSize) mLocalPeakSize = mLocalSize;
	++mLocalAllocCount;
	// linked list maintenance
	if (mFirstAlloc) mFirstAlloc->pPrev = pHeader;
	mFirstAlloc = pHeader;

	// update parent heap(s)
	if (mParent) mParent->addChildAllocation(sizeBytes);

	void *pStartMemBlock = pMem + sizeof(AllocHeader);
	//unsigned int *pEndMarker = (unsigned int *)((char *)pStartMemBlock + size);
	//*pEndMarker = MEMSYSTEM_ENDMARKER;

	return pStartMemBlock;
}

void Heap::deallocate(AllocHeader *pHeader)
{
	// update local heap
	mLocalSize -= pHeader->size;
	--mLocalAllocCount;
	// linked list maintenance
	if (!pHeader->pPrev) {
		_ASSERTE(mFirstAlloc == pHeader);
		mFirstAlloc = pHeader->pNext;
	} else {
		pHeader->pPrev->pNext = pHeader->pNext;
	}
	if (pHeader->pNext) pHeader->pNext->pPrev = pHeader->pPrev;

	// update parent heap(s)
	if (mParent) mParent->removeChildAllocation(pHeader->size);

	free(pHeader);
}

void Heap::deallocate(void *pMemBlock)
{
	AllocHeader *pHeader = (AllocHeader *)((char *)pMemBlock - sizeof(AllocHeader));
	_ASSERTE(pHeader->signature == MEMSYSTEM_SIGNATURE);
	//unsigned int *pEndMarker = (unsigned int *)((char *)pMemBlock + pHeader->size);
	//_ASSERTE(*pEndMarker == MEMSYSTEM_SIGNATURE);

	pHeader->pHeap->deallocate(pHeader);
}

void Heap::addChildAllocation(size_t sizeBytes)
{
	mChildSize += sizeBytes;
	if (mChildSize > mChildPeakSize) mChildPeakSize = mChildSize;
	++mChildAllocCount;
	if (mParent) mParent->addChildAllocation(sizeBytes);
}

void Heap::removeChildAllocation(size_t sizeBytes)
{
	mChildSize -= sizeBytes;
	--mChildAllocCount;
	if (mParent) mParent->removeChildAllocation(sizeBytes);
}

Heap::Heap(const char *name, Heap *parent) :
	mLocalAllocCount(0), mLocalSize(0), mLocalPeakSize(0),
	mChildAllocCount(0), mChildSize(0), mChildPeakSize(0),
	mParent(parent), mFirstAlloc(0), mLastAlloc(0)
{
	size_t strSize = strlen(name) + 1;	// add 1 for terminating null character
	mName = (char *)malloc(strSize*sizeof(char));
	strcpy_s(mName, strSize, name);
}

Heap::~Heap()
{
	free(mName);
}*/