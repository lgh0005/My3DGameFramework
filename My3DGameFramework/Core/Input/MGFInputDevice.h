#pragma once
#include "Hashes/StringHash.h"

struct GLFWwindow;

namespace MGF3D
{
    MGF_CLASS_PTR(MGFInputDevice)
    class MGFInputDevice
    {
    public:
        MGFInputDevice(const String& name);
        virtual ~MGFInputDevice();

    public:
        virtual bool Init() = 0;
        virtual void Update(GLFWwindow* window) = 0;
        virtual void Shutdown() = 0;

        // 특정 액션의 상태를 반환하는 공통 인터페이스
        virtual bool GetButton(StringView actionName) const = 0;
        virtual bool GetButtonDown(StringView actionName) const = 0;
        virtual bool GetButtonUp(StringView actionName) const = 0;

    protected:
        StringHash m_deviceName;
    };
}