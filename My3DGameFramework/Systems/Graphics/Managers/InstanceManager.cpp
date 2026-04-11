#include "EnginePch.h"
#include "InstanceManager.h"
#include "Instancing/RenderQueue.h"
#include "Instancing/AnimBuffer.h"
#include "Resources/Animations/Animation.h"

InstanceManager::InstanceManager() = default;
InstanceManager::~InstanceManager() = default;

bool InstanceManager::Init()
{
	m_globalInstanceQueue = RenderQueue::Create(4096, 0);
	if (!m_globalInstanceQueue) return false;
	return true;
}

void InstanceManager::BuildGlobalAnimBuffer()
{
	// 1. 리소스 매니저에서 로드된 모든 애니메이션 가져오기
	// (구조에 따라 GetAnimations() 같은게 필요함)
	auto animations = RESOURCE.GetAll<Animation>();

	if (animations.empty()) return;

	std::vector<glm::mat4> globalData;
	uint32 currentOffset = 0;

	// 2. 데이터 병합 (Atlas Packing)
	for (auto& anim : animations)
	{
		const auto& baked = anim->GetAnimClip();
		if (baked.localMatrices.empty()) continue;

		// 각 애니메이션에게 "너는 여기서부터 시작해"라고 알려줌
		anim->SetGlobalOffset(currentOffset);

		// 데이터 이어 붙이기
		globalData.insert
		(
			globalData.end(),
			baked.localMatrices.begin(),
			baked.localMatrices.end()
		);

		// 오프셋 전진
		currentOffset += (uint32)baked.localMatrices.size();
	}

	// 3. GPU 버퍼 생성 (딱 한 번!)
	if (!globalData.empty())
	{
		uint32 byteSize = (uint32)globalData.size() * sizeof(glm::mat4);

		// 불변 저장소(Immutable)로 생성
		m_globalAnimBuffer = AnimBuffer::Create(globalData.data(), byteSize);

		LOG_INFO("Global Animation Atlas Built: {} Matrices ({:.2f} MB)",
			globalData.size(), byteSize / (1024.0f * 1024.0f));
	}
}

void InstanceManager::BindGlobalAnimBuffer(uint32 slot)
{
	if (m_globalAnimBuffer) m_globalAnimBuffer->Bind(slot);
}

void InstanceManager::Submit(Mesh* mesh, Material* material, const InstanceProperty& prop)
{
	if (m_globalInstanceQueue) m_globalInstanceQueue->Add(mesh, material, prop);
}

void InstanceManager::Render(Program* program)
{
	if (!m_globalInstanceQueue) return;

	// 1. 전역 애니메이션 버퍼 바인딩
	BindGlobalAnimBuffer(10);

	// 2. 큐 실행 (GPU 인스턴싱 드로우)
	m_globalInstanceQueue->Execute(program);

	// 3. 언바인딩 (선택)
	if (m_globalAnimBuffer)
		m_globalAnimBuffer->Unbind(10);
}

void InstanceManager::ClearQueue()
{
	if (m_globalInstanceQueue) m_globalInstanceQueue->Clear();
}