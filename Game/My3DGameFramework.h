#pragma once
#include "Core/Application.h"

class My3DGameFramework : public Application
{
	DECLARE_APPLICATION_CREATE(My3DGameFramework)

public:
	virtual void OnInit() override;

private:
	My3DGameFramework();
};