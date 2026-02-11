#include "EnginePch.h"
#include "Parser.h"

DECLARE_DEFAULTS_IMPL(Parser)

std::string Parser::ResolvePath(const std::string& path)
{
	// TODO : 이후에 여기는 좀더 폴더 경로와 함께 생각을 해보긴 해야함.
	// 1. 이미 절대 경로인 경우 그대로 반환
	std::filesystem::path p(path);
	if (p.is_absolute()) return path;

	// 2. 엔진의 리소스 루트 경로와 결합 (예: Assets/...)
	// INFO : 실제 환경 변수나 설정 파일에서 루트 경로를 가져오는 로직이 필요
	std::filesystem::path rootPath = std::filesystem::current_path(); // 혹은 전역 Config의 리소스 경로
	std::filesystem::path fullPath = rootPath / p;

	// 3. 경로 정문화 (../ 등을 정리)
	return fullPath.lexically_normal().string();
}

const std::string& Parser::GetToken(uint32 keyHash) const
{
	auto it = m_tokenMap.find(keyHash);
	if (it != m_tokenMap.end()) return it->second;
	return Utils::GetEmptyStr();
}

const std::string& Parser::GetToken(const std::string& key) const
{
	return GetToken(Utils::StrHash(key));
}