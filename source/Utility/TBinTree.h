/* ----==== TBINTREE.H ====---- */

#pragma once

#include <boost/function.hpp>
#include "../Utility/Typedefs.h"

///// DEFINES /////

// Binary trees
#define	BinTree8		TBinTree<uchar>		//  8 levels (0-255)
#define	BinTree16		TBinTree<ushort>	// 16 levels (0-65535)
#define	BinTree32		TBinTree<uint>		// 32 levels (0-4294967295)
#define	BinTree64		TBinTree<ulong>		// 64 levels (0-18446744073709551615)

using boost::function;

///// STRUCTURES /////

//=============================================================================
//	This class encapsulates a generic non-recursive traversal routine for a
//	binary tree of N dimesions, which takes the form of a binary tree, quad-
//	tree, oct-tree, etc. The template type generally should be limited to a
//	uchar, ushort, uint, or ulong.
//=============================================================================
template <class T>
class TBinTree {
	private:
		///// VARIABLES /////
		int				mNumDimensions;			// (e.g. 1, 2, 3, ...)
		int				mChildNodesPerLevel;	// = 1 << numDimensions (e.g. 2, 4, 8, ...)
		BitField<T>		*mCurrentIndex;			// array of coordinates used during traversal, size = mNumDimensions

		///// FUNCTIONS /////
		inline bool		onIsLeafNode(int level, const BitField<T> *index) const;
		inline void		onBranchFirstVisit(int level, const BitField<T> *index) const;
		inline void		onLeafVisit(int level, const BitField<T> *index) const;

	public:
		///// VARIABLES /////
		// Function pointers
		function<bool (int level, const T *index)>	mOnIsLeafNodePtr;
		function<void (int level, const T *index)>	mOnBranchFirstVisitPtr;
		function<void (int level, const T *index)>	mOnLeafVisitPtr;

		///// FUNCTIONS /////
		int				numDimensions() const		{ return mNumDimensions; }
		int				childNodesPerLevel() const	{ return mChildNodesPerLevel; }

		inline void		traverse(int minLevel, int maxLevel, const T *index);

		// Constructors / Destructor
		explicit TBinTree();
		explicit TBinTree(int numDimensions);
		~TBinTree();
};


///// INLINE FUNCTIONS /////

//----------------------------------------------------------------------------------------
// This function is called upon the first arrival at each node. If it returns true, the
// node will be processed as a leaf, onLeafVisit will be called, and traversal will step
// back to the parent. If it returns false, and the node is not a max level node (which is
// inherently a leaf), onBranchFirstVisit will be called and traversal will progress to
// each child node before going back to the parent. First arrival at a node occurs when
// traversal progresses from a parent to a child node; it does not occur when traversal
// progresses from a child back to the parent.
//----------------------------------------------------------------------------------------
template <class T>
inline bool TBinTree<T>::onIsLeafNode(int level, const BitField<T> *index) const
{
	_ASSERTE(index);
	if (mOnIsLeafNodePtr) {
		return mOnIsLeafNodePtr(level, (T*)index);
	} else {
		return false;
	}
}

//----------------------------------------------------------------------------------------
// This function is called upon first visit to a branch node, as designated by the return
// value of onIsLeafNode. The node must not be a max level node, which are inherently leaf
// nodes. First visit occurs when traversal progresses from a parent to a child for the
// first time; it does not occur when traversal progresses from a child back to the parent
// node.
//----------------------------------------------------------------------------------------
template <class T>
inline void TBinTree<T>::onBranchFirstVisit(int level, const BitField<T> *index) const
{
	_ASSERTE(index);
	if (mOnBranchFirstVisitPtr) {
		mOnBranchFirstVisitPtr(level, (T*)index);
	}

/*	for (int l = 0; l < level; ++l) debugPrintf("  ");
	debugPrintf("onBranchFirstVisit: level=%d (", level);
	for (int d = 0; d < mNumDimensions; ++d) {
		if (d != 0) debugPrintf(","); debugPrintf("%d", index[d].bits());
	}
	debugPrintf(")\n");*/
}

//----------------------------------------------------------------------------------------
//	This function is called upon visit to a leaf node, as designated by the return value
//	of onIsLeafNode, or a max level node being an inherent leaf.
//----------------------------------------------------------------------------------------
template <class T>
inline void TBinTree<T>::onLeafVisit(int level, const BitField<T> *index) const
{
	_ASSERTE(index);
	if (mOnLeafVisitPtr) {
		mOnLeafVisitPtr(level, (T*)index);
	}

/*	for (int l = 0; l < level; ++l) debugPrintf("  ");
	debugPrintf("onLeafVisit: level=%d (", level);
	for (int d = 0; d < mNumDimensions; ++d) {
		if (d != 0) debugPrintf(","); debugPrintf("%d", index[d].bits());
	}
	debugPrintf(")\n");*/
}

