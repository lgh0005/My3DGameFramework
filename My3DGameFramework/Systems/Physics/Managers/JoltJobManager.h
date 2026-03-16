#pragma once

namespace MGF3D
{
	class JoltJobManager
	{
		MGF_DECLARE_SINGLE(JoltJobManager)

	private:
		JoltJobManager();
		~JoltJobManager();

	public:
		bool Init();
		void Shutdown();
		Ptr<JPH::JobSystem> GetJobSystem() const;

	private:
		JobSystemThreadPoolUPtr m_jobSystem;
	};
}