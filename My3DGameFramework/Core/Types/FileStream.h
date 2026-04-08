#pragma once
#include <fstream>
#include <sstream>
namespace MGF3D
{
	/*===================================//
	//       File Stream Aliases         //
	//===================================*/

	// 입력 파일 스트림 (파일 읽기 전용)
	using InputFileStream = std::ifstream;

	// 출력 파일 스트림 (파일 쓰기 전용)
	using OutputFileStream = std::ofstream;

	// 입출력 파일 스트림 (읽기/쓰기 모두 가능)
	using FileStream = std::fstream;

	// 스트링 스트림
	using StringStream = std::stringstream;
}