#pragma once
#include <chrono>
#include <functional>
#include <vector>
#include <glm/glm.hpp>
#define ERROR_MESSAGE(msg) MessageBoxA(NULL, msg, "Minecraft C++ Error", MB_OK | MB_ICONERROR)

namespace util
{
	namespace __internal
	{
		static auto gameClock = std::chrono::high_resolution_clock();
		static auto startTime = gameClock.now();
	}
	inline float getTime()
	{
		return std::chrono::duration<float>(__internal::gameClock.now() - __internal::startTime).count();
	}

	template <class F>
	struct event_handler;

	template <class R, class... A>
	struct event_handler<R(*)(A...)>
	{
		typedef std::function<R(A...)> callbackType;

		std::vector<callbackType> callbacks{};

		void assign(callbackType func)
		{
			callbacks.push_back(func);
		}

		void operator+=(callbackType func)
		{
			assign(func);
		}

		std::vector<R> call(A... args)
		{
			std::vector<R> outputs{};
			for (int i = 0; i < callbacks.size(); i++)
			{
				outputs.push_back(callbacks[i](args...));
			}
			return outputs;
		}
	};

	inline glm::vec2 getResolution()
	{
		int viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);
		return glm::vec2((float)viewport[2], (float)viewport[3]);
	}
}