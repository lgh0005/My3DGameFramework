#include "GraphicsPch.h"
#include "GLTexture2DArray.h"

namespace MGF3D
{
	GLTexture2DArray::GLTexture2DArray() = default;
	GLTexture2DArray::~GLTexture2DArray() = default;

	GLTexture2DArrayUPtr GLTexture2DArray::Create
	(
		uint32 width,
		uint32 height,
		uint32 layers
	)
	{
		auto textureArray = GLTexture2DArrayUPtr(new GLTexture2DArray());
		if (!textureArray->Init(width, height, layers)) return nullptr;
		return textureArray;
	}

	bool GLTexture2DArray::Init(uint32 width, uint32 height, uint32 layers)
	{
		m_width = width;
		m_height = height;
		m_layers = layers;

		// 초기 상태 설정
		SetState(EResourceState::Empty);
		return true;
	}

	bool GLTexture2DArray::OnSyncCreate()
	{
		if (m_target == 0) m_target = GL_TEXTURE_2D_ARRAY;

		// 1. DSA 방식으로 텍스처 생성
		glCreateTextures(m_target, 1, &m_handle);
		if (m_handle == 0) return false;

		// 2. 3D 스토리지 할당 (2D Array는 내부적으로 3D 데이터 구조를 가짐)
		// 그림자 맵용이므로 Depth 전용 포맷(GL_DEPTH_COMPONENT32F) 사용
		glTextureStorage3D(m_handle, 1, GL_DEPTH_COMPONENT32F, m_width, m_height, m_layers);
		GLenum err = glGetError();
		if (err != GL_NO_ERROR) 
		{
			MGF_LOG_ERROR("glTexStorage3D failed: 0x{0:X}, Layers: {1}", err, m_layers);
			return false;
		}

		// 3. 텍스처 파라미터 설정
		glTextureParameteri(m_handle, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_handle, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTextureParameteri(m_handle, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTextureParameteri(m_handle, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

		// Border Color를 흰색(1.0)으로 설정하여 그림자 범위 밖이 밝게 나오도록 함
		float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTextureParameterfv(m_handle, GL_TEXTURE_BORDER_COLOR, borderColor);

		// 4. 언바인딩 및 상태 업데이트
		SetState(EResourceState::Ready);
		return true;
	}
}