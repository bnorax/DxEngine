#include "FrameStats.h"

int DxEngine::FrameStat::GetFps()
{

	return prevFrames;
}

void DxEngine::FrameStat::StatBegin()
{
	beginFrame = GetTickCount64();
}

bool DxEngine::FrameStat::StatEnd()
{
	frameCounter++;
	prevFrameTime = GetTickCount64() - beginFrame;
	deltaTime += prevFrameTime;
	if (deltaTime >= 1000) {
		prevFrames = frameCounter;
		frameCounter = 0;
		deltaTime = 0;
		return true;
	}
	return false;
}
