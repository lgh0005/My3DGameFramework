#include "CorePch.h"
#include "IWaitable.h"
#include "Identity/MGFType.h"

namespace MGF3D
{
	IWaitable::IWaitable(Ptr<MGFType> type, const MGFName& name) : m_type(type), m_name(name) { }
	IWaitable::~IWaitable() = default;

	bool IWaitable::IsReady() const
	{
		return GetState() == WaitableObjectState::Ready;
	}

	const MGFName& IWaitable::GetName() const
	{
		return m_name;
	}

	WaitableObjectState IWaitable::GetState() const
	{
		return m_state.load(std::memory_order_acquire);
	}

	void IWaitable::SetState(WaitableObjectState state)
	{
		m_state.store(state, std::memory_order_release);
	}
}