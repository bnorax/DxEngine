#include "Time.h"

float DxEngine::Time::GetTimeSinceAppStart()
{
	QueryPerformanceCounter(&now);
	return (now.QuadPart - appStartTick.QuadPart) / freq.QuadPart;
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
	prevFrameTime = (now.QuadPart - beginFrame.QuadPart) / freq.QuadPart;
	deltaTime += prevFrameTime;
	if (deltaTime >= 1000) {
		prevFrames = frameCounter;
		frameCounter = 0;
		deltaTime = 0;
	}
}