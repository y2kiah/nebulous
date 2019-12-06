/* ----==== HEAP.H ====---- */

#pragma once
/*
///// STRUCTURES /////

struct AllocHeader;

class Heap {
	private:
		char *			mName;
		unsigned int	mLocalAllocCount;
		size_t			mLocalSize;
		size_t			mLocalPeakSize;
		unsigned int	mChildAllocCount;
		size_t			mChildSize;
		size_t			mChildPeakSize;
		Heap *			mParent;
		AllocHeader *	mFirstAlloc;
		AllocHeader *	mLastAlloc;

		// Static
		static unsigned int	sAllocNum;

	public:
		const char *	getName() const				{ return mName; }
		unsigned int	getLocalAllocCount() const	{ return mLocalAllocCount; }
		size_t			getLocalSize() const		{ return mLocalSize; }
		size_t			getLocalPeakSize() const	{ return mLocalPeakSize; }
		unsigned int	getChildAllocCount() const	{ return mChildAllocCount; }
		size_t			getChildSize() const		{ return mChildSize; }
		size_t			getChildPeakSize() const	{ return mChildPeakSize; }
		unsigned int	getTotalAllocCount() const	{ return mLocalAllocCount + mChildAllocCount; }
		size_t			getTotalSize() const		{ return mLocalSize + mChildSize; }
		size_t			getTotalPeakSize() const	{ return mLocalPeakSize + mChildPeakSize; }
		const AllocHeader *getFirstAlloc() const	{ return mFirstAlloc; }
		const AllocHeader *getLastAlloc() const		{ return mLastAlloc; }

		void *	allocate(size_t sizeBytes);
		void	deallocate(AllocHeader *pHeader);
		void	addChildAllocation(size_t sizeBytes);
		void	removeChildAllocation(size_t sizeBytes);

		explicit Heap(const char *name, Heap *parent);
		~Heap();

		// Static
		static void		deallocate(void *pMemBlock);
};*/