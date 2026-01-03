#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(UICanvas)
#pragma endregion

struct UIAction
{
	// TODO : UI 상호작용 시 수행해야 할 일감
};

// TODO : 렌더링 처리 로직은 순수히 UIRenderPass에게 맡기고,
// 여기에서는 스크립트로 작성한 UI 콜백 함수 이벤트를 받아서 처리하는 
// 구조를 택할 필요가 있음.
class UIManager
{
	DECLARE_SINGLE(UIManager)
	DECLARE_NONINSTANTIABLE(UIManager)

public:
	void Init();
	void Clear();

private:
};