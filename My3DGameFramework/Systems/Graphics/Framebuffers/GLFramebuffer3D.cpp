#include "GraphicsPch.h"
#include "GLFramebuffer3D.h"
#include "Textures/GLTexture3D.h"
#include "Hashing/FramebufferHash.h"
#include "Managers/TextureManager.h"

namespace MGF3D
{
	GLFramebuffer3D::GLFramebuffer3D() = default;
	GLFramebuffer3D::~GLFramebuffer3D()
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

	GLFramebuffer3DPtr GLFramebuffer3D::Create
	(
		uint32 width, uint32 height, uint32 depth,
		const SVector<uint32>& colorFormats,
		uint32 depthStencilFormat,
		uint32 samples,
		uint32 handle
	)
	{
		auto fbo = GLFramebuffer3DPtr(new GLFramebuffer3D());
		if (!fbo->Init(width, height, depth, colorFormats, depthStencilFormat, samples, handle)) return nullptr;
		return fbo;
	}

	bool GLFramebuffer3D::Init
	(
		uint32 width, uint32 height, uint32 depth,
		const SVector<uint32>& colorFormats,
		uint32 depthStencilFormat,
		uint32 samples,
		uint32 handle
	)
	{
		m_width = width;
		m_height = height;
		m_depth = depth;
		m_colorFormats = colorFormats;
		m_depthStencilFormat = depthStencilFormat;
		m_samples = samples;

		if (handle == 0)
		{
			glCreateFramebuffers(1, &m_handle);
			if (m_handle == 0) return false;
		}
		else m_handle = handle;

		// 3D 프레임버퍼이므로 m_depth를 포함하여 해시를 생성합니다.
		m_hash = FramebufferHash(m_width, m_height, m_colorFormats, m_depthStencilFormat, m_samples, m_depth);

		RefreshAttachments();
		return true;
	}

	void GLFramebuffer3D::Bind() const
	{
		if (m_handle)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, m_handle);
			glViewport(0, 0, (GLsizei)m_width, (GLsizei)m_height);
		}
	}

	void GLFramebuffer3D::BindSlice(uint32 slice) const
	{
		if (m_handle == 0) return;

		glBindFramebuffer(GL_FRAMEBUFFER, m_handle);
		glViewport(0, 0, (GLsizei)m_width, (GLsizei)m_height);

		// DSA 방식을 사용하여 특정 레이어(slice)를 어태치먼트에 바인딩합니다.
		for (uint32 i = 0; i < m_colorTextures.Count(); ++i)
			glNamedFramebufferTextureLayer(m_handle, GL_COLOR_ATTACHMENT0 + i, m_colorTextures[i]->GetHandle(), 0, slice);

		if (m_depthStencilTexture)
			glNamedFramebufferTextureLayer(m_handle, GL_DEPTH_STENCIL_ATTACHMENT, m_depthStencilTexture->GetHandle(), 0, slice);
	}

	void GLFramebuffer3D::Unbind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void GLFramebuffer3D::Resize(uint32 w, uint32 h)
	{
		Resize(w, h, m_depth);
	}

	void GLFramebuffer3D::Resize(uint32 w, uint32 h, uint32 d)
	{
		if (m_width == w && m_height == h && m_depth == d) return;

		m_width = w;
		m_height = h;
		m_depth = d;

		m_hash = FramebufferHash(m_width, m_height, m_colorFormats, m_depthStencilFormat, m_samples, m_depth);

		RefreshAttachments();
	}

	void GLFramebuffer3D::Blit
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

	GLTexture3DPtr GLFramebuffer3D::GetColorAttachment(uint32 index) const
	{
		if (index >= m_colorTextures.Count()) return nullptr;
		return m_colorTextures[index];
	}

	GLTexture3DPtr GLFramebuffer3D::GetDepthStencilAttachment() const
	{
		return m_depthStencilTexture;
	}

	void GLFramebuffer3D::RefreshAttachments()
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
			auto tex = MGF_TEXTURE.RequestTexture3D(m_width, m_height, m_depth, m_colorFormats[i]);
			m_colorTextures.PushBack(tex);

			uint32 attachmentSlot = GL_COLOR_ATTACHMENT0 + i;

			// 전체 볼륨을 한 번에 프레임버퍼에 연결합니다.
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
			m_depthStencilTexture = MGF_TEXTURE.RequestTexture3D(m_width, m_height, m_depth, m_depthStencilFormat);
			uint32 attachmentType = GL_DEPTH_STENCIL_ATTACHMENT;
			glNamedFramebufferTexture(m_handle, attachmentType, m_depthStencilTexture->GetHandle(), 0);
		}

		uint32 status = glCheckNamedFramebufferStatus(m_handle, GL_FRAMEBUFFER);
		MGF_ASSERT(status == GL_FRAMEBUFFER_COMPLETE, "Framebuffer 3D DSA Check Failed!");
	}

	Recti GLFramebuffer3D::GetBlitArea
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

		auto fbo3D = StaticSharedCast<GLFramebuffer3D>(fbo);
		MGF_ASSERT(fbo3D != nullptr, "GetBlitArea: Failed to cast GLFramebuffer to GLFramebuffer3D.");

		return Recti
		(
			0, 0,
			static_cast<int32>(fbo3D->GetWidth()),
			static_cast<int32>(fbo3D->GetHeight())
		);
	}
}