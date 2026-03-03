#pragma once

template<typename... Args>
void Script::InvokeLua(const sol::function& func, Args&&... args)
{
    if (func.valid()) 
    {
        auto result = func(std::forward<Args>(args)...);
        if (!result.valid()) 
        {
            sol::error err = result;
            LOG_ERROR("Lua Runtime Error: {}", err.what());
        }
    }
}