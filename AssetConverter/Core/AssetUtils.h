#pragma once

namespace MGF3D
{
	class AssetUtils
	{
		MGF_DECLARE_UTILITIES(AssetUtils)

	/*=============================//
	//  file writing util methods  //
	//=============================*/
	public:
		template<typename T> static void WriteData(OutputFileStream& file, const T& data);
		template<typename T> static void WriteVector(OutputFileStream& file, const Vector<T>& vec);
		static void WriteString(OutputFileStream& file, const String& str);

	/*=============================//
	//  file reading util methods  //
	//=============================*/
	public:
		template<typename T> static void ReadData(InputFileStream& file, T& data);
		template<typename T> static T ReadData(InputFileStream& file);
		static String ReadString(InputFileStream& file);

	/*===================================//
	//  asset format write util methods  //
	//===================================*/
	public:
		static void WriteRawMaterial(OutputFileStream& file, const RawMaterial& mat);
		static void WriteRawMesh(OutputFileStream& file, const RawMesh& mesh);
		static void WriteRawNodes(OutputFileStream& file, const Vector<RawNode>& nodes);
		static void WriteKeyPosition(OutputFileStream& file, const RawKeyPosition& key);
		static void WriteKeyRotation(OutputFileStream& file, const RawKeyRotation& key);
		static void WriteKeyScale(OutputFileStream& file, const RawKeyScale& key);
		static void WriteRawAnimation(OutputFileStream& file, const RawAnimation& anim);

		/*==================================//
		//  asset format read util methods  //
		//==================================*/
		static std::vector<RawNode> ReadRawNodes(InputFileStream& file);
	};
}

#include "Core/AssetUtils.inl"
