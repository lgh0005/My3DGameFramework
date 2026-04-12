#pragma once

namespace MGF3D
{
    template <typename TEnum>
    class MGFStateMachine
    {
    public:
        MGFStateMachine();
        MGFStateMachine(TEnum initialState);
        ~MGFStateMachine();

    public:
        inline TEnum GetState() const;
        inline bool Is(TEnum state) const;
        inline void SetState(TEnum newState);
        inline bool Transition(TEnum expectedState, TEnum newState);

    private:
        TEnum m_currentState;
    };
}

#include "States/MGFStateMachine.inl"