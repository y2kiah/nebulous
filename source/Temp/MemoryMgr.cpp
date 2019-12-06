
#include "MemoryMgr.h"
#include "Heap.h"
#include "HeapFactory.h"
#include <assert.h>
#include <cstddef>
#include <new>

void * operator new (std::size_t size) throw(std::bad_alloc)
{
    return operator new (size, HeapFactory::GetDefaultHeap() );
}


void * operator new (std::size_t size, Heap * pHeap) throw(std::bad_alloc)
{
    assert(pHeap != NULL);
    return pHeap->Allocate(size);
}


void operator delete (void * pMem) throw()
{
    if (pMem != NULL)
        Heap::Deallocate (pMem);    
}

