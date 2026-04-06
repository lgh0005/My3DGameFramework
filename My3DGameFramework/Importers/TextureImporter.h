#pragma once
#include "Importers/Importer.h"

CLASS_PTR(TextureImporter)
class TextureImporter : public Importer
{
public:
	TextureImporter();
	~TextureImporter();

	virtual ResourceDescPtr Import
	(
		const ImportContext& ctx,
		AssetRegistry& registry
	) override;
};