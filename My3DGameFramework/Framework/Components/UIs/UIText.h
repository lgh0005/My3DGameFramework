#pragma once
#include "UIs/UIRenderer.h"

namespace MGF3D
{
	MGF_CLASS_PTR(Mesh)
	MGF_CLASS_PTR(Material)
	MGF_CLASS_PTR(Program)
	MGF_CLASS_PTR(Font)

	MGF_CLASS_PTR(UIText)
	class UIText : public UIRenderer
	{
		using Super = UIRenderer;

	public:
		virtual ~UIText();
		static UITextUPtr Create
		(
			FontPtr font, MaterialPtr material,
			const std::wstring& text = L"", int32 sortingOrder = 0
		);
		static const ComponentType s_ComponentType = ComponentType::UIText;
		virtual ComponentType GetComponentType() const override { return ComponentType::UIText; }
		virtual void Render(Program* program) override;

		// 텍스트 설정 (변경 시 메쉬 재생성)
		void SetText(const std::wstring& text);
		const std::wstring& GetText() const { return m_text; }

	private:
		UIText();
		bool Init
		(
			FontPtr font, MaterialPtr material,
			const std::wstring& text, int32 sortingOrder
		);
		void RebuildMesh();

		std::wstring m_text;
		FontPtr      m_font;
	};
}