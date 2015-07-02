/**
*		usage: 1. provide class Timer  which define a specific date and time.
*
*
*/

#ifndef VRP1_H
#define VRP1_H

#include<iostream>
#include<fstream>
#include<map>
#include<string>
#include<sstream>
#include<cstdlib>
#include<ctime>
#include<chrono>
#include<ctime>
#include <iomanip>    

using namespace std;

class Timer
{
public:
	typedef std::chrono::system_clock Clock;
	typedef std::chrono::seconds Duration;
	typedef std::chrono::system_clock::time_point TimePoint;
	//std::chrono::system_clock::time_point
	Timer()
	{
		currentTime = std::chrono::system_clock::now();
	}
	//tm_sec	int	seconds after the minute	0 - 60 *
	//tm_min	int	minutes after the hour	0 - 59
	//tm_hour	int	hours since midnight	0 - 23
	//tm_mday	int	day of the month	1 - 31
	//tm_mon	int	months since January	0 - 11
	//tm_year	int	years since 1900
	//tm_wday	int	days since Sunday	0 - 6
	//tm_yday	int	days since January 1	0 - 365
	//tm_isdst	int	Daylight Saving Time flag
	Timer(int tm_sec,	//int	seconds after the minute	0 - 60 *
		int tm_min,		//int	minutes after the hour	0 - 59
		int tm_hour,	//int	hours since midnight	0 - 23
		int tm_mday = 1,	//int	day of the month	1 - 31
		int tm_mon = 7,		//int	months since January	0 - 11
		int tm_year = 2015	//int	years since 1900

		)
	{
		std::tm t;
		t.tm_year = tm_year - 1900, t.tm_mon = tm_mon-1, t.tm_mday = tm_mday;
		t.tm_hour = tm_hour, t.tm_min = tm_min, t.tm_sec = tm_sec;
		time_t tt = mktime(&t);
		currentTime = Clock::from_time_t(tt);
		cout << ctime(&tt) << "\t" <<
			std::chrono::duration_cast<std::chrono::hours>(currentTime.time_since_epoch()).count();

		TimePoint now_time = Clock::now(); // get current time
		time_t now_c = Clock::to_time_t(now_time);
		tm* now_tm = localtime(&now_c);
		cout << "\nnow : " << std::put_time(now_tm, "%c") << endl;
	}
	Timer(Duration duration, TimePoint startTime = Clock::now())
		: endTime(startTime + duration)
	{
	}

	bool isTimeOut() const
	{
		return (Clock::now() >= endTime);
	}
	bool isTimeOut(TimePoint curTime)
	{
		return (curTime>=endTime);
	}
	Duration restTime() const
	{
		return std::chrono::duration_cast<Duration>(
			currentTime - Clock::now());
	}

private:
	TimePoint currentTime;	// current time
	TimePoint endTime;
};
#endif