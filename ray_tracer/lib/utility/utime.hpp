#pragma once

#include <chrono>

namespace utl
{
	class stopwatch
	{
		using timepoint_t = std::chrono::steady_clock::time_point;
	public:
		stopwatch() noexcept
		{
			sp = std::chrono::high_resolution_clock::now();
		}

		template<class _Ty>
		_Ty stop()
		{
			auto now = std::chrono::high_resolution_clock::now();
			auto secs = std::chrono::duration<_Ty>(now - sp).count();
			sp = now;
			return secs;
		}
	private:
		timepoint_t sp;
	};
}