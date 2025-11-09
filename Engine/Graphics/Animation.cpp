#include "Animation.h"
#include "Graphics/Model.h"
#include "Graphics//Bone.h"

AnimationUPtr Animation::Create(const std::string& animationPath, Model* model)
{
	auto animation = AnimationUPtr(new Animation());
	if (!animation->Init(animationPath, model)) return nullptr;
	return animation;
}

bool Animation::Init(const std::string& animationPath, Model* model)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(animationPath, aiProcess_Triangulate);
	
	if (!scene || !scene->mRootNode)
	{
		SPDLOG_ERROR("Failed to load animation file: {}", animationPath);
		return false;
	}

	auto animation = scene->mAnimations[0];
	m_duration = animation->mDuration;
	m_ticksPerSecond = animation->mTicksPerSecond;
	ReadHeirarchyData(m_rootNode, scene->mRootNode);
	ReadMissingBones(animation, *model);

	return true;
}

Bone* Animation::FindBone(const std::string& name)
{
	auto it = std::find_if
	(
		m_bones.begin(), m_bones.end(),
		[&](const BoneUPtr& bonePtr) { return bonePtr->GetBoneName() == name; }
	);
	if (it == m_bones.end()) return nullptr;
	else return it->get();
}

void Animation::ReadMissingBones(const aiAnimation* animation, Model& model)
{
	uint32 size = animation->mNumChannels;
	auto& boneInfoMap = model.GetBoneInfoMap();
	int32& boneCount = model.GetBoneCount();

	for (int i = 0; i < size; i++)
	{
		auto channel = animation->mChannels[i];
		std::string boneName = channel->mNodeName.data;

		if (boneInfoMap.find(boneName) == boneInfoMap.end())
		{
			boneInfoMap[boneName].id = boneCount;
			boneCount++;
		}
		m_bones.push_back(Bone::Create(channel->mNodeName.data,
			boneInfoMap[channel->mNodeName.data].id, channel));
	}

	m_boneInfoMap = boneInfoMap;
}

void Animation::ReadHeirarchyData(AssimpNodeData& dest, const aiNode* src)
{
	if (!src) return;

	dest.name = src->mName.data;
	dest.transformation = Utils::ConvertMatrixToGLMFormat(src->mTransformation);
	dest.childrenCount = src->mNumChildren;

	for (int32 i = 0; i < src->mNumChildren; i++)
	{
		AssimpNodeData newData;
		ReadHeirarchyData(newData, src->mChildren[i]);
		dest.children.push_back(newData);
	}
}
