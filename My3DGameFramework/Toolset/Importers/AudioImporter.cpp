#include "EnginePch.h"
#include "AudioImporter.h"

DECLARE_DEFAULTS_IMPL(AudioImporter)
//REGISTER_IMPORTER(".wav", AudioImporter)
//REGISTER_IMPORTER(".mp3", AudioImporter)
//REGISTER_IMPORTER(".ogg", AudioImporter)

ResourceDescPtr AudioImporter::Import
(
    const ImportContext& ctx,
    AssetRegistry& registry
)
{
    // 1. 이미 동일한 이름의 오디오 설계도가 있는지 확인
    if (registry.find(ctx.assetName) != registry.end())
        return registry[ctx.assetName];

    // 2. AudioClipDesc 생성
    auto desc = AudioClipDescPtr(new AudioClipDesc(ctx.virtualPath, ctx.assetName));

    // 3. 오디오 고유 설정
    // 기본적으로 path 멤버에 가상 경로를 할당합니다.
    desc->path = ctx.virtualPath;
    // 기본 타입은 SFX로 설정되어 있으나, 파일명 등으로 구분 로직을 추가할 수 있습니다.
    // 예: if (ctx.assetName.find("BGM_") != std::string::npos) desc->type = AudioType::BGM;

    // 4. 장부에 등록
    registry[ctx.assetName] = desc;

    LOG_INFO("AudioImporter: Imported -> {} [{}]", ctx.assetName, ctx.virtualPath);
    return desc;
}