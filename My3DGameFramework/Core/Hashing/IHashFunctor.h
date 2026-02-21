#pragma once

namespace MGF3D
{
    // CRTP 기반의 해시 객체 인터페이스
	template<typename HashFucntor>
	struct IHashFunctor
	{
        uint32 value { 0 };

        constexpr IHashFunctor() : value(0) {}

        // CRTP면 이 생성자가 호출될 이유는 없어보이긴 함.
        explicit constexpr IHashFunctor(uint32 h) : value(h) {}

        bool operator==(const HashFucntor& other) const { return value == other.value; }
        bool operator!=(const HashFucntor& other) const { return value != other.value; }
        bool operator<(const HashFucntor& other) const { return value < other.value; }
        constexpr operator uint32() const { return value; }

        bool IsValid() const { return value != 0; }
	};
}

/*==================================//
//   Default Hash object register   //
//==================================*/
#pragma region HASH_OBJECT_REGISTER

#define MGF_REGISTER_HASH(Type)                                                 \
namespace std                                                                   \
{                                                                               \
    template<>                                                                  \
    struct hash<Type>                                                           \
    {                                                                           \
        MGF3D::usize operator()(const Type& h) const noexcept                   \
        {                                                                       \
            return static_cast<MGF3D::usize>(h.value);                          \
        }                                                                       \
    };                                                                          \
}

#pragma endregion