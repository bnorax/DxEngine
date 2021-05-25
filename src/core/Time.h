#pragma once
namespace DxEngine {
	//all times in millies
	class Time {
	public:
	
		static Time& Instance() {
			static Time s;
			return s;
		}
		float GetTimeSinceAppStart();
		int GetFps();
		void FrameBegin();
		void FrameEnd();
		float deltaTime();
	private:
		LARGE_INTEGER appStartTick, beginFrame, now, freq;
		int frameCounter = 0, prevFrames = 0, prevFrameTime = 0;
		float pDeltaTime = 0;
		Time() {
			QueryPerformanceFrequency(&freq);
			freq.QuadPart /= 1000; //time in millis
			QueryPerformanceCounter(&appStartTick);
		};
		~Time() {};
		Time(Time const&) = delete;
		Time& operator= (Time const&) = delete;
	};
}