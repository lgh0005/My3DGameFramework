#pragma once
#include "Containers/IContainer.h"
#include "Hashing/StringHash.h"

namespace MGF3D
{
	template<typename Alloc>
	class TString : public IContainer
	{
	private:
		std::basic_string<char8, std::char_traits<char8>, Alloc> m_string;

	public:
		TString() = default;
		TString(cstr s);
		TString(strview sv, const Alloc& alloc = Alloc());
		virtual ~TString() = default;

	public:
		// 1. 다른 할당자를 사용하는 TString으로부터 복사 생성
		template<typename OtherAlloc>
		TString(const TString<OtherAlloc>& other);

		// 2. 다른 할당자를 사용하는 TString으로부터 이동 생성 (가상 이동)
		template<typename OtherAlloc>
		TString(TString<OtherAlloc>&& other) noexcept;

		// 3. 다른 할당자로부터의 대입 연산자
		template<typename OtherAlloc>
		TString& operator=(const TString<OtherAlloc>& other);

		template<typename OtherAlloc>
		TString& operator=(TString<OtherAlloc>&& other) noexcept;

		TString& operator=(cstr s);
		TString& operator=(strview sv);

		// 3. += 연산자들 (기존 using Base::operator+= 대체)
		TString& operator+=(const TString& rhs);
		TString& operator+=(cstr rhs);
		TString& operator+=(char8 rhs);
		TString& operator+=(strview rhs);

	public:
		usize MemoryUsage() const override { return sizeof(*this) + (m_string.capacity() * sizeof(char8)); }
		usize Count()		const override { return m_string.length(); }
		bool  Empty()		const override { return m_string.empty(); }
		void  Clear()			  override { m_string.clear(); }
		void  Release()			  override;

	public:
		cstr CStr() const { return m_string.c_str(); }
		StringHash Hash() const;

		usize Length()		   const { return m_string.length(); }
		usize Capacity()	   const { return m_string.capacity(); }
		void Reserve(usize n)		 { m_string.reserve(n); }
		bool Contains(cstr s)  const;
		bool Contains(char8 c) const;

	public:
		using iterator = typename std::basic_string<char8, std::char_traits<char8>, Alloc>::iterator;
		using const_iterator = typename std::basic_string<char8, std::char_traits<char8>, Alloc>::const_iterator;

		iterator begin() { return m_string.begin(); }
		iterator end() { return m_string.end(); }
		const_iterator begin() const { return m_string.begin(); }
		const_iterator end()   const { return m_string.end(); }

		char8& operator[](usize index) { return m_string[index]; }
		const char8& operator[](usize index) const { return m_string[index]; }

		using npos_type = typename std::basic_string<char8, std::char_traits<char8>, Alloc>::size_type;
		inline static const npos_type npos = -1;
	};

	/*=======================================//
	//   operator overloadings for TString   //
	//=======================================*/
	template<typename Alloc> TString<Alloc> operator+(const TString<Alloc>& lhs, const TString<Alloc>& rhs);
	template<typename Alloc> TString<Alloc> operator+(const TString<Alloc>& lhs, cstr rhs);
	template<typename Alloc> TString<Alloc> operator+(cstr lhs, const TString<Alloc>& rhs);
	template<typename Alloc> TString<Alloc> operator+(const TString<Alloc>& lhs, char8 rhs);
	template<typename Alloc> TString<Alloc> operator+(char8 lhs, const TString<Alloc>& rhs);
	template<typename Alloc> TString<Alloc> operator+(const TString<Alloc>& lhs, strview rhs);
	template<typename Alloc> TString<Alloc> operator+(strview lhs, const TString<Alloc>& rhs);
	
	template<typename Alloc> bool operator==(const TString<Alloc>& lhs, const TString<Alloc>& rhs);
	template<typename Alloc> bool operator==(const TString<Alloc>& lhs, cstr rhs);
}

#include "Containers/TString.inl"

