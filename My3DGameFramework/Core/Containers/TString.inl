#pragma once

namespace MGF3D
{
    template<typename Alloc>
    inline TString<Alloc>::TString(cstr s) : m_string(s) {}

    template<typename Alloc>
    inline TString<Alloc>::TString(strview sv, const Alloc& alloc) : m_string(sv, alloc) {}

    template<typename Alloc>
    template<typename OtherAlloc>
    inline TString<Alloc>::TString(const TString<OtherAlloc>& other)
        : m_string(other.begin(), other.end()) { }

    template<typename Alloc>
    template<typename OtherAlloc>
    inline TString<Alloc>::TString(TString<OtherAlloc>&& other) noexcept
        : m_string(std::make_move_iterator(other.begin()), std::make_move_iterator(other.end())) { }

    template<typename Alloc>
    template<typename OtherAlloc>
    inline TString<Alloc>& TString<Alloc>::operator=(const TString<OtherAlloc>& other) 
    {
        m_string.assign(other.begin(), other.end());
        return *this;
    }

    template<typename Alloc>
    template<typename OtherAlloc>
    inline TString<Alloc>& TString<Alloc>::operator=(TString<OtherAlloc>&& other) noexcept 
    {
        m_string.assign(std::make_move_iterator(other.begin()), std::make_move_iterator(other.end()));
        return *this;
    }

    template<typename Alloc> inline TString<Alloc>& TString<Alloc>::operator=(cstr s) { m_string = s; return *this; }
    template<typename Alloc> inline TString<Alloc>& TString<Alloc>::operator=(strview sv) { m_string = sv; return *this; }

    template<typename Alloc> inline TString<Alloc>& TString<Alloc>::operator+=(const TString& rhs) { m_string += rhs.CStr(); return *this; }
    template<typename Alloc> inline TString<Alloc>& TString<Alloc>::operator+=(cstr rhs) { m_string += rhs; return *this; }
    template<typename Alloc> inline TString<Alloc>& TString<Alloc>::operator+=(char8 rhs) { m_string += rhs; return *this; }
    template<typename Alloc> inline TString<Alloc>& TString<Alloc>::operator+=(strview rhs) { m_string += rhs; return *this; }

    template<typename Alloc>
    inline void TString<Alloc>::Release() 
    {
        m_string.clear();
        m_string.shrink_to_fit();
    }

    template<typename Alloc>
    inline void TString<Alloc>::Resize(usize n)
    {
        m_string.resize(n);
    }

    template<typename Alloc>
    inline void TString<Alloc>::Resize(usize n, byte c)
    {
        m_string.resize(n, c);
    }

    template<typename Alloc>
    inline StringHash TString<Alloc>::Hash() const 
    {
        return StringHash(m_string.c_str());
    }

    template<typename Alloc>
    inline bool TString<Alloc>::Contains(cstr s) const
    {
        return m_string.find(s) != npos;
    }

    template<typename Alloc>
    inline bool TString<Alloc>::Contains(char8 c) const 
    {
        return m_string.find(c) != npos;
    }

    /*=======================================//
    //   operator overloadings for TString   //
    //=======================================*/
    template<typename Alloc>
    inline TString<Alloc> operator+(const TString<Alloc>& lhs, const TString<Alloc>& rhs) 
    {
        TString<Alloc> res = lhs; res += rhs; return res;
    }

    template<typename Alloc>
    inline TString<Alloc> operator+(const TString<Alloc>& lhs, cstr rhs) 
    {
        TString<Alloc> res = lhs; res += rhs; return res;
    }

    template<typename Alloc>
    inline TString<Alloc> operator+(cstr lhs, const TString<Alloc>& rhs) 
    {
        TString<Alloc> res(lhs); res += rhs; return res;
    }

    template<typename Alloc>
    inline TString<Alloc> operator+(const TString<Alloc>& lhs, char8 rhs) 
    {
        TString<Alloc> res = lhs; res += rhs; return res;
    }

    template<typename Alloc>
    inline TString<Alloc> operator+(char8 lhs, const TString<Alloc>& rhs) 
    {
        TString<Alloc> res; res += lhs; res += rhs; return res;
    }

    template<typename Alloc>
    inline TString<Alloc> operator+(const TString<Alloc>& lhs, strview rhs) 
    {
        TString<Alloc> res = lhs; res += rhs; return res;
    }

    template<typename Alloc>
    inline TString<Alloc> operator+(strview lhs, const TString<Alloc>& rhs) 
    {
        TString<Alloc> res(lhs); res += rhs; return res;
    }

    template<typename Alloc> inline bool operator==(const TString<Alloc>& lhs, const TString<Alloc>& rhs) 
    {
        return std::strcmp(lhs.CStr(), rhs.CStr()) == 0; 
    }

    template<typename Alloc> inline bool operator==(const TString<Alloc>& lhs, cstr rhs) 
    { 
        return std::strcmp(lhs.CStr(), rhs) == 0; 
    }
}