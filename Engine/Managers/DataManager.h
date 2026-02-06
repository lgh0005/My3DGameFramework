#pragma once

class DataManager
{
	DECLARE_SINGLE(DataManager)
	DECLARE_NONINSTANTIABLE(DataManager)

/*====================================//
//   MetaData file handling methods   //
//====================================*/
public:
	template <typename T_Settings>
	T_Settings GetFinalSettings
	(
		const std::string& virtualPath, 
		const T_Settings& hint, 
		bool forced
	);

private:
	template <typename T_Settings>
	bool LoadMetadata(const fs::path& assetPath, T_Settings& outSettings);

	template <typename T_Settings>
	void SaveMetadata(const fs::path& assetPath, const T_Settings& settings);
};