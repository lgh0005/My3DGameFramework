#pragma once
#include "Containers/IContainer.h"
#include "Hashing/StringHash.h"

namespace MGF3D
{
	template<typename Alloc>
	class TString : public std::basic_string<char8, std::char_traits<char8>, Alloc>, public IContainer
	{
	public:
		using Base = std::basic_string<char8, std::char_traits<char8>, Alloc>;
		using Base::basic_string;
		using Base::operator=;
		using Base::operator+=;
		using Base::operator[];

		TString() = default;
		TString(const str& s, const Alloc& alloc = Alloc());
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

	public:
		usize MemoryUsage() const override;
		usize Count()   const override { return this->size(); }
		bool  Empty()   const override { return this->empty(); }
		void  Clear()         override { this->clear(); }

	public:
		cstr CStr() const { return this->c_str(); }
		StringHash Hash() const;

		usize Length() const;
		usize Capacity() const;
		void Reserve(usize n);
		bool Contains(cstr s) const;
		bool Contains(char8 c) const;

	public:
		using iterator = typename Base::iterator;
		using const_iterator = typename Base::const_iterator;
		using reverse_iterator = typename Base::reverse_iterator;
		using const_reverse_iterator = typename Base::const_reverse_iterator;

		iterator       begin()        noexcept { return Base::begin(); }
		iterator       end()          noexcept { return Base::end(); }
		const_iterator begin()  const noexcept { return Base::begin(); }
		const_iterator end()    const noexcept { return Base::end(); }
		const_iterator cbegin() const noexcept { return Base::cbegin(); }
		const_iterator cend()   const noexcept { return Base::cend(); }

		reverse_iterator       rbegin()        noexcept { return Base::rbegin(); }
		reverse_iterator       rend()          noexcept { return Base::rend(); }
		const_reverse_iterator crbegin() const noexcept { return Base::crbegin(); }
		const_reverse_iterator crend()   const noexcept { return Base::crend(); }
	};

	/*=======================================//
	//   operator overloadings for TString   //
	//=======================================*/
	template<typename Alloc>
	TString<Alloc> operator+(const TString<Alloc>& lhs, const TString<Alloc>& rhs);

	template<typename Alloc>
	TString<Alloc> operator+(const TString<Alloc>& lhs, cstr rhs);

	template<typename Alloc>
	TString<Alloc> operator+(cstr lhs, const TString<Alloc>& rhs);

	template<typename Alloc>
	TString<Alloc> operator+(const TString<Alloc>& lhs, char8 rhs);

	template<typename Alloc>
	TString<Alloc> operator+(char8 lhs, const TString<Alloc>& rhs);

	template<typename Alloc>
	TString<Alloc> operator+(const TString<Alloc>& lhs, strview rhs);

	template<typename Alloc>
	TString<Alloc> operator+(strview lhs, const TString<Alloc>& rhs);
}

#include "Containers/TString.inl"

