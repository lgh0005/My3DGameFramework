#pragma once
#include "Importers/Importer.h"

CLASS_PTR(AnimationImporter)
class AnimationImporter : public Importer
{
public:
    AnimationImporter();
    virtual ~AnimationImporter() override;

    virtual ResourceDescPtr Import
    (
        const ImportContext& ctx,
        AssetRegistry& registry
    ) override;
};