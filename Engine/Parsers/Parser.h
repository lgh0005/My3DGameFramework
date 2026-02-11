#pragma once

CLASS_PTR(Parser)
class Parser
{
public:
	virtual ~Parser();
	virtual bool LoadFromFile(const std::string& path) = 0;
	virtual bool LoadConfig(const std::string& configPath) = 0;
	virtual void Clear() = 0;

	bool IsLoaded() const { return m_isLoaded; }
	const std::string& GetFilePath() const { return m_filePath; }
	const std::string& GetLastError() const { return m_lastErrorMessage; }

	const std::string& GetToken(uint32 keyHash) const;
	const std::string& GetToken(const std::string& key) const;

protected:
	Parser();
	std::string ResolvePath(const std::string& path);

	std::unordered_map<uint32, std::string> m_tokenMap;
	std::string m_filePath;           // 로드된 파일의 경로
	std::string m_lastErrorMessage;   // 에러 발생 시 로그용 메시지
	bool m_isLoaded = false;          // 로드 성공 여부
};