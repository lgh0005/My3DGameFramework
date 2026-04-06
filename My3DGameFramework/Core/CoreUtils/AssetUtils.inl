#pragma once

namespace MGF3D
{
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
}