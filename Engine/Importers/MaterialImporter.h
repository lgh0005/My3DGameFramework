#pragma once
#include "Importers/Importer.h"

CLASS_PTR(MaterialImporter)
class MaterialImporter : public Importer
{
public:
	MaterialImporter();
	virtual ~MaterialImporter() override;

    virtual ResourceDescPtr Import
    (
        const ImportContext& ctx,
        AssetRegistry& registry
    ) override;
};