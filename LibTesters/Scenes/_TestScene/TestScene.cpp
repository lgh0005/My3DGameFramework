#include "GamePch.h"
#include "TestScene.h"
#include "Managers/TypeManager.h"

namespace MGF3D
{
	TestScene::TestScene() = default;
	TestScene::~TestScene() = default;

	/*================================//
	//    MGFInputDevice Custom Type  //
	//================================*/
	int16 TestScene::s_typeIndex = -1;
	const MGFType* TestScene::GetType() const
	{
		MGFTypeTree* tree = MGF_TYPE.GetTree("Scene");
		if (tree != nullptr) return tree->GetType(s_typeIndex);
		return nullptr;
	}

	bool TestScene::OnLoadSceneResources()
	{
		MGF_LOG_FATAL("TestScene : OnLoadSceneResources called.");
		return true;
	}

	bool TestScene::OnPlaceActors()
	{
		MGF_LOG_FATAL("TestScene : OnPlaceActors called.");

		// 1. Entities API를 사용하여 게임 오브젝트 생성
		ObjectIDHash player1ID = Entities::Create("Player");
		ObjectIDHash player2ID = Entities::Create("Player");

		// 2. ID가 정상적으로 발급되었는지 확인
		if (player1ID.IsValid())
		{
			MGF_LOG_ERROR("Player 1 created! Index: {0}, Generation: {1}",
				player1ID.GetIndex(), player1ID.GetGeneration());
		}

		if (player2ID.IsValid())
		{
			// NameManager가 중복 이름을 "Player_1" 등으로 처리하는지 확인하기 좋은 테스트입니다.
			MGF_LOG_ERROR("Player 2 created! Index: {0}, Generation: {1}",
				player2ID.GetIndex(), player2ID.GetGeneration());
		}

		return true;
	}
}