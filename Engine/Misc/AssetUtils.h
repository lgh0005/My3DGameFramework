#pragma once
#include "AssetFormat.h"

class AssetUtils
{
	DECLARE_STATIC_CLASS(AssetUtils)

/*=============================//
//  File writing util methods  //
//=============================*/
public:
	template<typename T> static void WriteData(std::ofstream& file, const T& data);
	template<typename T> static void WriteVector(std::ofstream& file, const std::vector<T>& vec);
	static void WriteString(std::ofstream& file, const std::string& str);

/*=============================//
//  File reading util methods  //
//=============================*/
public:
	template<typename T> static void ReadData(std::ifstream& file, T& data);
	template<typename T> static T ReadData(std::ifstream& file);
	template<typename T> static void ReadVector(std::ifstream& file, std::vector<T>& vec);
	static std::string ReadString(std::ifstream& file);

/*==================================//
//  asset format read util methods  //
//==================================*/
public:
	static AssetFmt::RawMaterial ReadRawMaterial(std::ifstream& file);
	static AssetFmt::RawMesh ReadRawMesh(std::ifstream& file);
	static std::vector<AssetFmt::RawNode> ReadRawNodes(std::ifstream& file);
	static AssetFmt::RawKeyPosition ReadKeyVector3(std::ifstream& file);
	static AssetFmt::RawKeyRotation ReadKeyQuaternion(std::ifstream& file);
	static AssetFmt::RawAnimation ReadRawAnimation(std::ifstream& file);
};

/*================================//
//  Asset utils template inlines  //
//================================*/
#pragma region ASSET_UTILS_TEMPLATE_INLINES

//  File writing util methods 
template<typename T>
inline static void AssetUtils::WriteData(std::ofstream& file, const T& data)
{
	file.write(reinterpret_cast<const char*>(&data), sizeof(T));
}

template<typename T>
inline static void AssetUtils::WriteVector(std::ofstream& file, const std::vector<T>& vec)
{
	uint32 count = static_cast<uint32>(vec.size());
	file.write(reinterpret_cast<const char*>(&count), sizeof(uint32));
	if (count > 0) file.write(reinterpret_cast<const char*>(vec.data()), sizeof(T) * count);
}

//  File reading util methods 
template<typename T>
inline static void AssetUtils::ReadData(std::ifstream& file, T& data)
{
	file.read(reinterpret_cast<char*>(&data), sizeof(T));
}

template<typename T>
inline static T AssetUtils::ReadData(std::ifstream& file)
{
	T data = {};
	file.read(reinterpret_cast<char*>(&data), sizeof(T));
	return data;
}

template<typename T>
inline static void AssetUtils::ReadVector(std::ifstream& file, std::vector<T>& vec)
{
	uint32 count = 0;
	file.read(reinterpret_cast<char*>(&count), sizeof(uint32));

	vec.resize(count);
	if (count > 0)
	{
		file.read(reinterpret_cast<char*>(vec.data()), sizeof(T) * count);
	}
}

#pragma endregion