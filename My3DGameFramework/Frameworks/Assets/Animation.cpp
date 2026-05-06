#include "FrameworkPch.h"
#include "Animation.h"
#include "Managers/TypeManager.h"
#include "CoreUtils/AssetUtils.h"
#include "Mechanics/Animation/AnimClip.h"

namespace MGF3D
{
	Animation::Animation(const String& path) : Super(path) {}
	Animation::~Animation() = default;

	AnimationPtr Animation::Create(const String& path)
	{
		return AnimationPtr(new Animation(path));
	}

	/*========================//
	//     Animation Type     //
	//========================*/
	int16 Animation::s_typeIndex = -1;
	const MGFType* Animation::GetType() const
	{
		MGFTypeTree* tree = MGF_TYPE.GetTree("Asset");
		if (tree != nullptr) return tree->GetType(s_typeIndex);
		return nullptr;
	}

	/*========================//
	//      Asset Loading     //
	//========================*/
	bool Animation::Load()
	{
		InputFileStream inFile(m_path, std::ios::binary);
		if (!inFile)
		{
			MGF_LOG_ERROR("Animation::Load - Failed to open file: {}", m_path);
			SetState(EAssetState::Failed);
			return false;
		}

		// 1. AssetUtils를 통해 .myanim 구조체 전체를 파싱
		RawAnimation rawAnim = AssetUtils::ReadRawAnimation(inFile);
		inFile.close();

		// 2. 매직 넘버 검증
		if (rawAnim.magic != 0x414E494D) // "ANIM"
		{
			MGF_LOG_ERROR("Animation::Load - Invalid magic number in: {}", m_path);
			SetState(EAssetState::Failed);
			return false;
		}

		// 3. 메타데이터 바인딩
		m_name = std::move(rawAnim.name);
		m_duration = rawAnim.duration;
		m_ticksPerSecond = rawAnim.ticksPerSecond;
		m_animClip = AnimClipPtr
		(
			new AnimClip
			(
				rawAnim.frameCount,
				rawAnim.boneCount,
				rawAnim.frameRate,
				std::move(rawAnim.bakedMatrices)
			)
		);

		// 4. 채널 데이터 파싱 및 Map 구축
		m_channels.reserve(rawAnim.channels.size());
		for (auto& rawCh : rawAnim.channels)
		{
			// 키로 사용할 해시값을 먼저 계산
			StringHash nameHash(rawCh.nodeName);

			auto newChannel = AnimChannel::Create
			(
				rawCh.nodeName,
				std::move(rawCh.positions),
				std::move(rawCh.rotations),
				std::move(rawCh.scales)
			);

			// Map에 StringHash 키로 등록
			m_channelMap[nameHash] = newChannel.get();
			m_channels.push_back(std::move(newChannel));
		}

		// 5. 유효성 검증
		if (!m_animClip->IsValid())
		{
			MGF_LOG_ERROR("Animation::Load - Loaded [{}] but AnimClip is empty/invalid.", m_name);
			SetState(EAssetState::Failed);
			return false;
		}

		MGF_LOG_INFO("Loaded Baked Animation: {} ({} frames, {} bones)", m_name, m_animClip->GetFrameCount(), m_animClip->GetBoneCount());

		// 6. 성공 시 에셋 상태를 Loaded로 전환 (Animation은 내부 리소스가 없으므로 Update 단계에서 바로 Ready가 됩니다)
		SetState(EAssetState::Loaded);
		return true;
	}

	/*========================//
	//      Data Getters      //
	//========================*/
	const AnimChannel* Animation::FindChannel(StringHash nameHash) const
	{
		auto it = m_channelMap.find(nameHash);
		if (it != m_channelMap.end()) return it->second;
		return nullptr;
	}

	Vector<String> Animation::GetChannelNames() const
	{
		Vector<String> names;
		names.reserve(m_channels.size());

		for (const auto& ch : m_channels)
		{
			// AnimChannel::GetName()이 구현되어 있다고 가정합니다.
			names.push_back(ch->GetBoneName());
		}
		return names;
	}

	void Animation::DebugPrintChannels() const
	{
		MGF_LOG_FATAL("------- [Animation Debug] Channel Names in '{}' -------", m_name);
		for (const auto& ch : m_channels)
		{
			MGF_LOG_FATAL(" - Channel: '{}' (Hash: {})", ch->GetBoneName(), StringHash(ch->GetBoneName()).value);
		}
		MGF_LOG_FATAL("-------------------------------------------------------");
	}
}