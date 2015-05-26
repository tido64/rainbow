// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef COMMON_TREENODE_H_
#define COMMON_TREENODE_H_

#include <functional>
#include <vector>

#include "Common/Algorithm.h"
#include "Common/NonCopyable.h"

/// A barebone implementation of a tree node.
///
/// This class does not hold any data and is meant to be sub-classed.
template<typename T>
class TreeNode : private NonCopyable<TreeNode<T>>
{
public:
	/// Returns parent node.
	T* parent() { return parent_; }

	/// Adds a node as child.
	void add_child(T *node)
	{
		if (node->parent_)
			rainbow::remove(node->parent_->children_, node);
		node->parent_ = static_cast<T*>(this);
		children_.push_back(node);
	}

	/// Removes node from the tree and deletes it.
	void remove()
	{
		parent_->remove_child(static_cast<T*>(this));
	}

	/// Removes a child node.
	void remove_child(T *node)
	{
		if (!node)
			return;

		rainbow::remove(children_, node);
		delete node;
	}

	/// Recursively calls function \p f on \p node and its children.
	template<typename U, typename F, typename Enable, typename... Args>
	friend void for_each(U *node, F&& f, Args&&... args);

protected:
	T *parent_;                 ///< This node's parent.
	std::vector<T*> children_;  ///< This node's children.

	TreeNode() : parent_(nullptr)
	{
		static_assert(std::is_base_of<TreeNode, T>::value,
		              "T must be a subclass of TreeNode");
	}

	TreeNode(TreeNode&& node)
	    : parent_(node.parent_), children_(std::move(node.children_))
	{
		node.parent_ = nullptr;
	}

	~TreeNode()
	{
		for (auto child : children_)
			delete child;
	}

	TreeNode& operator=(TreeNode&& node)
	{
		parent_ = node.parent_;
		children_ = std::move(node.children_);
		node.parent_ = nullptr;
		return *this;
	}
};

template<typename T,
         typename F,
         typename = EnableIfBaseOf<TreeNode<typename std::decay<T>::type>,
                                   typename std::decay<T>::type>,
         typename... Args>
void for_each(T *node, F&& f, Args&&... args)
{
	static_assert(
	    std::is_convertible<F, std::function<void(T*, Args&&...)>>::value,
	    "function type void(T*, Args&&...) required");

	f(node, std::forward<Args>(args)...);
	for (auto child : node->children_)
		for_each(child, f, std::forward<Args>(args)...);
}

#endif
