#pragma once
#include "Assets/RawMaterial.h"
#include "Assets/RawMesh.h"
#include "Assets/RawNode.h"
#include "Assets/RawKeyPosition.h"
#include "Assets/RawKeyRotation.h"
#include "Assets/RawAnimation.h"

namespace MGF3D
{
	class AssetUtils
	{
		MGF_DECLARE_UTILITIES(AssetUtils)

	/*=============================//
	//  File writing util methods  //
	//=============================*/
	public:
		template<typename T> static void WriteData(OutputFileStream& file, const T& data);
		template<typename T> static void WriteVector(OutputFileStream& file, const Vector<T>& vec);
		static void WriteString(OutputFileStream& file, const String& str);

	/*=============================//
	//  File reading util methods  //
	//=============================*/
	public:
		template<typename T> static void ReadData(InputFileStream& file, T& data);
		template<typename T> static T ReadData(InputFileStream& file);
		template<typename T> static void ReadVector(InputFileStream& file, Vector<T>& vec);
		static String ReadString(InputFileStream& file);

	/*==================================//
	//  asset format read util methods  //
	//==================================*/
	public:
		static RawMaterial ReadRawMaterial(InputFileStream& file);
		static RawMesh ReadRawMesh(InputFileStream& file);
		static Vector<RawNode> ReadRawNodes(InputFileStream& file);
		static RawKeyPosition ReadKeyPosition(InputFileStream& file);
		static RawKeyRotation ReadKeyRotation(InputFileStream& file);
		static RawKeyScale ReadKeyScale(InputFileStream& file);
		static RawAnimation ReadRawAnimation(InputFileStream& file);
	};
}

#include "CoreUtils/AssetUtils.inl"