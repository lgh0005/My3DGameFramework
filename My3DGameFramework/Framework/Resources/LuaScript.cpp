#include "EnginePch.h"
#include "LuaScript.h"

DECLARE_DEFAULTS_IMPL(LuaScript)

LuaScriptPtr LuaScript::Load(const ScriptDesc& desc)
{
	// 1. 가상 경로를 물리 경로로 변환
	std::string physicalPath = RESOURCE.ResolvePath(desc.path);

    // 2. 파일 열기
    std::ifstream file(physicalPath);
    if (!file.is_open())
    {
        LOG_ERROR("LuaScript: Failed to open file at {}", physicalPath);
        return nullptr;
    }

    // 3. 파일 내용을 문자열 스트림으로 읽기
    std::stringstream buffer;
    buffer << file.rdbuf();

    // 4. 리소스 객체 생성 및 데이터 채우기
    LuaScriptPtr script(new LuaScript());
    script->m_desc = desc;
    script->m_code = buffer.str();

    // 5. 검증: 코드가 비어있는지 확인
    if (script->m_code.empty())
        LOG_WARN("LuaScript: Loaded script is empty -> {}", desc.name);

    LOG_INFO
    (
        "LuaScript: Successfully loaded [{}] (Size: {} bytes)", 
        desc.name, 
        script->m_code.size()
    );
    return script;
}