#pragma once
#include "Mechanics/Animation/Pose.h"
#include "Assets/RawKeyPosition.h"
#include "Assets/RawKeyRotation.h"
#include "Assets/RawKeyScale.h"

namespace MGF3D
{
	MGF_CLASS_PTR(AnimChannel)
	class AnimChannel
	{
		MGF_DISABLE_COPY(AnimChannel)

	public:
		~AnimChannel();
		static AnimChannelUPtr Create
		(
			const String& name,
			Vector<RawKeyPosition>&& positions,
			Vector<RawKeyRotation>&& rotations,
			Vector<RawKeyScale>&& scales
		);

		Pose GetPose(float time) const;

	/*==============================================//
	//   animation channel default getter methods   //
	//==============================================*/
	public:
		uint32 GetPositionIndex(float animationTime) const;
		uint32 GetRotationIndex(float animationTime) const;
		uint32 GetScaleIndex(float animationTime) const;

		StringHash GetNameHash() const;
		const String& GetBoneName() const;
		mat4 GetLocalTransform(float animationTime) const;

	/*===========================//
	//   interpolation methods   //
	//===========================*/
	private:
		float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime) const;
		vec3 InterpolatePosition(float animationTime) const;
		quat InterpolateRotation(float animationTime) const;
		vec3 InterpolateScaling(float animationTime) const;

	private:
		AnimChannel();
		void Init
		(
			const String& name,
			Vector<RawKeyPosition>&& positions,
			Vector<RawKeyRotation>&& rotations,
			Vector<RawKeyScale>&& scales
		);

		StringHash m_nameHash;
		String m_name;

		Vector<RawKeyPosition> m_positions;
		Vector<RawKeyRotation> m_rotations;
		Vector<RawKeyScale>    m_scales;

		uint32 m_numPositions{ 0 };
		uint32 m_numRotations{ 0 };
		uint32 m_numScalings{ 0 };
	};
}
