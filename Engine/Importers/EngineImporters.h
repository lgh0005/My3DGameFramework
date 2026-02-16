#pragma once

// 1. 매크로 도구함 포함 (REGISTER_IMPORTER 정의)
/*======================================================//
//    self-registration macro for resource importers    //
//======================================================*/
#define CONCAT_IMPL(x, y) x##y
#define CONCAT(x, y) CONCAT_IMPL(x, y)
#define REGISTER_IMPORTER(Ext, ImporterClass)                                            \
    static bool CONCAT(_reg_##ImporterClass##_, __COUNTER__) = []()                      \
    {                                                                                    \
        Importer::Register(Ext, std::make_unique<ImporterClass>());                      \
        return true;                                                                     \
    }();

// 2. 엔진이 기본으로 제공하는 모든 임포터 헤더들
#include "Importers/TextureImporter.h"
#include "Importers/ShaderImporter.h"
#include "Importers/ModelImporter.h"
#include "Importers/AudioImporter.h"
#include "Importers/FontImporter.h"
#include "Importers/AnimationImporter.h"

/*==========================================================================//
//   [STB Style] Engine Default Importer Registration                      //
//   이 영역은 ENGINE_IMPORTERS_IMPLEMENTATION이 정의된 단 한 곳에서만        //
//   실제 정적 변수들로 실체화되어 링커에 의해 강제 연결됩니다.                //
//==========================================================================*/

#ifdef ENGINE_IMPORTERS_IMPLEMENTATION

// 텍스처 (다양한 확장자를 하나의 임포터에 매핑)
REGISTER_IMPORTER(".ktx", TextureImporter)

// 셰이더 (여러 스테이지를 하나의 프로그램 설계도로 병합)
REGISTER_IMPORTER(".vert", ShaderImporter)
REGISTER_IMPORTER(".frag", ShaderImporter)
REGISTER_IMPORTER(".geom", ShaderImporter)
REGISTER_IMPORTER(".comp", ShaderImporter)
REGISTER_IMPORTER(".tesc", ShaderImporter)
REGISTER_IMPORTER(".tese", ShaderImporter)

// 모델 및 애니메이션 (커스텀 바이너리 포맷)
REGISTER_IMPORTER(".mymodel", ModelImporter)
REGISTER_IMPORTER(".myanim", AnimationImporter)

// 오디오 및 폰트
REGISTER_IMPORTER(".wav", AudioImporter)
REGISTER_IMPORTER(".mp3", AudioImporter)
REGISTER_IMPORTER(".ogg", AudioImporter)
REGISTER_IMPORTER(".ttf", FontImporter)
REGISTER_IMPORTER(".otf", FontImporter)

#endif