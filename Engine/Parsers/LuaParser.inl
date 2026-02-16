#pragma once

template<typename T>
inline T LuaParser::Get(const std::string& key, const T& defaultValue)
{
	// 1. 파서 상태가 유효하지 않으면 즉시 기본값 반환
	if (!m_isValid) return defaultValue;

	// 2. sol::optional을 사용하여 안전하게 값 추출 시도
	// 이 방식은 키가 없거나(nil), 타입이 일치하지 않을 때 예외를 던지지 않고 
	// 빈 optional 객체를 반환합니다.
	sol::optional<T> result = m_luaState[key];
	if (result) return result.value();

	// 3. 추출 실패 시 기본값 반환
	return defaultValue;
}