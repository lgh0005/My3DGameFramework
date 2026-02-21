#include "TString.h"
#pragma once

namespace MGF3D
{
    template<typename Alloc>
    inline TString<Alloc>::TString(const str& s, const Alloc& alloc) : Base(s.c_str(), alloc) { }

    template<typename Alloc>
    template<typename OtherAlloc>
    inline TString<Alloc>::TString(const TString<OtherAlloc>& other)
        : Base(other.data(), other.size(), Alloc()) { }

    template<typename Alloc>
    template<typename OtherAlloc>
    inline TString<Alloc>::TString(TString<OtherAlloc>&& other) noexcept
        : Base(other.data(), other.size(), Alloc())
    {
        other.clear();
    }

    template<typename Alloc>
    template<typename OtherAlloc>
    inline TString<Alloc>& TString<Alloc>::operator=(const TString<OtherAlloc>& other)
    {
        this->assign(other.data(), other.size());
        return *this;
    }

    template<typename Alloc>
    template<typename OtherAlloc>
    inline TString<Alloc>& TString<Alloc>::operator=(TString<OtherAlloc>&& other) noexcept
    {
        this->assign(other.data(), other.size());
        other.clear();
        return *this;
    }

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
    inline StringHash TString<Alloc>::Hash() const
    {
        return StringHash(strview(this->data(), this->size()));
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

    // TString + TString
    template<typename Alloc>
    inline TString<Alloc> operator+(const TString<Alloc>& lhs, const TString<Alloc>& rhs)
    {
        TString<Alloc> result(lhs.get_allocator());
        result.reserve(lhs.length() + rhs.length());
        result.append(lhs);
        result.append(rhs);
        return result;
    }

    // TString + cstr
    template<typename Alloc>
    inline TString<Alloc> operator+(const TString<Alloc>& lhs, cstr rhs)
    {
        usize rhsLen = (rhs == nullptr) ? 0 : std::char_traits<char8>::length(rhs);
        TString<Alloc> result(lhs.get_allocator());
        result.reserve(lhs.length() + rhsLen);
        result.append(lhs);
        if (rhsLen > 0) result.append(rhs);
        return result;
    }

    // cstr + TString
    template<typename Alloc>
    inline TString<Alloc> operator+(cstr lhs, const TString<Alloc>& rhs)
    {
        usize lhsLen = (lhs == nullptr) ? 0 : std::char_traits<char8>::length(lhs);
        TString<Alloc> result(rhs.get_allocator());
        result.reserve(lhsLen + rhs.length());
        if (lhsLen > 0) result.append(lhs);
        result.append(rhs);
        return result;
    }

    // TString + char8
    template<typename Alloc>
    inline TString<Alloc> operator+(const TString<Alloc>& lhs, char8 rhs)
    {
        TString<Alloc> result(lhs.get_allocator());
        result.reserve(lhs.length() + 1);
        result.append(lhs);
        result.push_back(rhs);
        return result;
    }

    // char8 + TString
    template<typename Alloc>
    inline TString<Alloc> operator+(char8 lhs, const TString<Alloc>& rhs)
    {
        TString<Alloc> result(rhs.get_allocator());
        result.reserve(rhs.length() + 1);
        result.push_back(lhs);
        result.append(rhs);
        return result;
    }

    template<typename Alloc>
    inline TString<Alloc> operator+(const TString<Alloc>& lhs, strview rhs)
    {
        TString<Alloc> result(lhs.get_allocator());
        result.reserve(lhs.length() + rhs.size());
        result.append(lhs);
        result.append(rhs.data(), rhs.size());
        return result;
    }

    template<typename Alloc>
    inline TString<Alloc> operator+(strview lhs, const TString<Alloc>& rhs)
    {
        TString<Alloc> result(rhs.get_allocator());
        result.reserve(lhs.size() + rhs.length());
        result.append(lhs.data(), lhs.size());
        result.append(rhs);
        return result;
    }
}