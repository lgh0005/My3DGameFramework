#pragma once
#include "Engine\Scene.h"

class DeviceInit : public Scene
{
	using Super = Scene;

public:
	DeviceInit(const string& name);
	virtual ~DeviceInit() override = default;

public:
	virtual void Init() override;
	virtual void Update() override;
};

