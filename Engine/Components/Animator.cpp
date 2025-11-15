#include "EnginePch.h"
#include "Animator.h"
#include "Graphics/Bone.h"

AnimatorUPtr Animator::Create(AnimationPtr animation)
{
	auto animator = AnimatorUPtr(new Animator());
	if (!animator->Init(std::move(animation))) return nullptr;
	return animator;
}

bool Animator::Init(AnimationPtr animation)
{
	if (!animation)
	{
		SPDLOG_ERROR("Animator::Init failed: animation is nullptr");
		return false;
	}

	m_currentTime = 0.0;
	m_currentAnimation = std::move(animation);

	usize boneMatSize = m_currentAnimation->GetBoneIDMap().size();
	m_finalBoneMatrices.resize(boneMatSize, glm::mat4(1.0f));

	return true;
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

void Animator::PlayAnimation(AnimationPtr animation)
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
	auto& boneInfoMap = m_currentAnimation->GetBoneIDMap();

	auto it = boneInfoMap.find(nodeName);
	if (it != boneInfoMap.end())
	{
		int index = it->second.id;
		glm::mat4 offset = it->second.offset;
		m_finalBoneMatrices[index] = globalTransformation * offset;
	}

	for (int i = 0; i < node->childrenCount; i++)
		CalculateBoneTransform(&node->children[i], globalTransformation);
}