#pragma once

/*===========================================//
//     Default hash functor registration     //
//===========================================*/
// Define a hash functor for a custom type to be used in unordered containers.
#define MGF_REGISTER_HASH(Type, ValueType)                                           \
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
