#include "GraphicsPch.h"
#include "ShaderImporter.h"
#include "Shader/Shader.h"
#include "Shader/ShaderDescriptor.h"

namespace MGF3D
{
	ShaderImporter::ShaderImporter() = default;
	ShaderImporter::~ShaderImporter() = default;

	bool ShaderImporter::Load(AssetPtr& outAsset, IAssetDescriptorUPtr desc)
	{
		// 1. 안전한 캐스팅 (RTTI 대신 typeID 체크)
		if (desc->type->name.GetStringHash() != "Shader"_sh) return false;

		// 2. Descriptor를 구체적인 타입으로 취급 (정적 캐스팅)
		auto* shaderDesc = static_cast<Ptr<ShaderDescriptor>>(desc.Get());

		// 3. Descriptor를 기반으로 껍데기(Asset) 생성
		// 비동기 로드 시스템에 맞게, 여기서 무거운 작업(OnLoad)을 수행하지 않고
		// 멤버 변수만 초기화된 객체를 반환합니다.
		auto shader = Shader::Create(Move(*shaderDesc));
		if (!shader) return false;

		// 4. 결과물 전달
		outAsset = shader;
		return true;
	}
}