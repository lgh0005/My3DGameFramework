#pragma once

CLASS_PTR(GameObjectRegistry)
class GameObjectRegistry
{
public:
	// TODO : GameObject는 자신이 소유하는 Component
	// unique_ptr를 이곳을 창고로 쓴다

	// 마치 현재의 구조에서 Scene이 SceneRegistry를 창고 삼고
	// GameObject의 생명주기를 GameObjectMaanger를 통해서 관리하는
	// 방식과 유사하다.

private:

};