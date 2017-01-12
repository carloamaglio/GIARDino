/*
 * rtc.h - 
 * 
 * Released into the public domain
 */

#ifndef RTC_H
#define RTC_H

#include <arduino.h>
//#include <rtc.h>

class Rtc {
	public:
		Rtc();
		//Rtc* lcd;
		void loop();
		unsigned long getTime();	// second from midnight
		int getDate();
		void setTime(int hour, int minute, int second);
		void setDate(int year, int month, int day);
	private:
		int a;
};

#endif

