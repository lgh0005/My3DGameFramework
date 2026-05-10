#include "FrameworkPch.h"
#include "SkyCube.h"
#include "Managers/TypeManager.h"
#include "Resources/EnvironmentMap.h"
#include "Textures/GLTextureCube.h"
#include "Textures/GLTexture2D.h"

namespace MGF3D
{
	SkyCube::SkyCube(Component* owner) : Super(owner) {}
	SkyCube::~SkyCube() = default;
	SkyCube::SkyCube(SkyCube&& other) noexcept = default;
	SkyCube& SkyCube::operator=(SkyCube&& other) noexcept = default;

	/*================================//
	//   MGF3D Mechanic Custom Type   //
	//================================*/
	int16 SkyCube::s_typeIndex = -1;
	const MGFType* SkyCube::GetType() const
	{
		MGFTypeTree* tree = MGF_TYPE.GetTree("Mechanic");
		if (tree != nullptr) return tree->GetType(s_typeIndex);
		return nullptr;
	}

	void SkyCube::BindIBLTextures(uint32 irradSlot, uint32 prefilterSlot, uint32 brdfSlot) const
	{
		if (!m_environmentMap) return;

		GLTextureCubePtr irrad = m_environmentMap->GetIrradiance();
		GLTextureCubePtr pref = m_environmentMap->GetPrefiltered();
		GLTexture2DPtr brdf = m_environmentMap->GetBrdfLUT();

		if (irrad) irrad->Bind(irradSlot);
		if (pref) pref->Bind(prefilterSlot);
		if (brdf) brdf->Bind(brdfSlot);
	}
}