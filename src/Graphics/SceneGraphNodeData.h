// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_SCENEGRAPHNODEDATA_H_
#define GRAPHICS_SCENEGRAPHNODEDATA_H_

namespace SceneGraph
{
	class Node;

	class NodeData
	{
		friend Node;

	protected:
		NodeData() : node_(nullptr) { }
		~NodeData();

	private:
		Node *node_;
	};
}

#endif
