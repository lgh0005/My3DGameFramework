#include "EnginePch.h"
#include "RigidbodyConstraint.h"

/*======================================//
//   RigidbodyConstraint constructors   //
//======================================*/
RigidbodyConstraint::RigidbodyConstraint() : m_bitMask(None) {}
RigidbodyConstraint::RigidbodyConstraint(uint8 mask) : m_bitMask(mask) {}
RigidbodyConstraint::RigidbodyConstraint(Type type) : m_bitMask(static_cast<uint8>(type)) {}

/*========================//
//   override operators   //
//========================*/
RigidbodyConstraint RigidbodyConstraint::operator|(Type flag) const
{
	return RigidbodyConstraint(m_bitMask | flag);
}

RigidbodyConstraint RigidbodyConstraint::operator|(const RigidbodyConstraint& other) const
{
	return RigidbodyConstraint(m_bitMask | other.m_bitMask);
}

void RigidbodyConstraint::operator|=(Type flag)
{
	m_bitMask |= flag;
}

bool RigidbodyConstraint::operator==(const RigidbodyConstraint& other) const
{
	return m_bitMask == other.m_bitMask;
}

bool RigidbodyConstraint::operator!=(const RigidbodyConstraint& other) const
{
	return m_bitMask != other.m_bitMask;
}