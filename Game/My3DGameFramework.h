#pragma once
#include "Core/Application.h"

CLASS_PTR(My3DGameFramework)
class My3DGameFramework : public Application
{
	using Super = Application;

public:
	virtual ~My3DGameFramework();
	static My3DGameFrameworkUPtr Create
	(
		int32 width, int32 height, 
		const std::string& title
	);
	virtual void OnInit() override;

private:
	bool Init(int32 width, int32 height, const std::string& title);
	My3DGameFramework();
};