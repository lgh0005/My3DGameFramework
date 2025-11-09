#include "EnginePch.h"
#include "Animator.h"
#include "Graphics/Animation.h"
#include "Graphics//Bone.h"

AnimatorUPtr Animator::Create(AnimationUPtr animation)
{
	auto animator = AnimatorUPtr(new Animator());
	if (!animator->Init(std::move(animation))) return nullptr;
	return animator;
}

bool Animator::Init(AnimationUPtr animation)
{
	m_currentTime = 0.0;
	m_currentAnimation = std::move(animation);

	usize boneMatSize = 100; // 크기 하드코딩
	m_finalBoneMatrices.reserve(boneMatSize);
	for (int i = 0; i < boneMatSize; i++)
		m_finalBoneMatrices.push_back(glm::mat4(1.0f));
}

void Animator::UpdateAnimation()
{
	if (m_currentAnimation)
	{
		m_currentTime += m_currentAnimation->GetTicksPerSecond() * TIME.GetDeltaTime();
		m_currentTime = fmod(m_currentTime, m_currentAnimation->GetDuration());
		CalculateBoneTransform(&m_currentAnimation->GetRootNode(), glm::mat4(1.0f));
	}
}

void Animator::PlayAnimation(AnimationUPtr animation)
{
	m_currentAnimation = std::move(animation);
	m_currentTime = 0.0f;
}

void Animator::CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform)
{
	std::string nodeName = node->name;
	glm::mat4 nodeTransform = node->transformation;

	Bone* bone = m_currentAnimation->FindBone(nodeName);
	if (bone)
	{
		bone->Update(m_currentTime);
		nodeTransform = bone->GetLocalTransform();
	}

	glm::mat4 globalTransformation = parentTransform * nodeTransform;
	auto boneInfoMap = m_currentAnimation->GetBoneIDMap();
	if (boneInfoMap.find(nodeName) != boneInfoMap.end())
	{
		int index = boneInfoMap[nodeName].id;
		glm::mat4 offset = boneInfoMap[nodeName].offset;
		m_finalBoneMatrices[index] = globalTransformation * offset;
	}

	for (int i = 0; i < node->childrenCount; i++)
		CalculateBoneTransform(&node->children[i], globalTransformation);
}