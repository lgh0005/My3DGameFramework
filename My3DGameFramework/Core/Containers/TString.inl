#include "TString.h"
#pragma once

namespace MGF3D
{
    template<typename Alloc>
    inline TString<Alloc>::TString(const str& s, const Alloc& alloc) : Base(s.c_str(), alloc) { }

    template<typename Alloc>
    inline usize TString<Alloc>::MemoryUsage() const
    {
        // INFO : SSO(Small String Optimization)에 대한 메모리 할당은
        // 고려 대상에서 제외. 순수히 메모리 풀에 할당된 메모리 크기만 반환.

        // 1. 객체 자체의 기본 크기
        usize usage = sizeof(*this);

        // 2. 현재 문자열 데이터가 저장된 시작 주소
        const char8* dataPtr = this->data();

        // 3. 객체 자신의 메모리 주소 범위 계산
        const char8* thisStart = reinterpret_cast<const char8*>(this);
        const char8* thisEnd = thisStart + sizeof(*this);

        // 4. 데이터 포인터가 내 몸통 밖에 있다면 힙 할당 상태
        if (dataPtr < thisStart || dataPtr >= thisEnd)
            usage += (this->capacity() * sizeof(char8));

        return usage;
    }

    template<typename Alloc>
    inline usize TString<Alloc>::Capacity() const
    {
        return this->capacity();
    }

    template<typename Alloc>
    inline void TString<Alloc>::Reserve(usize n)
    {
        this->reserve(n);
    }

    template<typename Alloc>
    inline bool TString<Alloc>::Contains(cstr s) const
    {
        return this->find(s) != Base::npos;
    }

    template<typename Alloc>
    inline bool TString<Alloc>::Contains(char8 c) const
    {
        return this->find(c) != Base::npos;
    }

    template<typename Alloc>
    inline usize TString<Alloc>::Length() const
    {
        return this->length();
    }
}