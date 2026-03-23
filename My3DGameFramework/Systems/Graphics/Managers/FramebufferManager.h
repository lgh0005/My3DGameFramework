#pragma once
#include "Pools/Framebuffer2DPool.h" 
#include "Pools/Framebuffer3DPool.h"
#include "Pools/FramebufferCubePool.h"

namespace MGF3D
{
	MGF_CLASS_PTR(GLFramebuffer)
	MGF_CLASS_PTR(GLFramebuffer2D)
	MGF_CLASS_PTR(GLFramebuffer3D)
	MGF_CLASS_PTR(GLFramebufferCube)

	class FramebufferManager
	{
		MGF_DECLARE_SINGLE(FramebufferManager)

	private:
		FramebufferManager();
		~FramebufferManager();

	public:
		bool Init();
		void Shutdown();
		void Flush();

	public:
		void Bind(const GLFramebufferPtr& fbo);
		void Unbind();
		void Release(GLFramebufferPtr fbo);

	/*=================================//
	//   Framebuffer request methods   //
	//=================================*/
	public:
		GLFramebuffer2DPtr RequestFramebuffer2D
		(
			uint32 width, uint32 height,
			const SVector<uint32>& colorFormats,
			uint32 depthStencilFormat = 0,
			uint32 samples = 1
		);

		GLFramebuffer3DPtr RequestFramebuffer3D
		(
			uint32 width, uint32 height, uint32 depth,
			const SVector<uint32>& colorFormats,
			uint32 depthStencilFormat = 0,
			uint32 samples = 1
		);

		GLFramebufferCubePtr RequestFramebufferCube
		(
			uint32 size,
			const SVector<uint32>& colorFormats,
			uint32 depthStencilFormat = 0,
			uint32 samples = 1
		);

	public:
		int32 GetMaxColorAttachments() const { return m_maxColorAttachments; }
		int32 GetMaxFramebufferWidth() const { return m_maxFramebufferWidth; }
		int32 GetMaxFramebufferHeight() const { return m_maxFramebufferHeight; }
		int32 GetMaxFramebufferSamples() const { return m_maxFramebufferSamples; }

	private:
		uint32 m_currentBoundFBOHandle{ 0 };
		Framebuffer2DPool   m_pool2D;
		Framebuffer3DPool   m_pool3D;
		FramebufferCubePool m_poolCube;

		int32 m_maxColorAttachments{ 0 };
		int32 m_maxFramebufferWidth{ 0 };
		int32 m_maxFramebufferHeight{ 0 };
		int32 m_maxFramebufferSamples{ 0 };
	};
}