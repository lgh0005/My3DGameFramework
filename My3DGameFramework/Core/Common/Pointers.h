#pragma once

namespace MGF3D
{
	template <typename T>
	using UniquePtr = std::unique_ptr<T>;

	template <typename T>
	using SharedPtr = std::shared_ptr<T>;

	template <typename T>
	using WeakPtr = std::weak_ptr<T>;
}

/*================================//
//   Class Pointer Declarations   //
//================================*/
#define MGF_CLASS_PTR(className)                                                \
class className;                                                                \
using className ## UPtr = MGF3D::UniquePtr<className>;                          \
using className ## Ptr  = MGF3D::SharedPtr<className>;                          \
using className ## WPtr = MGF3D::WeakPtr<className>;