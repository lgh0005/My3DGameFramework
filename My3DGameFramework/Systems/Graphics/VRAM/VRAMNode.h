#pragma once
#include "Pointer/PoolAlloc.h"

namespace MGF3D
{
    MGF_STRUCT_PTR(VRAMNode)
	struct VRAMNode : public PoolAlloc
	{
        VRAMNode(uint64 _offset, uint64 _size, bool _free);
        ~VRAMNode();

        uint64 offset       { 0 };      // VRAM 내 시작 주소
        uint64 size         { 0 };      // 해당 구역의 크기
        bool   isFree       { true };   // 현재 비어있는지 여부

        Ptr<VRAMNode> prev  { nullptr };
        Ptr<VRAMNode> next  { nullptr };
	};
}