#pragma once

namespace MGF3D
{
	namespace ThisThread
	{
		inline void Yield() noexcept
		{
			std::this_thread::yield();
		}

		inline void SleepFor(unsigned int milliseconds) noexcept
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
		}

		inline Thread::id GetId() noexcept
		{
			return std::this_thread::get_id();
		}
	}
}