#include "GraphicsPch.h"
#include "GLFramebuffer2D.h"
#include "Textures/GLTexture2D.h"
#include "Hashing/FramebufferHash.h"
#include "Managers/TextureManager.h"

namespace MGF3D
{
	GLFramebuffer2D::GLFramebuffer2D() = default;
	GLFramebuffer2D::~GLFramebuffer2D()
	{
		if (m_handle)
		{
			glDeleteFramebuffers(1, &m_handle);
			m_handle = 0;
		}
	}

	GLFramebuffer2DPtr GLFramebuffer2D::Create(const FramebufferLayout& layout, uint32 handle)
	{
		auto fbo = GLFramebuffer2DPtr(new GLFramebuffer2D());
		if (!fbo->Init(layout, handle)) return nullptr;
		return fbo;
	}

	bool GLFramebuffer2D::Init(const FramebufferLayout& layout, uint32 handle)
	{
		// 0. 레이아웃 정보 저장
		m_layout = layout;

		// 1. 재사용 핸들 여부 검사
		if (handle == 0)
		{
			glCreateFramebuffers(1, &m_handle);
			if (m_handle == 0) return false;
		}
		else m_handle = handle;

		// 2. 레이아웃 기반 해시 생성
		m_hash = FramebufferHash(m_layout);

		// 3. 텍스처 구성 시작
		RefreshAttachments();
		return true;
	}

	void GLFramebuffer2D::Bind() const
	{
		if (m_handle)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, m_handle);
			glViewport(0, 0, (GLsizei)m_layout.width, (GLsizei)m_layout.height);
		}
	}

	void GLFramebuffer2D::Unbind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void GLFramebuffer2D::Resize(uint32 w, uint32 h)
	{
		if (m_layout.width == w && m_layout.height == h) return;

		m_layout.width = w;
		m_layout.height = h;
		m_hash = FramebufferHash(m_layout);

		RefreshAttachments();
	}

	void GLFramebuffer2D::Blit
	(
		const GLFramebufferPtr& src, const GLFramebufferPtr& dst, 
		const Ptr<Recti> srcRect, const Ptr<Recti> dstRect, 
		uint32 mask, uint32 filter
	)
	{
		// 1. 유효성 및 타입 검증
		MGF_ASSERT(src != nullptr, "Blit: Source framebuffer is null.");
		MGF_ASSERT(dst != nullptr, "Blit: Destination framebuffer is null.");

		// 2. 영역 추출 (내부에서 static_pointer_cast 수행)
		Recti s = GetBlitArea(src, srcRect);
		Recti d = GetBlitArea(dst, dstRect);

		// 3. 실제 OpenGL Blit 수행
		glBlitNamedFramebuffer
		(
			src->GetHandle(), dst->GetHandle(),
			s.x0, s.y0, s.x1, s.y1,
			d.x0, d.y0, d.x1, d.y1,
			mask, filter
		);
	}

	GLTexture2DPtr GLFramebuffer2D::GetColorAttachment(uint32 index) const
	{
		if (index >= m_colorTextures.Count()) return nullptr;
		return m_colorTextures[index];
	}

	GLTexture2DPtr GLFramebuffer2D::GetDepthStencilAttachment() const
	{
		return m_depthStencilTexture;
	}

	void GLFramebuffer2D::RefreshAttachments()
	{
		if (m_handle == 0) return;

		// 1. 기존 어태치먼트 정리 (TextureManager 풀로 반납되도록 유도)
		m_colorTextures.Clear();
		m_depthStencilTexture = nullptr;

		// 2. 컬러 어태치먼트 생성 및 부착
		LVector<uint32> drawBuffers;
		for (uint32 i = 0; i < m_layout.colorFormats.Count(); ++i)
		{
			//// TextureManager를 통해 텍스처 요청
			//auto tex = MGF_TEXTURE->Request2D(m_layout.Width, m_layout.Height, m_layout.ColorFormats[i]);
			//m_colorTextures.push_back(tex);

			//uint32 attachmentSlot = GL_COLOR_ATTACHMENT0 + i;
			//glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentSlot, GL_TEXTURE_2D, tex->GetHandle(), 0);
			//drawBuffers.PushBack(attachmentSlot);
		}

		// 3. MRT 활성화
		if (!drawBuffers.Empty())
		{
			glNamedFramebufferDrawBuffers(m_handle, (GLsizei)drawBuffers.Count(), drawBuffers.Data());
		}
		else
		{
			// 컬러 어태치먼트가 없는 경우 (Shadow Map 등)
			glNamedFramebufferDrawBuffer(m_handle, GL_NONE);
			glNamedFramebufferReadBuffer(m_handle, GL_NONE);
		}

		// 3. 깊이/스텐실 어태치먼트 부착
		if (m_layout.depthStencilFormat)
		{
			//m_depthStencilTexture = MGF_TEXTURE->Request2D(m_layout.Width, m_layout.Height, m_layout.DepthStencilFormat);
			
			// 포맷에 따라 적절한 슬롯 선택
			// GLenum attachmentType = GL_DEPTH_STENCIL_ATTACHMENT;
			// 만약 순수 Depth 포맷인 경우 처리 로직을 확장할 수 있음

			//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_depthStencilTexture->GetHandle(), 0);
		}

		// 상태 검증
		//GLenum status = glCheckNamedFramebufferStatus(m_handle, GL_FRAMEBUFFER);
		//if (status != GL_FRAMEBUFFER_COMPLETE)
		//{
		//	MGF_LOG_ERROR("Framebuffer DSA Check Failed! Handle: {0}, Status: 0x{1:X}", m_handle, status);
		//}
	}
	Recti GLFramebuffer2D::GetBlitArea
	(
		const GLFramebufferPtr& fbo, 
		const Ptr<Recti>& rect
	) const
	{
		// 0. 사용자 정의 영역이 있으면 그것을 즉시 반환
		if (rect) return *rect;

		// 1. GLFramebuffer2D 획득
		auto fbo2D = StaticSharedCast<GLFramebuffer2D>(fbo);
		MGF_ASSERT
		(
			fbo2D != nullptr, 
			"GetBlitArea: Failed to cast GLFramebuffer to GLFramebuffer2D."
		);

		return Recti
		(
			0, 0,
			static_cast<int32>(fbo2D->GetWidth()),
			static_cast<int32>(fbo2D->GetHeight())
		);
	}
}