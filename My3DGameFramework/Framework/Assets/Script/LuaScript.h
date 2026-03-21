#pragma once
#include "Resources/Resource.h"
#include "Resources/ResourceDesc.h"

CLASS_PTR(LuaScript)
class LuaScript : public Resource
{
	DEFINE_RESOURCE_TYPE(ResourceType::LuaScript, ScriptDesc)

public:
	virtual ~LuaScript() override;
	static LuaScriptPtr Load(const ScriptDesc& desc);
	const std::string& GetCode() const { return m_code; }

public:
	virtual ResourceDesc& GetDesc() { return m_desc; }
	virtual const ResourceDesc& GetDesc() const { return m_desc; }

private:
	LuaScript();

	ScriptDesc m_desc;
	std::string m_code;
};