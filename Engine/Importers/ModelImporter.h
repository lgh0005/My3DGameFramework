#pragma once
#include "Importers/Importer.h"

CLASS_PTR(ModelImporter)
class ModelImporter : public Importer
{
public:
	ModelImporter();
	~ModelImporter();

	virtual ResourceDescPtr Import
	(
		const ImportContext& ctx,
		AssetRegistry& registry
	) override;
};