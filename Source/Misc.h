#pragma once

#include <windows.h>
#if defined(_DEBUG)
#include <crtdbg.h>

#define _ASSERT_EXPR_A(expr, msg) \
	(void)((!!(expr)) || \
	(1 != _CrtDbgReport(_CRT_ASSERT, __FILE__, __LINE__, NULL, "%s", msg)) || \
	(_CrtDbgBreak(), 0))
#else
#define  _ASSERT_EXPR_A(expr, expr_str) ((void)0)
#endif

inline LPWSTR HRTrace(HRESULT hr)
{
	LPWSTR msg;
	FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPWSTR>(&msg), 0, NULL);
	return msg;
}

class Benchmark
{
	LARGE_INTEGER ticksPerSecond;
	LARGE_INTEGER startTicks;
	LARGE_INTEGER currentTicks;

public:
	Benchmark()
	{
		QueryPerformanceFrequency(&ticksPerSecond);
		QueryPerformanceCounter(&startTicks);
		QueryPerformanceCounter(&currentTicks);
	}
	~Benchmark() = default;
	Benchmark(const Benchmark&) = delete;
	Benchmark& operator=(const Benchmark&) = delete;
	Benchmark(Benchmark&&) noexcept = delete;
	Benchmark& operator=(Benchmark&&) noexcept = delete;

	void begin()
	{
		QueryPerformanceCounter(&startTicks);
	}
	float end()
	{
		QueryPerformanceCounter(&currentTicks);
		return static_cast<float>(currentTicks.QuadPart - startTicks.QuadPart) / static_cast<float>(ticksPerSecond.QuadPart);
	}
};
