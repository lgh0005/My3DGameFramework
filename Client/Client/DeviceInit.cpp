#include "pch.h"
#include "DeviceInit.h"

DeviceInit::DeviceInit(const string& name) : Super(name)
{

}


void DeviceInit::Init()
{
	Super::Init();
}

void DeviceInit::Update()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// TODO

	SDL_GL_SwapWindow(_window);
}