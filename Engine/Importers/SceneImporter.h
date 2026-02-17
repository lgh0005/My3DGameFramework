#pragma once
#include "Importers/Importer.h"

CLASS_PTR(SceneImporter)
class SceneImporter : public Importer
{
public:
    SceneImporter();
    virtual ~SceneImporter() override;

    virtual ResourceDescPtr Import
    (
        const ImportContext& ctx,
        AssetRegistry& registry
    ) override;
};