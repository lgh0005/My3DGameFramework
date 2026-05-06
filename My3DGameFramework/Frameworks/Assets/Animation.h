#pragma once
#include "Sources/Asset.h"
#include "Mechanics/Animation/AnimChannel.h"
#include "Mechanics/Animation/AnimClip.h"

namespace MGF3D
{
	MGF_CLASS_PTR(AnimClip)

	MGF_CLASS_PTR(Animation)
	class Animation : public Asset
	{
		MGF_DISABLE_COPY(Animation)
		using Super = Asset;

	/*========================//
	//     Animation Type     //
	//========================*/
	public:
		static int16 s_typeIndex;
		virtual const MGFType* GetType() const override;
		static AnimationPtr Create(const String& path);

	/*========================//
	//      Asset Loading     //
	//========================*/
	public:
		virtual ~Animation() override;
		virtual bool Load() override;

	/*========================//
	//      Data Getters      //
	//========================*/
	public:
		const AnimClipPtr& GetAnimClip() const { return m_animClip; }
		const String& GetName() const { return m_name; }

		float GetDuration() const { return m_duration; }
		float GetTicksPerSecond() const { return m_ticksPerSecond; }

		uint32 GetGlobalOffset() const { return m_globalOffset; }
		void SetGlobalOffset(uint32 offset) { m_globalOffset = offset; }

		// [CPU Skinning / Blend] StringHash 기반 채널 검색
		const AnimChannel* FindChannel(StringHash nameHash) const;

	// [DEBUG]
	public:
		// 현재 로드된 모든 애니메이션 채널의 원본 이름을 리스트로 반환 (디버깅용)
		Vector<String> GetChannelNames() const;

		// 로그에 모든 채널 이름을 즉시 출력
		void DebugPrintChannels() const;

	private:
		Animation(const String& path);
		String m_name;
		float m_duration{ 0.0f };
		float m_ticksPerSecond{ 0.0f };

		// 베이킹된 글로벌 오프셋과 행렬 데이터 모음
		AnimClipPtr m_animClip;
		uint32 m_globalOffset{ 0 };

		// 원본 키프레임 데이터 (CPU 스킨닝 및 보간 용도)
		Vector<AnimChannelUPtr> m_channels;
		HashMap<StringHash, AnimChannel*> m_channelMap;
	};
}