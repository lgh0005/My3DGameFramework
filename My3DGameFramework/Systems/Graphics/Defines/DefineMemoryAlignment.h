#pragma once

/*======================================================//
//     GPU memory alignment static assertion macros     //
//======================================================*/
#define MGF_ASSERT_GPU_ALIGNMENT(Type)                               \
    MGF_STATIC_ASSERT(sizeof(Type) % OPENGL_MEMORY_ALIGNMENT == 0,   \
    #Type " size must be a multiple of OPENGL_MEMORY_ALIGNMENT!")

#define MGF_ASSERT_GPU_OFFSET(Type, Member, Alignment)               \
    MGF_STATIC_ASSERT(offsetof(Type, Member) % (Alignment) == 0,     \
    #Type "::" #Member " alignment mismatch!")

#define MGF_ASSERT_GPU_EXACT_SIZE(Type, ExpectedSize)                \
    MGF_STATIC_ASSERT(sizeof(Type) == (ExpectedSize),                \
    #Type " size mismatch! Expected exact size.")

#define MGF_ASSERT_GPU_EXACT_OFFSET(Type, Member, ExpectedOffset)    \
    MGF_STATIC_ASSERT(offsetof(Type, Member) == (ExpectedOffset),    \
    #Type "::" #Member " offset mismatch! Expected exact offset.")