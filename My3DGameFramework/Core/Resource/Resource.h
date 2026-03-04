#pragma once
#include "Misc/Enums.h"
#include "Resources/ResourceDesc.h"

namespace MGF3D
{
	MGF_CLASS_PTR(Resource)
	class Resource
	{
	public:
		virtual ~Resource();
		virtual ResourceType GetResourceType() const = 0;
		virtual ResourceDesc& GetDesc() = 0;
		virtual const ResourceDesc& GetDesc() const = 0;
		bool MatchesType(ResourceType type) const;
		const std::string& GetName() const;

		// TODO :
		// 이후 비동기 리소스 로드를 위해
		// "순수 CPU 단에서 로드될 수 있고, 여타 다른 API의 스레드 불안정성을 배제한
		// 로직들"을 따로 구현하여 워커 스레드의 일감으로 떠넘길 수 있는 추상 메서드를
		// 추가할 필요가 있다.

	protected:
		Resource();
	};
}