#pragma once
#include "ISingleton.h"

class TimeManager : public ISingleton<TimeManager>
{
	DECLARE_SINGLE(TimeManager);

public:
	virtual ~TimeManager() override;

public:
	virtual void Init() override;
	virtual void Update() override;
	float deltaTime = 0.0f;
	float FPS = 0.0f;

private:
	uint64 _lastTime = SDL_GetPerformanceCounter();
	uint64 _currentTime = 0;
};