//----------------------------------------------------------------------------------------
//	This function traverses a binary tree of arbitrary depth and dimensions, where the
//	number of depth levels is limited to the precision of T, and the number of children
//	per node is 2^N where N is the number of dimensions. Traversal will start at minLevel
//	and treat maxLevel nodes as leaf nodes. The location of start when minLevel > 0 is
//	specified by the index array, where each element represents an index coordinate for
//	one dimension, and the coordinates must be normalized to table-space for the level
//	that is being indexed. Table-space can be calculated as the range [ 0, 1<<minLevel ).
//	Min and max level range is [ 0, sizeof(T)*8 ]. Max level must be >= min level.
//	Traversal order is low to high level, (parent to child, root to leaf), looping first
//	by dimension 0, then 1, etc. So a quad-tree branch would traverse to its NW, then NE,
//	then SW, then SE children in that order.
//----------------------------------------------------------------------------------------
template <class T>
inline void TBinTree<T>::traverse(int minLevel, int maxLevel, const T *index)
{
	_ASSERTE(minLevel >= 0); _ASSERTE(maxLevel <= sizeof(T)*8); _ASSERTE(maxLevel >= minLevel);

	const int numLevels = maxLevel - minLevel + 1;
	int childNodesVisited[sizeof(T)*8+1] = {0};		// array for storing # of children visited at each level
	int currentLevel = minLevel;					// start traversal at the minimum level

	// set up starting node
	for (int l = 0; l < minLevel; ++l) {
		BitField<T> lBits(0);
		for (int d = 0; d < mNumDimensions; ++d) {
			lBits.setBit(d, BitField<T>(index[d]).getBit(minLevel-1-l));
		}
		childNodesVisited[l] = lBits.bits();
	}

	// traverse the tree
	for (;;) {
		// build the indexers
		for (int d = 0; d < mNumDimensions; ++d) mCurrentIndex[d].set(0);
		for (int l = 1; l <= currentLevel; ++l) {
			BitField<T> lBits(0);
			// get the index of the current node being visited
			lBits.set(childNodesVisited[l-1]);
			
			// build index bits
			for (int d = 0; d < mNumDimensions; ++d) {
				mCurrentIndex[d].setBit(currentLevel-l, lBits.getBit(d));
			}
		}

		// only on the first visit to this node check to see if it's a leaf node
		if (childNodesVisited[currentLevel] == 0) {
			// If this is a max-depth node it is a leaf, avoid calling onIsLeafNode
			// otherwise check to see if the node is a leaf
			bool isLeaf = false;
			if (currentLevel == maxLevel) isLeaf = true;
			else if (onIsLeafNode(currentLevel, mCurrentIndex)) isLeaf = true;

			if (!isLeaf) {
				// if the node is not a leaf, step down one level and start at the first child
				onBranchFirstVisit(currentLevel, mCurrentIndex);
				++currentLevel;
				childNodesVisited[currentLevel] = 0;
			} else {
				// if this is a leaf node go up to the parent level and indicate that
				// another child was visited at the parent level
				onLeafVisit(currentLevel, mCurrentIndex);
				if (--currentLevel < minLevel) break;
				else ++childNodesVisited[currentLevel];
			}
		// or on any subsequent visit don't recheck if it's a leaf, just visit all child nodes
		} else {
			if (childNodesVisited[currentLevel] < mChildNodesPerLevel) {
				// if all child nodes have not been visited step down one level and start at the first child
				++currentLevel;
				childNodesVisited[currentLevel] = 0;
			} else {
				// if all child nodes have been visited, step up to parent level and indicate
				// that another child was visited at the parent level
				if (--currentLevel < minLevel) break;
				else ++childNodesVisited[currentLevel];
			}
		}
	}
}

//----------------------------------------------------------------------------------------
//	The default contructor instantiates a 1D binary tree with empty function pointers
//----------------------------------------------------------------------------------------
template <class T>
TBinTree<T>::TBinTree() :
	mNumDimensions(1), mChildNodesPerLevel(2),
	mOnIsLeafNodePtr(0), mOnBranchFirstVisitPtr(0), mOnLeafVisitPtr(0)
{
	mCurrentIndex = new BitField<T>[mNumDimensions];
}

//----------------------------------------------------------------------------------------
//	This constructor allows specification of the number of dimensions, with empty function
//	pointers
//----------------------------------------------------------------------------------------
template <class T>
TBinTree<T>::TBinTree(int numDimensions) :
	mNumDimensions(numDimensions), mChildNodesPerLevel(1 << numDimensions),
	mOnIsLeafNodePtr(0), mOnBranchFirstVisitPtr(0), mOnLeafVisitPtr(0)
{
	mCurrentIndex = new BitField<T>[mNumDimensions];
}

template <class T>
TBinTree<T>::~TBinTree()
{
	delete [] mCurrentIndex;
}
