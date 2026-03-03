#pragma once
#include "Importers/Importer.h"

CLASS_PTR(ShaderImporter)
class ShaderImporter : public Importer
{
public:
	ShaderImporter();
	~ShaderImporter();

	virtual ResourceDescPtr Import
	(
		const ImportContext& ctx,
		AssetRegistry& registry
	) override;
};