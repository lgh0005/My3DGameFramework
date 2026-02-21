#pragma once

namespace MGF3D
{
    // CRTP 기반의 해시 객체 인터페이스
	template<typename HashFucntor, typename T = uint32>
	struct IHashFunctor
	{
        T value { 0 };

        constexpr IHashFunctor() : value(0) {}

        // 자식 클래스에서 해시 계산 결과를 부모에게 전달할 때 사용
        explicit constexpr IHashFunctor(T h) : value(h) {}

        bool operator==(const HashFucntor& other) const { return value == other.value; }
        bool operator!=(const HashFucntor& other) const { return value != other.value; }
        bool operator<(const HashFucntor& other) const { return value < other.value; }
        constexpr operator T() const { return value; }

        bool IsValid() const { return value != 0; }
	};
}

/*==================================//
//   Default Hash object register   //
//==================================*/
#define MGF_REGISTER_HASH(Type, ValueType)                                           \
                                                                                     \
namespace std                                                                        \
{                                                                                    \
    template<>                                                                       \
    struct hash<MGF3D::Type>                                                         \
    {                                                                                \
        MGF3D::usize operator()(const MGF3D::Type& h) const noexcept                 \
        {                                                                            \
            return std::hash<MGF3D::ValueType>{}(static_cast<MGF3D::ValueType>(h));  \
        }                                                                            \
    };                                                                               \
}

#pragma endregion