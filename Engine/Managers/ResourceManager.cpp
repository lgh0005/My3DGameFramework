#include "EnginePch.h"
#include "ResourceManager.h"
#include "Graphics/StaticMesh.h"
#include "Graphics/Mesh.h"
#include "Graphics/Material.h"
#include "Graphics/Program.h"
#include "Graphics/Texture.h"
#include "Graphics/CubeTexture.h"
#include "Graphics/Model.h"
#include "Graphics/Animation.h"
#include "Audios/AudioClip.h"

// TODO : 이후에 음원과 관련된 리소스들도 추가 필요
void ResourceManager::Clear()
{
	// TODO : m_audioClips를 어디에 적어야?
	
	// 1. 최상위 복합 리소스 (다른 리소스를 참조할 가능성이 높은 것들)
	m_models.clear();     // Model은 Mesh나 Material을 들고 있을 수 있음
	m_animations.clear();

	// 2. 중간 단계 리소스
	m_materials.clear();  // Material은 Texture를 들고 있음

	// 3. 최하위 기본 리소스 (다른 리소스에 의해 참조되는 것들)
	m_meshes.clear();
	m_cubeTextures.clear();
	m_textures.clear();

	// 4. Misc
	m_audioClips.clear();
}