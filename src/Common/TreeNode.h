#ifndef TREENODE_H_
#define TREENODE_H_

#include "Common/List.h"

/// A barebone implementation of a tree node.
///
/// This class does not hold any data and is meant to be sub-classed.
///
/// Copyright 2011 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen
template<class T>
class TreeNode
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
	T *parent;          ///< This node's parent.
	List<T*> children;  ///< This node's children.
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
	this->children.remove(n);
	n->parent = nullptr;
}

template<class T>
void TreeNode<T>::set_parent(T *n)
{
	if (this->parent)
		this->parent->remove_child(static_cast<T*>(this));
	n->add_child(static_cast<T*>(this));
}

#endif
