#pragma once
#include "Importers/Importer.h"

CLASS_PTR(ScriptImporter)
class ScriptImporter : public Importer
{
public:
	ScriptImporter();
	virtual ~ScriptImporter() override;

    virtual ResourceDescPtr Import
    (
        const ImportContext& ctx,
        AssetRegistry& registry
    ) override;
};