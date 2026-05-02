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

	bool GLTexture2DArray::Init
	(
		uint32 width,
		uint32 height,
		uint32 layers
	)
	{
		if (width == 0 || height == 0 || layers == 0) return false;

		m_width = width;
		m_height = height;
		m_layers = layers;

		// 1. DSA 기반 텍스처 핸들 생성
		glCreateTextures(m_target, 1, &m_handle);
		if (m_handle == 0) return false;

		// 2. Immutable Storage 할당 (3D 함수를 사용하여 2D 레이어들을 적층)
		// 깊이 정밀도를 위해 GL_DEPTH_COMPONENT32F 사용
		glTextureStorage3D(m_handle, 1, GL_DEPTH_COMPONENT32F, m_width, m_height, m_layers);

		// 3. 샘플링 필터 설정 (그림자 맵은 보통 NEAREST 혹은 하드웨어 PCF를 위한 LINEAR 사용)
		glTextureParameteri(m_handle, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(m_handle, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		// 4. 래핑 설정: 범위를 벗어난 곳은 그림자가 생기지 않도록 Border를 1.0(최대 깊이)으로 고정
		glTextureParameteri(m_handle, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTextureParameteri(m_handle, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

		const float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTextureParameterfv(m_handle, GL_TEXTURE_BORDER_COLOR, borderColor);

		// 5. 하드웨어 Depth Comparison 활성화 (sampler2DArrayShadow 사용 가능)
		glTextureParameteri(m_handle, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
		glTextureParameteri(m_handle, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

		return true;
	}
}