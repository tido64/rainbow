// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef COMMON_TREENODE_H_
#define COMMON_TREENODE_H_

#include <functional>
#include <vector>

#include "Common/Algorithm.h"
#include "Common/NonCopyable.h"
#include "Memory/NotNull.h"

/// <summary>A barebone implementation of a tree node.</summary>
/// <remarks>
///   This class does not hold any data and is meant to be sub-classed.
/// </remarks>
template <typename T>
class TreeNode : private NonCopyable<TreeNode<T>>
{
public:
    template <typename U>
    using Owner = U;

    /// <summary>Returns parent node.</summary>
    T* parent() { return parent_; }

    /// <summary>Adds a node as child.</summary>
    void add_child(NotNull<Owner<T*>> node)
    {
        if (node->parent_)
            rainbow::remove(node->parent_->children_, node);
        node->parent_ = self();
        children_.push_back(node);
    }

    /// <summary>Removes node from the tree and deletes it.</summary>
    void remove()
    {
        if (parent_ == nullptr)
            delete self();
        else
            parent_->remove_child(self());
    }

    /// <summary>Removes a child node.</summary>
    void remove_child(T* node)
    {
        if (node == nullptr)
            return;

        rainbow::remove(children_, node);
        delete node;
    }

    /// <summary>
    ///   Recursively calls function <paramref name="f"/> on
    ///   <paramref name="node"/> and its children.
    /// </summary>
    template <typename U, typename F, typename Enable, typename... Args>
    friend void for_each(U& node, F&& f, Args&&... args);

protected:
    T* parent_;                        ///< Parent node.
    std::vector<Owner<T*>> children_;  ///< Children node.

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
        for (auto&& child : children_)
            delete child;
    }

    auto self() { return static_cast<T*>(this); }

    TreeNode& operator=(TreeNode&& node)
    {
        parent_ = node.parent_;
        children_ = std::move(node.children_);
        node.parent_ = nullptr;
        return *this;
    }
};

template <typename T,
          typename F,
          typename = EnableIfBaseOf<TreeNode<PlainType<T>>, PlainType<T>>,
          typename... Args>
void for_each(T& node, F&& f, Args&&... args)
{
    f(node, std::forward<Args>(args)...);
    for (auto&& child : node.children_)
        for_each(*child, f, std::forward<Args>(args)...);
}

#endif
