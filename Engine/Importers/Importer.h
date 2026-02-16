#pragma once

struct ImportContext
{
	std::string filePath;
	std::string assetName;
	std::string virtualPath;
};

CLASS_PTR(Importer)
class Importer
{
public:
	virtual ~Importer();
	Importer();

	using AssetRegistry = std::unordered_map<std::string, ResourceDescPtr>;
	using ImporterRegistry = std::unordered_map<std::string, ImporterUPtr>;

	virtual ResourceDescPtr Import
	(
		const ImportContext& ctx,
		AssetRegistry& registry
	) = 0;

public:
	static void Register(const std::string& ext, ImporterUPtr importer);
	static ResourceDescPtr ImportAsset
	(
		const std::string& ext,
		const ImportContext& ctx,
		AssetRegistry& registry
	);

private:
	static ImporterRegistry& GetRegistry();
};