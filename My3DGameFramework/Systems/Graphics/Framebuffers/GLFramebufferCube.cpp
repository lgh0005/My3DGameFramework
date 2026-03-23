#include "GraphicsPch.h"
#include "GLFramebufferCube.h"
#include "Textures/GLTextureCube.h"
#include "Hashing/FramebufferHash.h"
#include "Managers/TextureManager.h"

namespace MGF3D
{
	GLFramebufferCube::GLFramebufferCube() = default;
	GLFramebufferCube::~GLFramebufferCube()
	{
		for (auto& tex : m_colorTextures) MGF_TEXTURE.Release(tex);
		m_colorTextures.Clear();

		if (m_depthStencilTexture != nullptr)
		{
			MGF_TEXTURE.Release(m_depthStencilTexture);
			m_depthStencilTexture = nullptr;
		}

		if (m_handle)
		{
			glDeleteFramebuffers(1, &m_handle);
			m_handle = 0;
		}
	}

	GLFramebufferCubePtr GLFramebufferCube::Create
	(
		uint32 size,
		const SVector<uint32>& colorFormats,
		uint32 depthStencilFormat,
		uint32 samples,
		uint32 handle
	)
	{
		auto fbo = GLFramebufferCubePtr(new GLFramebufferCube());
		if (!fbo->Init(size, colorFormats, depthStencilFormat, samples, handle)) return nullptr;
		return fbo;
	}

	bool GLFramebufferCube::Init
	(
		uint32 size,
		const SVector<uint32>& colorFormats,
		uint32 depthStencilFormat,
		uint32 samples,
		uint32 handle
	)
	{
		m_size = size;
		m_colorFormats = colorFormats;
		m_depthStencilFormat = depthStencilFormat;
		m_samples = samples;

		if (handle == 0)
		{
			glCreateFramebuffers(1, &m_handle);
			if (m_handle == 0) return false;
		}
		else m_handle = handle;

		// 큐브맵은 6개의 면을 가지므로 depth 자리에 6을 전달하여 해시를 분리합니다.
		m_hash = FramebufferHash(m_size, m_size, m_colorFormats, m_depthStencilFormat, m_samples, 6);

		RefreshAttachments();
		return true;
	}

