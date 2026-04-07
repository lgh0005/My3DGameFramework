#include "CorePch.h"
#include "MGFInputDevice.h"
#include <GLFW/glfw3.h>

namespace MGF3D
{
	MGFInputDevice::MGFInputDevice(const String& name) : m_deviceName{ StringHash(name) } { }
	MGFInputDevice::~MGFInputDevice() = default;
}