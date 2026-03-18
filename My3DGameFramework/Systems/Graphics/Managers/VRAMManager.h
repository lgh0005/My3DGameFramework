#pragma once
#include "VRAM/VRAMAllocation.h"

namespace MGF3D
{
	MGF_CLASS_PTR(VRAMPool)

	class VRAMManager
	{
		MGF_DECLARE_SINGLE(VRAMManager)

	private:
		VRAMManager();
		~VRAMManager();

	public:
		bool Init(uint64 staticPoolSize, uint64 dynamicPoolSize);
		void Shutdown();

	public:
		VRAMAllocation Allocate
		(
			VRAMAllocation::PoolType type, 
			uint64 size, 
			uint32 alignment
		);
		void Deallocate(VRAMAllocation& alloc);
		void UploadData(const VRAMAllocation& alloc, const void* data);

	/*=================================================//
	//   Internal VRAM Allocate & Deallocate methods   //
	//=================================================*/
	private:
		VRAMAllocation AllocateFromPool
		(
			Ptr<VRAMPool> pool,
			VRAMAllocation::PoolType type,
			uint64 size,
			uint32 alignment
		);

		void DeallocateFromPool
		(
			Ptr<VRAMPool> pool, 
			uint64 offset
		);

		void UploadToPool
		(
			Ptr<VRAMPool> pool, 
			uint64 offset, 
			const void* data, 
			uint64 size
		);

	/*====================================//
	//   VRAMPool usage profile methods   //
	//====================================*/
	public:
		uint64 GetStaticPoolUsed() const;
		uint64 GetStaticPoolTotal() const;
		uint64 GetDynamicPoolUsed() const;
		uint64 GetDynamicPoolTotal() const;

	private:
		VRAMPoolUPtr m_staticVRAMPool	{ nullptr };
		VRAMPoolUPtr m_dynamicVRAMPool	{ nullptr };
	};
}