	void GLFramebufferCube::Bind() const
	{
		if (m_handle)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, m_handle);
			glViewport(0, 0, (GLsizei)m_size, (GLsizei)m_size);
		}
	}

	void GLFramebufferCube::BindFace(uint32 face) const
	{
		if (m_handle == 0) return;

		glBindFramebuffer(GL_FRAMEBUFFER, m_handle);
		glViewport(0, 0, (GLsizei)m_size, (GLsizei)m_size);

		// 특정 면(Face)만 프레임버퍼에 연결하여 렌더링 타겟으로 설정합니다.
		for (uint32 i = 0; i < m_colorTextures.Count(); ++i)
			glNamedFramebufferTextureLayer(m_handle, GL_COLOR_ATTACHMENT0 + i, m_colorTextures[i]->GetHandle(), 0, face);

		if (m_depthStencilTexture)
			glNamedFramebufferTextureLayer(m_handle, GL_DEPTH_STENCIL_ATTACHMENT, m_depthStencilTexture->GetHandle(), 0, face);
	}

	void GLFramebufferCube::Unbind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void GLFramebufferCube::Resize(uint32 w, uint32 h)
	{
		// 큐브맵은 항상 정사각형을 유지해야 하므로 큰 값을 기준으로 잡습니다.
		uint32 newSize = Math::Max(w, h);
		Resize(newSize);
	}

	void GLFramebufferCube::Resize(uint32 size)
	{
		if (m_size == size) return;

		m_size = size;
		m_hash = FramebufferHash(m_size, m_size, m_colorFormats, m_depthStencilFormat, m_samples, 6);

		RefreshAttachments();
	}

	void GLFramebufferCube::Blit
	(
		const GLFramebufferPtr& src, const GLFramebufferPtr& dst,
		const Ptr<Recti> srcRect, const Ptr<Recti> dstRect,
		uint32 mask, uint32 filter
	)
	{
		MGF_ASSERT(src != nullptr, "Blit: Source framebuffer is null.");

		uint32 srcHandle = src->GetHandle();
		uint32 dstHandle = dst->GetHandle();

		Recti s = GetBlitArea(src, srcRect);
		Recti d = GetBlitArea(dst, dstRect);

		glBlitNamedFramebuffer
		(
			srcHandle, dstHandle,
			s.x0, s.y0, s.x1, s.y1,
			d.x0, d.y0, d.x1, d.y1,
			mask, filter
		);
	}

	GLTextureCubePtr GLFramebufferCube::GetColorAttachment(uint32 index) const
	{
		if (index >= m_colorTextures.Count()) return nullptr;
		return m_colorTextures[index];
	}

	GLTextureCubePtr GLFramebufferCube::GetDepthStencilAttachment() const
	{
		return m_depthStencilTexture;
	}

	void GLFramebufferCube::RefreshAttachments()
	{
		if (m_handle == 0) return;

		for (auto& tex : m_colorTextures) MGF_TEXTURE.Release(tex);
		m_colorTextures.Clear();

		if (m_depthStencilTexture != nullptr)
		{
			MGF_TEXTURE.Release(m_depthStencilTexture);
			m_depthStencilTexture = nullptr;
		}

		LVector<uint32> drawBuffers;
		for (uint32 i = 0; i < m_colorFormats.Count(); ++i)
		{
			auto tex = MGF_TEXTURE.RequestTextureCube(m_size, m_colorFormats[i]);
			m_colorTextures.PushBack(tex);

			uint32 attachmentSlot = GL_COLOR_ATTACHMENT0 + i;

			// 기본 부착 시에는 전체 면을 부착하여 지오메트리 셰이더를 통한 단일 패스 렌더링에 대비합니다.
			glNamedFramebufferTexture(m_handle, attachmentSlot, tex->GetHandle(), 0);
			drawBuffers.PushBack(attachmentSlot);
		}

		if (!drawBuffers.Empty())
		{
			glNamedFramebufferDrawBuffers(m_handle, static_cast<GLsizei>(drawBuffers.Count()), drawBuffers.Data());
		}
		else
		{
			glNamedFramebufferDrawBuffer(m_handle, GL_NONE);
			glNamedFramebufferReadBuffer(m_handle, GL_NONE);
		}

		if (m_depthStencilFormat)
		{
			m_depthStencilTexture = MGF_TEXTURE.RequestTextureCube(m_size, m_depthStencilFormat);
			uint32 attachmentType = GL_DEPTH_STENCIL_ATTACHMENT;
			glNamedFramebufferTexture(m_handle, attachmentType, m_depthStencilTexture->GetHandle(), 0);
		}

		uint32 status = glCheckNamedFramebufferStatus(m_handle, GL_FRAMEBUFFER);
		MGF_ASSERT(status == GL_FRAMEBUFFER_COMPLETE, "Framebuffer Cube DSA Check Failed!");
	}

	Recti GLFramebufferCube::GetBlitArea
	(
		const GLFramebufferPtr& fbo,
		const Ptr<Recti>& rect
	) const
	{
		if (rect) return *rect;

		if (fbo == nullptr)
		{
			// TODO: 엔진의 WindowManager나 Application 클래스가 있다면 
			// 윈도우의 해상도를 가져오는 것이 가장 정확합니다.
			// (예: return Recti(0, 0, MGF_WINDOW.GetWidth(), MGF_WINDOW.GetHeight());)

			// 전역 접근자가 아직 없다면, 현재 설정된 OpenGL 뷰포트를 읽어올 수 있습니다.
			//int32 viewport[4];
			//glGetIntegerv(GL_VIEWPORT, viewport);
			//return Recti(0, 0, viewport[2], viewport[3]); 
			return Recti();
		}

		auto fboCube = StaticSharedCast<GLFramebufferCube>(fbo);
		MGF_ASSERT(fboCube != nullptr, "GetBlitArea: Failed to cast GLFramebuffer to GLFramebufferCube.");

		return Recti
		(
			0, 0,
			static_cast<int32>(fboCube->GetSize()),
			static_cast<int32>(fboCube->GetSize())
		);
	}
}