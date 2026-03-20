#pragma once

namespace MGF3D
{
	/// <summary>
	/// ResourceManager는 사용자에게 공개되지 않은 더 작은 요소들 까지 다루는
	/// 대상이라면 AssetManager는 작은 요소들이 모여서 만들어질, 실제 사용자
	/// 상에서 쓰일 그런 실질적 리소스 요소들을 관리하는 매니저로 만들 예정
	/// 집합 관계로 생각하면 AssetManager는 ResourceManager에 포함되는 관계.
	/// </summary>
	class AssetManager
	{
		MGF_DECLARE_SINGLE(AssetManager)

	private:
		AssetManager();
		~AssetManager();

	public:


	private:

	};
}