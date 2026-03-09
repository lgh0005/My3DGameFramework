#pragma once
#include "AssetFormat.h"

namespace MGF3D
{
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
		static void WriteKeyVector3(std::ofstream& file, const AssetFmt::RawKeyPosition& key);
		static void WriteKeyQuaternion(std::ofstream& file, const AssetFmt::RawKeyRotation& key);
		static void WriteRawAnimation(std::ofstream& file, const AssetFmt::RawAnimation& anim);

		/*==================================//
		//  asset format read util methods  //
		//==================================*/
		static std::vector<AssetFmt::RawNode> ReadRawNodes(std::ifstream& file);
	};

}

#include "AssetTypes/AssetUtils.inl"
