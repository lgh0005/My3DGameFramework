#pragma once

namespace MGF3D
{
	MGF_CLASS_PTR(VRAMNode)

	MGF_CLASS_PTR(VRAMFreeList)
	class VRAMFreeList
	{
	public:
		VRAMFreeList(uint64 totalSize);
		~VRAMFreeList();

	public:
		uint64 Allocate(uint64 size, uint32 alignment);
		void Deallocate(uint64 offset);

	private:
		void SplitPaddingNode(Ptr<VRAMNode> target, uint64 padding);

	private:
		Ptr<VRAMNode> FindNode(uint64 offset);
		void		  SplitNode(Ptr<VRAMNode> target, uint64 size);
		Ptr<VRAMNode> MergeNode(Ptr<VRAMNode> target);
		Ptr<VRAMNode> MergeWithNext(Ptr<VRAMNode> target);
		Ptr<VRAMNode> MergeWithPrev(Ptr<VRAMNode> target);

	public:
		uint64 GetUsedMemory() const { return m_usedMemory; }
		uint64 GetTotalMemory() const { return m_totalSize; }

	private:
		Ptr<VRAMNode> m_head	   { nullptr };
		uint64		  m_totalSize  { 0 };
		uint64		  m_usedMemory { 0 };
	};
}