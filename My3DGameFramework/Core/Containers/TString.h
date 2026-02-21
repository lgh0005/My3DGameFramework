#pragma once
#include "Containers/IContainer.h"

namespace MGF3D
{
	template<typename Alloc>
	class TString : public std::basic_string<char8, std::char_traits<char8>, Alloc>, public IContainer
	{
	public:
		using Base = std::basic_string<char8, std::char_traits<char8>, Alloc>;
		using Base::basic_string;

		TString() = default;
		TString(const str& s, const Alloc& alloc = Alloc());
		virtual ~TString() = default;

	public:
		usize MemoryUsage() const override;
		usize Count()   const override { return this->size(); }
		bool  Empty()   const override { return this->empty(); }
		void  Clear()         override { this->clear(); }

	public:
		cstr CStr() const { return this->c_str(); }

		usize Capacity() const;
		void Reserve(usize n);
		bool Contains(cstr s) const;
		bool Contains(char8 c) const;

		usize Length() const;
	};
}

#include "Containers/TString.inl"