#include "CorePch.h"
#include <fstream>
#include "JsonParser.h"
#include "Stream/Stream.h"
#include "Stream/FileStream.h"

namespace MGF3D
{
	JsonParser::JsonParser() = default;
	JsonParser::~JsonParser() = default;

	bool JsonParser::LoadFromJsonFile(const MGFPath& path)
	{
        //// 1. 파일 스트림 열기
        //std::ifstream file(path.GetCStr());
        //if (!file.is_open())
        //{
        //    MGF_LOG_ERROR("JsonParser: Failed to open file: {}", path.GetCStr());
        //    return false;
        //}

        //// 2. 예외 없이 파싱 수행 (callback: nullptr, allow_exceptions: false)
        //m_data = nlohmann::json::parse(file, nullptr, false);

        //// 3. 파싱 결과 유효성 체크
        //if (m_data.is_discarded())
        //{
        //    MGF_LOG_ERROR("JsonParser: Syntax error in JSON: {}", path.GetCStr());
        //    return false;
        //}

        //return true;
        // 
        // 파일 존재 여부 먼저 체크
        if (!path.Exists()) return false;

        // 임시로 FileStream을 열어 LoadFromStream에 넘깁니다.
        // ReadOnly 모드로 여는 것이 안전합니다.
        auto file = MakeShared<FileStream>(path, FileMode::Open, FileAccess::Read);
        return LoadFromStream(file);
	}

    bool JsonParser::LoadFromStream(StreamPtr stream)
    {
        if (!stream || !stream->CanRead())
        {
            MGF_LOG_ERROR("JsonParser: Provided stream is null or cannot be read.");
            return false;
        }

        // 0. 스트림 길이를 얻어오기
        uint64 length = stream->GetLength();
        if (length == 0) return false;

        // 1. 스트림 버퍼 생성
        LString buffer;
        buffer.Resize(static_cast<usize>(length));
        stream->Read(buffer.Data(), static_cast<usize>(length));

        // 2. nlohmann::json은 LString의 CStr()을 통해 파싱
        m_data = nlohmann::json::parse(buffer.CStr(), nullptr, false);

        return !m_data.is_discarded();
    }

	bool JsonParser::IsArray(const SString& key) const
	{
		return Contains(key) && m_data[key.CStr()].is_array();
	}

	bool JsonParser::Contains(const SString& key) const
	{
		return m_data.contains(key.CStr());
	}
}