#include "GraphicsPch.h"
#include "VRAMNode.h"

namespace MGF3D
{
	VRAMNode::VRAMNode(uint64 _offset, uint64 _size, bool _free)
		: offset(_offset), size(_size), isFree(_free) { }

	VRAMNode::~VRAMNode()
	{
		prev = nullptr;
		next = nullptr;
	}
}