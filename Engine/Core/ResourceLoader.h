#pragma once

CLASS_PTR(ResourceLoader)
class ResourceLoader
{
	DECLARE_STATIC_CLASS(ResourceLoader)

public:
	static void LoadFromManifest(const std::string& manifestPath);

private:

};