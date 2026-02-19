#pragma once
#include "Importers/Importer.h"

CLASS_PTR(AudioImporter)
class AudioImporter : public Importer
{
public:
    AudioImporter();
    virtual ~AudioImporter() override;

    virtual ResourceDescPtr Import
    (
        const ImportContext& ctx,
        AssetRegistry& registry
    ) override;
};