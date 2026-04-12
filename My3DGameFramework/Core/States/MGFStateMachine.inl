#include "MGFStateMachine.h"
#pragma once

namespace MGF3D
{
    template<typename TEnum>
    inline MGFStateMachine<TEnum>::MGFStateMachine() : m_currentState(static_cast<TEnum>(0)) {}
    
    template<typename TEnum>
    inline MGFStateMachine<TEnum>::MGFStateMachine(TEnum initialState) : m_currentState(initialState) {}

    template<typename TEnum>
    inline MGFStateMachine<TEnum>::~MGFStateMachine() = default;

    template<typename TEnum>
    inline TEnum MGFStateMachine<TEnum>::GetState() const
    {
        return m_currentState;
    }

    template<typename TEnum>
    inline bool MGFStateMachine<TEnum>::Is(TEnum state) const
    {
        return m_currentState == state;
    }

    template<typename TEnum>
    inline void MGFStateMachine<TEnum>::SetState(TEnum newState)
    {
        m_currentState = newState;
    }

    template<typename TEnum>
    inline bool MGFStateMachine<TEnum>::Transition(TEnum expectedState, TEnum newState)
    {
        if (m_currentState == expectedState)
        {
            m_currentState = newState;
            return true;
        }
        return false;
    }
}