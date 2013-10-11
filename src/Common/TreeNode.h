#ifndef COMMON_TREENODE_H_
#define COMMON_TREENODE_H_

#include "Common/Vector.h"

/// A barebone implementation of a tree node.
///
/// This class does not hold any data and is meant to be sub-classed.
///
/// Copyright 2011-13 Bifrost Entertainment. All rights reserved.
/// \author Tommy Nguyen
template<class T>
class TreeNode : private NonCopyable<TreeNode<T> >
{
public:
	/// Adds a node as child.
	void add_child(T *);

	/// Removes node from the tree and deletes it.
	void remove();

	/// Removes a child node.
	void remove_child(T *);

	/// Sets parent node.
	void set_parent(T *);

protected:
	T *parent;            ///< This node's parent.
	Vector<T*> children;  ///< This node's children.

	TreeNode();
	~TreeNode();
};

template<class T>
void TreeNode<T>::add_child(T *node)
{
	node->parent = static_cast<T*>(this);
	this->children.push_back(node);
}

template<class T>
void TreeNode<T>::remove()
{
	this->parent->remove_child(static_cast<T*>(this));
}

template<class T>
void TreeNode<T>::remove_child(T *node)
{
	if (!node)
		return;
	this->children.remove(node);
	delete node;
}

template<class T>
void TreeNode<T>::set_parent(T *node)
{
	node->add_child(static_cast<T*>(this));
	if (this->parent)
		this->parent->children.remove(static_cast<T*>(this));
}

template<class T>
TreeNode<T>::TreeNode() : parent(nullptr) { }

template<class T>
TreeNode<T>::~TreeNode()
{
	for (auto child : this->children)
		delete child;
}

#endif
