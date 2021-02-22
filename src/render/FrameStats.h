#pragma once
namespace DxEngine {
	class FrameStat {
	public:
		int GetFps();
		void StatBegin();
		bool StatEnd();
		ULONGLONG beginFrame = 0;
		int prevFrameTime = 0;
	private:
		UINT frameCounter = 0;
		UINT prevFrames = 0;
		UINT deltaTime = 0;
	};
}