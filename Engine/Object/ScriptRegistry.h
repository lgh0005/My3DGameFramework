#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(Script)
#pragma endregion

class ScriptRegistry
{
	DECLARE_STATIC_CLASS(ScriptRegistry)

public:
	template<typename T> 
	static ScriptID GetID()
	{
		static_assert(std::is_base_of<Script, T>::value);
		static const ScriptID id = s_nextID.fetch_add(1, std::memory_order_relaxed);

		if (id == UINT64_MAX)
		{
			LOG_FATAL("Critical Error: ScriptID Overflow! You have defined too many Script classes.");
			std::abort();
		}

		return id;
	}

private:
	static std::atomic<ScriptID> s_nextID;
};