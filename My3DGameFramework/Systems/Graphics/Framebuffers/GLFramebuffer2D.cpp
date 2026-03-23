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
		// 1. 부착되어 있던 색상 텍스쳐들을 풀로 반납
		for (auto& tex : m_colorTextures)
			MGF_TEXTURE.Release(tex);
		m_colorTextures.Clear();

		// 2. 부착되어 있던 깊이/스텐실 텍스쳐를 풀로 반납
		if (m_depthStencilTexture != nullptr)
		{
			MGF_TEXTURE.Release(m_depthStencilTexture);
			m_depthStencilTexture = nullptr;
		}

		// 3. 프레임버퍼 핸들 파괴
		if (m_handle)
		{
			glDeleteFramebuffers(1, &m_handle);
			m_handle = 0;
		}
	}

	GLFramebuffer2DPtr GLFramebuffer2D::Create
	(
		uint32 width, uint32 height,
		const SVector<uint32>& colorFormats,
		uint32 depthStencilFormat,
		uint32 samples,
		uint32 handle
	)
	{
		auto fbo = GLFramebuffer2DPtr(new GLFramebuffer2D());
		if (!fbo->Init(width, height, colorFormats, depthStencilFormat, samples, handle)) return nullptr;
		return fbo;
	}

	bool GLFramebuffer2D::Init
	(
		uint32 width, uint32 height,
		const SVector<uint32>& colorFormats,
		uint32 depthStencilFormat,
		uint32 samples,
		uint32 handle
	)
	{
		// 0. 레이아웃 정보 저장
		m_width = width;
		m_height = height;
		m_colorFormats = colorFormats;
		m_depthStencilFormat = depthStencilFormat;
		m_samples = samples;

		// 1. 재사용 핸들 여부 검사
		if (handle == 0)
		{
			glCreateFramebuffers(1, &m_handle);
			if (m_handle == 0) return false;
		}
		else m_handle = handle;

		// 2. 레이아웃 기반 해시 생성
		m_hash = FramebufferHash(m_width, m_height, m_colorFormats, m_depthStencilFormat, m_samples);

		// 3. 텍스처 구성 시작
		RefreshAttachments();
		return true;
	}

	void GLFramebuffer2D::Bind() const
	{
		if (m_handle)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, m_handle);
			glViewport(0, 0, (GLsizei)m_width, (GLsizei)m_height);
		}
	}

	void GLFramebuffer2D::Unbind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void GLFramebuffer2D::Resize(uint32 w, uint32 h)
	{
		if (m_width == w && m_height == h) return;

		m_width = w;
		m_height = h;
		m_hash = FramebufferHash(m_width, m_height, m_colorFormats, m_depthStencilFormat, m_samples);

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
		for (auto& tex : m_colorTextures) MGF_TEXTURE.Release(tex);
		m_colorTextures.Clear();

		if (m_depthStencilTexture != nullptr)
		{
			MGF_TEXTURE.Release(m_depthStencilTexture);
			m_depthStencilTexture = nullptr;
		}

		// 2. 컬러 어태치먼트 생성 및 부착
		LVector<uint32> drawBuffers;
		for (uint32 i = 0; i < m_colorFormats.Count(); ++i)
		{
			// TextureManager를 통해 텍스처 요청
			auto tex = MGF_TEXTURE.RequestTexture2D(m_width, m_height, m_colorFormats[i]);
			m_colorTextures.PushBack(tex);

			uint32 attachmentSlot = GL_COLOR_ATTACHMENT0 + i;
			glNamedFramebufferTexture(m_handle, attachmentSlot, tex->GetHandle(), 0);
			drawBuffers.PushBack(attachmentSlot);
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

		// 4. 깊이/스텐실 어태치먼트 부착
		if (m_depthStencilFormat)
		{
			m_depthStencilTexture = MGF_TEXTURE.RequestTexture2D(m_width, m_height, m_depthStencilFormat);
			uint32 attachmentType = GL_DEPTH_STENCIL_ATTACHMENT;
			glNamedFramebufferTexture(m_handle, attachmentType, m_depthStencilTexture->GetHandle(), 0);
		}

		// 5. 프레임버퍼 완성 상태 검증
		uint32 status = glCheckNamedFramebufferStatus(m_handle, GL_FRAMEBUFFER);
		MGF_ASSERT(status == GL_FRAMEBUFFER_COMPLETE, "Framebuffer DSA Check Failed!");
	}

	Recti GLFramebuffer2D::GetBlitArea
	(
		const GLFramebufferPtr& fbo, 
		const Ptr<Recti>& rect
	) const
	{
		// 0. 사용자 정의 영역이 있으면 그것을 즉시 반환
		if (rect) return *rect;

		// 1. fbo가 nullptr인 경우 기본 화면(Default Framebuffer)으로 간주
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

		// 2. GLFramebuffer2D 획득
		auto fbo2D = StaticSharedCast<GLFramebuffer2D>(fbo);
		MGF_ASSERT
		(
			fbo2D != nullptr, 
			"GetBlitArea: Failed to cast GLFramebuffer to GLFramebuffer2D."
		);

		// 3. Blit 영역 반환
		return Recti
		(
			0, 0,
			static_cast<int32>(fbo2D->GetWidth()),
			static_cast<int32>(fbo2D->GetHeight())
		);
	}
}