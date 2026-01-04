#include "EnginePch.h"
#include "UIText.h"
#include "Components/Transform.h"
#include "Resources/UIMesh.h"    // UIMesh 구현체
#include "Resources/Material.h"
#include "Resources/Font.h"
#include "Graphics/Program.h"
#include "Graphics/Vertex.h"

DECLARE_DEFAULTS_IMPL(UIText)

UITextUPtr UIText::Create(FontPtr font, MaterialPtr material, const std::wstring& text, int32 sortingOrder)
{
    auto uiText = UITextUPtr(new UIText());
    if (uiText->Init(font, material, text, sortingOrder)) return std::move(uiText);
    return nullptr;
}

bool UIText::Init(FontPtr font, MaterialPtr material, const std::wstring& text, int32 sortingOrder)
{
    // 1. 부모 초기화 (SortingOrder 설정 등)
    SetSortingOrder(sortingOrder); // 부모 메서드 활용

    // 2. 멤버 변수 설정
    m_font = font;
    m_material = material;
    m_text = text;

    // 3. UIText 전용 Dynamic Mesh 생성
    m_uiMesh = UIMesh::Create();
    RebuildMesh();

    return true;
}

void UIText::RebuildMesh()
{
    // 폰트가 없으면 아무것도 할 수 없음
    if (!m_font) return;

    // TODO : 텍스트가 비어있으면 메쉬를 클리어하고 리턴

    std::vector<UIVertex> vertices;
    std::vector<uint32> indices;

    // 메모리 재할당 방지를 위해 예약 (글자당 정점 4개, 인덱스 6개)
    vertices.reserve(m_text.length() * 4);
    indices.reserve(m_text.length() * 6);

    float cursorX = 0.0f;
    float cursorY = 0.0f;
    float posZ = 0.0f; // 2D UI이므로 Z는 0
    uint32 indexCount = 0;

    for (wchar_t c : m_text)
    {
        // 1. 폰트에서 글자 정보(Glyph) 가져오기
        // GetOrLoadGlyph는 없는 글자일 경우 아틀라스에 새로 그리고 정보를 반환함
        const GlyphInfo& glyph = m_font->GetOrLoadGlyph((uint32)c);

        // 2. 위치 및 크기 계산
        // bearing.x: 기준점(Cursor)에서 글자 이미지 왼쪽까지의 거리
        // bearing.y: 기준선(Baseline)에서 글자 이미지 상단까지의 거리
        // size.y - bearing.y: 기준선 아래로 내려오는 길이 (g, j, p, q, y 등)
        float xpos = cursorX + glyph.bearing.x;
        float ypos = cursorY - (glyph.size.y - glyph.bearing.y);
        float w = (float)glyph.size.x;
        float h = (float)glyph.size.y;

        // 3. 정점 생성 (UIVertex: Position, Color, TexCoord)
        // UIMesh는 UIVertex 구조체를 사용하며, Normal 대신 Color를 포함함.
        // UV 좌표는 Font::GetOrLoadGlyph에서 계산된 아틀라스 UV를 사용.
        vertices.push_back(UIVertex{ glm::vec3(xpos, ypos, posZ), m_color, glm::vec2(glyph.uvMin.x, glyph.uvMax.y) }); // [0] 좌측 하단 (Bottom-Left)
        vertices.push_back(UIVertex{ glm::vec3(xpos + w, ypos, posZ), m_color, glm::vec2(glyph.uvMax.x, glyph.uvMax.y) }); // [1] 우측 하단 (Bottom-Right)
        vertices.push_back(UIVertex{ glm::vec3(xpos + w, ypos + h, posZ), m_color, glm::vec2(glyph.uvMax.x, glyph.uvMin.y) }); // [2] 우측 상단 (Top-Right)
        vertices.push_back(UIVertex{ glm::vec3(xpos, ypos + h, posZ), m_color, glm::vec2(glyph.uvMin.x, glyph.uvMin.y) }); // [3] 좌측 상단 (Top-Left)

        // 4. 인덱스 생성 (CCW - 반시계 방향)
        indices.push_back(indexCount + 0);
        indices.push_back(indexCount + 1);
        indices.push_back(indexCount + 2);

        indices.push_back(indexCount + 2);
        indices.push_back(indexCount + 3);
        indices.push_back(indexCount + 0);

        // 다음 글자를 위해 인덱스 오프셋 증가
        indexCount += 4;

        // 5. 커서 이동
        // advance는 다음 글자 원점까지의 거리 (1/64 픽셀 단위로 저장된 것을 Font에서 이미 변환해서 줌)
        cursorX += glyph.advance;
    }

    // 6. 생성된 데이터를 UIMesh에 전달하여 GPU 버퍼 갱신
    if (m_uiMesh)
    {
        m_uiMesh->SetVertices(vertices);
        m_uiMesh->SetIndices(indices);
    }
}

void UIText::Render(Program* program)
{
    // 렌더링 할 게 없으면 리턴
    if (!m_uiMesh || m_text.empty()) return;
    m_uiMesh->Draw();
}

void UIText::SetText(const std::wstring& text)
{
    if (m_text == text) return;
    m_text = text;
    RebuildMesh();
}
