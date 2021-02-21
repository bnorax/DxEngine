#pragma once

class Time {
public:
	static Time& Instance() {
		static Time s;
		return s;
	}
	ULONGLONG GetTimeInMillisSinceAppStart() {

		return (GetTickCount64()- startTick);
	}
private:
	ULONGLONG startTick = GetTickCount64();
	Time() {};
	~Time() {};
	Time(Time const&) = delete;
	Time& operator= (Time const&) = delete;
};