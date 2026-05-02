#pragma once

namespace MGF3D
{
	class AnimationConverter
	{
		MGF_DECLARE_SINGLE(AnimationConverter)

	private:
		AnimationConverter();
		~AnimationConverter();

	public:
		bool Convert
		(
			const String& animPath,
			const String& refModelPath,
			const String& outputPath
		);

	private:
		bool LoadReferenceSkeleton(const String& path);
		AnimChannel ConvertAssimpChannelToEngine(aiNodeAnim* srcChannel);
		void BakeAnimation(const aiAnimation* srcAnim, RawAnimation& outAnim);
		String MakeSafeName(const String& rawName);
		bool WriteAnimationFile(const String& finalPath, const RawAnimation& anim);

	private:
		Vector<RawBone> m_bones;
		HashMap<String, int32> m_boneNameMap;

		/*=============================//
		//   multi-threading members   //
		//=============================*/
	private:
		Atomic<int32> m_activeTasks{ 0 };
		Mutex m_taskMutex;
		ConditionVariable m_taskCv;
		Atomic<int32> m_successCount{ 0 };

		void AddTaskCount();
		void CompleteTaskCount();
		void WaitAllTasks();
	};
}