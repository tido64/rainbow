// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
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
#ifdef USE_HEIMDALL
	bool has_changes() const;
	void reset_state();
#endif

	/// Adds a node as child.
	void add_child(T *);

	/// Removes node from the tree and deletes it.
	void remove();

	/// Removes a child node.
	void remove_child(T *);

	/// Recursively calls function \p f on \p node and its children.
	template<typename U, typename F, typename... Args>
	friend void for_each(U *node, F&& f, Args&&... args);

protected:
	T *parent_;                 ///< This node's parent.
	std::vector<T*> children_;  ///< This node's children.

	TreeNode();
	~TreeNode();

#ifdef USE_HEIMDALL
	bool has_changes_;

	void on_change();
#endif
};

template<typename T>
void TreeNode<T>::add_child(T *node)
{
	if (node->parent_)
		rainbow::remove(node->parent_->children_, node);
	node->parent_ = static_cast<T*>(this);
	children_.push_back(node);
#ifdef USE_HEIMDALL
	on_change();
#endif
}

template<typename T>
void TreeNode<T>::remove()
{
	parent_->remove_child(static_cast<T*>(this));
}

template<typename T>
void TreeNode<T>::remove_child(T *node)
{
	if (!node)
		return;

	rainbow::remove(children_, node);
	delete node;
#ifdef USE_HEIMDALL
	on_change();
#endif
}

template<typename T>
TreeNode<T>::TreeNode() : parent_(nullptr)
#ifdef USE_HEIMDALL
                        , has_changes_(false)
#endif
{
	static_assert(std::is_base_of<TreeNode, T>::value,
	              "T must be a subclass of TreeNode");
}

template<typename T>
TreeNode<T>::~TreeNode()
{
	for (auto child : children_)
		delete child;
}

#ifdef USE_HEIMDALL
template<typename T>
bool TreeNode<T>::has_changes() const
{
	return has_changes_;
}

template<typename T>
void TreeNode<T>::reset_state()
{
	has_changes_ = false;
}

template<typename T>
void TreeNode<T>::on_change()
{
	if (!parent_)
		has_changes_ = true;
	else
		parent_->on_change();
}
#endif

template<typename T, typename F, typename... Args>
void for_each(T *node, F&& f, Args&&... args)
{
	using U = typename std::decay<T>::type;
	static_assert(std::is_base_of<TreeNode<U>, U>::value,
	              "T must be a subclass of TreeNode");
	static_assert(
	    std::is_convertible<F, std::function<void(T*, Args&&...)>>::value,
	    "function type void(T*, Args&&...) required");

	f(node, std::forward<Args>(args)...);
	for (auto child : node->children_)
		for_each(child, f, std::forward<Args>(args)...);
}

#endif
