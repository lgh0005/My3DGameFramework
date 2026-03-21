#pragma once
#include "Entities/Entity/Object.h"

namespace MGF3D
{
	class Mechanic : public Object
	{
		// TODO :
		// 가끔 보면 컴포넌트가 내부적으로 추가적으로 필요로하는 '부품'(클래스)이 존재한다.
		// 예) Animator는 AnimController를 필요로 한다.
		// 또는, 그 부품이 다른 부품을 요구하는 경우도 있다.
		// 예) AnimController는 AnimState를 필요로 한다.

		// 아마 이후에 NavMesh와 같은 것을 구현하거나 여러 다른 컴포넌트를 구현할 때도
		// 이러한 '부품'이 필요하게 될 가능성이 매우 높다.
		// 그에 대한 Object 족보의 부품 클래스의 추상 클래스를 구현하고자
		// 작성하게 되었다.
	};
}