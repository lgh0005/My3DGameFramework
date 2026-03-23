#include "GraphicsPch.h"
#include "FramebufferManager.h"
#include "TextureManager.h"
#include "Framebuffers/GLFramebuffer.h"
#include "Framebuffers/GLFramebuffer2D.h"
#include "Framebuffers/GLFramebuffer3D.h"
#include "Framebuffers/GLFramebufferCube.h"

namespace MGF3D
{
	FramebufferManager::FramebufferManager() = default;
	FramebufferManager::~FramebufferManager()
	{
		Shutdown();
	}

	bool FramebufferManager::Init()
	{
		// 1. 전역 그래픽 설정
		glEnable(GL_FRAMEBUFFER_SRGB);

		// 2. GPU 하드웨어 한계치 쿼리 및 캐싱
		glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &m_maxColorAttachments);
		glGetIntegerv(GL_MAX_FRAMEBUFFER_WIDTH, &m_maxFramebufferWidth);
		glGetIntegerv(GL_MAX_FRAMEBUFFER_HEIGHT, &m_maxFramebufferHeight);
		glGetIntegerv(GL_MAX_FRAMEBUFFER_SAMPLES, &m_maxFramebufferSamples);
		
		return true;
	}

	void FramebufferManager::Shutdown()
	{
		Flush();
	}

	void FramebufferManager::Flush()
	{
		// 해상도 변경이나 씬 전환 시 호출하여 캐싱된 프레임버퍼 비우기
		m_pool2D.Clear();
		m_pool3D.Clear();
		m_poolCube.Clear();
		m_currentBoundFBOHandle = 0;
	}

	void FramebufferManager::Bind(const GLFramebufferPtr& fbo)
	{
		if (fbo == nullptr)
		{
			Unbind();
			return;
		}

		uint32 handle = fbo->GetHandle();

		// 중복 바인딩 방지 캐싱 로직
		if (m_currentBoundFBOHandle != handle)
		{
			fbo->Bind();
			m_currentBoundFBOHandle = handle;
		}
	}

	void FramebufferManager::Unbind()
	{
		if (m_currentBoundFBOHandle)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			m_currentBoundFBOHandle = 0;
		}
	}

	void FramebufferManager::Release(GLFramebufferPtr fbo)
	{
		if (fbo == nullptr) return;

		switch (static_cast<FramebufferType>(fbo->GetTarget()))
		{
			case FramebufferType::FRAMEBUFFER_2D:
			{
				m_pool2D.Release(StaticSharedCast<GLFramebuffer2D>(fbo));
				break;
			}

			case FramebufferType::FRAMEBUFFER_3D:
			{
				m_pool3D.Release(StaticSharedCast<GLFramebuffer3D>(fbo));
				break;
			}

			case FramebufferType::FRAMEBUFFER_CUBE:
			{
				m_poolCube.Release(StaticSharedCast<GLFramebufferCube>(fbo));
				break;
			}
		}
	}

	/*=================================//
	//   Framebuffer request methods   //
	//=================================*/
	GLFramebuffer2DPtr FramebufferManager::RequestFramebuffer2D
	(
		uint32 width, uint32 height, 
		const SVector<uint32>& colorFormats, uint32 depthStencilFormat, 
		uint32 samples
	)
	{
		if (colorFormats.Count() > static_cast<usize>(m_maxColorAttachments))
		{
			MGF_LOG_ERROR("FramebufferManager: Requested color attachments exceed hardware limits.");
			return nullptr;
		}

		uint32 safeWidth = Math::Min(width, static_cast<uint32>(m_maxFramebufferWidth));
		uint32 safeHeight = Math::Min(height, static_cast<uint32>(m_maxFramebufferHeight));
		uint32 safeSamples = Math::Min(samples, static_cast<uint32>(m_maxFramebufferSamples));
		return m_pool2D.Request(safeWidth, safeHeight, colorFormats, depthStencilFormat, safeSamples);
	}

	GLFramebuffer3DPtr FramebufferManager::RequestFramebuffer3D
	(
		uint32 width, uint32 height, uint32 depth, 
		const SVector<uint32>& colorFormats, uint32 depthStencilFormat, 
		uint32 samples
	)
	{
		if (colorFormats.Count() > static_cast<usize>(m_maxColorAttachments))
		{
			MGF_LOG_ERROR("FramebufferManager: Requested color attachments exceed hardware limits.");
			return nullptr;
		}
		uint32 safeWidth = Math::Min(width, static_cast<uint32>(m_maxFramebufferWidth));
		uint32 safeHeight = Math::Min(height, static_cast<uint32>(m_maxFramebufferHeight));
		uint32 safeDepth = Math::Min(depth, static_cast<uint32>(MGF_TEXTURE.GetMax3DTextureSize()));
		uint32 safeSamples = Math::Min(samples, static_cast<uint32>(m_maxFramebufferSamples));
		return m_pool3D.Request(safeWidth, safeHeight, safeDepth, colorFormats, depthStencilFormat, safeSamples);
	}

	GLFramebufferCubePtr FramebufferManager::RequestFramebufferCube
	(
		uint32 size, 
		const SVector<uint32>& colorFormats, uint32 depthStencilFormat, 
		uint32 samples
	)
	{
		if (colorFormats.Count() > static_cast<usize>(m_maxColorAttachments))
		{
			MGF_LOG_ERROR("FramebufferManager: Requested color attachments exceed hardware limits.");
			return nullptr;
		}
		uint32 safeSize = Math::Min(size, static_cast<uint32>(m_maxFramebufferWidth));
		uint32 safeSamples = Math::Min(samples, static_cast<uint32>(m_maxFramebufferSamples));
		return m_poolCube.Request(safeSize, colorFormats, depthStencilFormat, safeSamples);
	}
}
