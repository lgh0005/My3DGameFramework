#pragma once
#include "Importers/Importer.h"

CLASS_PTR(FontImporter)
class FontImporter : public Importer
{
public:
	FontImporter();
	virtual ~FontImporter() override;

	virtual ResourceDescPtr Import
	(
		const ImportContext& ctx,
		AssetRegistry& registry
	) override;
};