#pragma once
#include "Resources/Resource.h"
#include <ft2build.h>
#include FT_FREETYPE_H

#pragma region FORWARD_DECLARATION
CLASS_PTR(Texture)
#pragma endregion

// 글자 하나에 대한 메타 데이터
struct GlyphInfo
{
	glm::vec2 uvMin;		// 텍스처 좌표 (Top-Left)
	glm::vec2 uvMax;		// 텍스처 좌표 (Bottom-Right)
	glm::ivec2 size;		// 글자의 크기 (Pixel)
	glm::ivec2 bearing;		// 기준선(Baseline)으로부터의 오프셋
	uint32 advance;			// 다음 글자까지의 거리 (1/64 Pixel 단위)
};

CLASS_PTR(Font)
class Font : public Resource
{
public:
	virtual ~Font();
	static FontUPtr Load(const std::string& path, uint32 fontSize);
	static const ResourceType s_ResourceType = ResourceType::Font;
	virtual ResourceType GetResourceType() const override { return ResourceType::Font; }

	const GlyphInfo& GetOrLoadGlyph(uint32 codePoint);
	TexturePtr GetAtlasTexture() const { return m_atlasTexture; }
	uint32 GetFontSize() const { return m_fontSize; }

private:
	Font();

	// FreeType 관련
	FT_Library	m_ftLibrary{ nullptr };
	FT_Face		m_face{ nullptr };
	uint32		m_fontSize{ 24 };

	// 텍스처 아틀라스 관련
	TexturePtr	m_atlasTexture;
	int32		m_atlasWidth{ 4096 }; // 기본 4096*4096
	int32		m_atlasHeight{ 4096 };

	// 아틀라스 커서 (빈 공간 추적용)
	int32		m_cursorX{ 0 };
	int32		m_cursorY{ 0 };
	int32		m_currentRowHeight{ 0 };

	// 글자 캐시 (이미 로드된 글자 검색용)
	std::unordered_map<uint32, GlyphInfo> m_glyphs;
};