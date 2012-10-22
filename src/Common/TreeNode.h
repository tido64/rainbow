#ifndef TREENODE_H_
#define TREENODE_H_

#include "Common/Vector.h"

/// A barebone implementation of a tree node.
///
/// This class does not hold any data and is meant to be sub-classed.
///
/// Copyright 2011-12 Bifrost Entertainment. All rights reserved.
/// \author Tommy Nguyen
template<class T>
class TreeNode : public NonCopyable<TreeNode<T> >
{
public:
	TreeNode();
	virtual ~TreeNode();

	/// Add a node as child.
	void add_child(T *);

	/// Remove a child node.
	void remove_child(T *);

	/// Set parent node.
	void set_parent(T *);

protected:
	T *parent;            ///< This node's parent.
	Vector<T*> children;  ///< This node's children.
};

template<class T>
TreeNode<T>::TreeNode() : parent(nullptr) { }

template<class T>
TreeNode<T>::~TreeNode() { }

template<class T>
void TreeNode<T>::add_child(T *n)
{
	n->parent = static_cast<T*>(this);
	this->children.push_back(n);
}

template<class T>
void TreeNode<T>::remove_child(T *n)
{
	if (!n)
		return;
	this->children.remove(n);
	delete n;
}

template<class T>
void TreeNode<T>::set_parent(T *n)
{
	if (this->parent)
		this->parent->children.remove(static_cast<T*>(this));
	n->add_child(static_cast<T*>(this));
}

#endif
