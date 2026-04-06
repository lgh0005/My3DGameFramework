#pragma once
#include "AssetFormat.h"

namespace MGF3D
{
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
}

#include "CoreUtils/AssetUtils.inl"