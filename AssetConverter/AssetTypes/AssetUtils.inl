#pragma once

namespace MGF3D
{
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
}