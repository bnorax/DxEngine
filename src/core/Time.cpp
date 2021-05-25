#include "Time.h"

float DxEngine::Time::GetTimeSinceAppStart()
{
	QueryPerformanceCounter(&now);
	return static_cast<float>(now.QuadPart - appStartTick.QuadPart) / freq.QuadPart;
}

int DxEngine::Time::GetFps()
{
	return prevFrames;
}

void DxEngine::Time::FrameBegin()
{
	QueryPerformanceCounter(&beginFrame);
}

void DxEngine::Time::FrameEnd()
{
	frameCounter++;
	QueryPerformanceCounter(&now);
	prevFrameTime = static_cast<int>((now.QuadPart - beginFrame.QuadPart) / freq.QuadPart);
	pDeltaTime += prevFrameTime;
	if (pDeltaTime >= 1000) {
		prevFrames = frameCounter;
		frameCounter = 0;
		pDeltaTime = 0;
	}
}

float DxEngine::Time::deltaTime()
{
	return  pDeltaTime /1000.0f;
}
