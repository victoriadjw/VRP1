/**
*		usage: 1. provide class Timer  which define a specific date and time.
*
*
*/

#ifndef UTILITY_H
#define UTILITY_H

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
#include<algorithm>
#include<math.h>

using namespace std;

#define PI  3.1415926
#define EARTH_RADIUS  6378.137// earth radius
#define BUFF_SIZE 1000

class Timer
{
	friend std::ostream& operator<<(std::ostream &os, const Timer& timer)
	{
		time_t now_c = Clock::to_time_t(timer.currentTime);
		tm* now_tm = localtime(&now_c);
		os << std::put_time(now_tm, "%c");
		return os;
	}
	friend std::istream& operator>>(std::istream &is, Timer& timer)
	{
		timer.currentTime = Clock::now();
		return is;
	}
public:
	typedef std::chrono::system_clock Clock;
	typedef std::chrono::hours Duration;
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
		: currentTime(startTime + duration)
	{
	}

	bool isTimeOut() const
	{
		return (Clock::now() >= currentTime);
	}
	bool isTimeOut(const TimePoint &cmpTimePoint)const
	{
		return (cmpTimePoint >= currentTime);
	}
	Duration restTime() const
	{
		return std::chrono::duration_cast<Duration>(
			currentTime - Clock::now());
	}
	Duration restTime(const TimePoint &cmpTimePoint) const
	{
		return std::chrono::duration_cast<Duration>(
			currentTime - cmpTimePoint);
	}
	static string getCurrentTime()
	{
		char timeBuf[64];
		time_t t = time(NULL);
		tm *date = localtime(&t);
		strftime(timeBuf, 64, "%Y-%m-%d %a %H:%M:%S", date);
		return std::string(timeBuf);
	}
	TimePoint getCurrentTimePoint()const{ return currentTime; }
private:
	TimePoint currentTime;	// current time
};



static double rad(const double &d)
{
	return (d * PI / 180.0);
}

static double calculateDistance(const double &lat1, const double &lng1, const double &lat2, const double &lng2)
{
	double radLat1 = rad(lat1);
	double radLat2 = rad(lat2);
	double a = radLat1 - radLat2;
	double b = rad(lng1) - rad(lng2);
	double s = 2 * asin(sqrt(pow(sin(a / 2), 2) +
		cos(radLat1)*cos(radLat2)*pow(sin(b / 2), 2)));
	//return (round(s * EARTH_RADIUS * 10000) / 10000);
	return ((s * EARTH_RADIUS * 10000) / 10000);
}

// split a string according to a specific pattern
static std::vector<std::string> split(std::string str, std::string pattern)
{
	std::string::size_type pos;
	std::vector<std::string> result;
	str += pattern;// extend the string
	int size = str.size();

	for (int i = 0; i<size; i++)
	{
		pos = str.find(pattern, i);
		if (pos<size)
		{
			std::string s = str.substr(i, pos - i);
			result.push_back(s);
			i = pos + pattern.size() - 1;
		}
	}
	return result;
}

template<typename T>
class MyVector : vector<T>
{
public:
	MyVector(int size):vector<T>(size){	}

	MyVector(int size, const T &value) :vector<T>(size, value){}


	// TODO[0]: iii
	int find(const T &value)const{
		return (std::find(this->begin(), this->end, value) - this->begin());
	}

private:
};
#endif