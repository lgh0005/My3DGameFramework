#pragma once
#include "AssetFormat.h"

class AssetUtils
{
/*=============================//
//  file writing util methods  //
//=============================*/
public:
	template<typename T> static void WriteData(std::ofstream& file, const T& data);
	template<typename T> static void WriteVector(std::ofstream& file, const std::vector<T>& vec);
	static void WriteString(std::ofstream& file, const std::string& str);

/*=============================//
//  file reading util methods  //
//=============================*/
public:
	template<typename T> static void ReadData(std::ifstream& file, T& data);
	template<typename T> static T ReadData(std::ifstream& file);
	static std::string ReadString(std::ifstream& file);

/*===================================//
//  asset format write util methods  //
//===================================*/
public:
	static void WriteRawMaterial(std::ofstream& file, const AssetFmt::RawMaterial& mat);
	static void WriteRawMesh(std::ofstream& file, const AssetFmt::RawMesh& mesh);
	static void WriteRawNodes(std::ofstream& file, const std::vector<AssetFmt::RawNode>& nodes);
};

/*================================//
//  asset utils template inlines  //
//================================*/
#pragma region ASSET_UTILS_TEMPLATE_INLINES

//  File writing util methods 
template<typename T>
inline void AssetUtils::WriteData(std::ofstream& file, const T& data)
{
	file.write(reinterpret_cast<const char*>(&data), sizeof(T));
}

template<typename T>
inline void AssetUtils::WriteVector(std::ofstream& file, const std::vector<T>& vec)
{
	uint32 count = static_cast<uint32>(vec.size());
	file.write(reinterpret_cast<const char*>(&count), sizeof(uint32));
	if (count > 0) file.write(reinterpret_cast<const char*>(vec.data()), sizeof(T) * count);
}

//  File reading util methods 
template<typename T>
inline void AssetUtils::ReadData(std::ifstream& file, T& data)
{
	file.read(reinterpret_cast<char*>(&data), sizeof(T));
}

template<typename T>
inline T AssetUtils::ReadData(std::ifstream& file)
{
	T data;
	file.read(reinterpret_cast<char*>(&data), sizeof(T));
	return data;
}

#pragma endregion