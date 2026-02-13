#include "EnginePch.h"
#include "Font.h"
#include "Resources/Textures/Texture.h"

Font::Font() = default;
Font::~Font()
{
	if (m_face) FT_Done_Face(m_face);
	if (m_ftLibrary) FT_Done_FreeType(m_ftLibrary);
}

FontPtr Font::Load(const FontDesc& desc)
{
	FontPtr font(new Font());
	font->m_fontSize = desc.fontSize;

	// 1. FreeType 초기화
	if (FT_Init_FreeType(&font->m_ftLibrary))
	{
		LOG_ERROR("Could not init FreeType Library");
		return nullptr;
	}

	// 2. 폰트 파일 로드
	if (FT_New_Face(font->m_ftLibrary, desc.path.c_str(), 0, &font->m_face))
	{
		LOG_ERROR("Failed to load font: {}", desc.path);
		return nullptr;
	}

	// 3. 유니코드 및 사이즈 설정
	FT_Select_Charmap(font->m_face, FT_ENCODING_UNICODE);
	FT_Set_Pixel_Sizes(font->m_face, 0, font->m_fontSize);

	// 4. 텍스처 아틀라스 생성 (1채널 R8 포맷 사용)
	// 초기에는 빈 텍스처(1024x1024)만 생성해둠
	font->m_atlasTexture = Texture::Create
	(
		font->m_atlasWidth,
		font->m_atlasHeight,
		GL_R8,				// Internal Format (GPU 저장용)
		GL_RED,				// Data Format (전송용)
		GL_UNSIGNED_BYTE	// Data Type
	);

	// 5. 텍스처 파라미터 설정
	font->m_atlasTexture->SetFilter(GL_LINEAR, GL_LINEAR);
	font->m_atlasTexture->SetWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

	font->SetName(desc.name);
	font->SetPath(desc.path);

	return font;
}

const GlyphInfo& Font::GetOrLoadGlyph(uint32 codePoint)
{
	// 1. 캐시 검색 (unordered_map - O(1))
	auto it = m_glyphs.find(codePoint);
	if (it != m_glyphs.end()) return it->second;

	// 2. FreeType으로 글자 렌더링
	if (FT_Load_Char(m_face, codePoint, FT_LOAD_RENDER))
	{
		LOG_WARN("Failed to load Glyph: {}", codePoint);
		static GlyphInfo emptyInfo;
		return emptyInfo;
	}

	auto& bitmap = m_face->glyph->bitmap;
	int width = bitmap.width;
	int height = bitmap.rows;

	// 3. 아틀라스 위치 계산 (Simple Line Packing)
	// 오른쪽 공간이 부족하면 다음 줄로
	if (m_cursorX + width + 1 >= m_atlasWidth)
	{
		m_cursorX = 0;
		m_cursorY += m_currentRowHeight + 1; // 1px 패딩
		m_currentRowHeight = 0;
	}

	// 세로 공간도 부족하면 (아틀라스 꽉 참)
	if (m_cursorY + height + 1 >= m_atlasHeight)
	{
		LOG_ERROR("Font Atlas Texture is Full! Cannot add character: {}", codePoint);
		static GlyphInfo emptyInfo;
		return emptyInfo; // 혹은 기존 텍스처를 리사이징하거나 캐시를 비우는 정책 필요
	}

	// 4. 텍스처 부분 업데이트 (SetSubData)
	if (width > 0 && height > 0)
	{
		// 폰트 비트맵은 1바이트 정렬이므로 GL 언팩 정렬 변경
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		m_atlasTexture->SetSubData(m_cursorX, m_cursorY, width, height, bitmap.buffer);

		// 기본값 복구
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	}

	// 5. GlyphInfo 생성 및 등록
	GlyphInfo info;
	info.size = glm::ivec2(width, height);
	info.bearing = glm::ivec2(m_face->glyph->bitmap_left, m_face->glyph->bitmap_top);
	info.advance = (uint32)(m_face->glyph->advance.x >> 6); // 26.6 fixed point -> int

	// UV 좌표 정규화 (0.0 ~ 1.0)
	// 아틀라스 내에서의 픽셀 위치를 0~1 사이 값으로 변환
	info.uvMin = glm::vec2
	(
		(float)m_cursorX / m_atlasWidth,
		(float)m_cursorY / m_atlasHeight
	);
	info.uvMax = glm::vec2
	(
		(float)(m_cursorX + width) / m_atlasWidth,
		(float)(m_cursorY + height) / m_atlasHeight
	);

	// 맵에 저장
	m_glyphs[codePoint] = info;

	// 6. 커서 이동 (+1 패딩)
	m_cursorX += width + 1;
	m_currentRowHeight = (m_currentRowHeight > height) ? m_currentRowHeight : height;

	// 레퍼런스 반환
	return m_glyphs[codePoint];
}