#pragma once

enum ETimerStatus 
{
	ETimerStop,
	ETimerRunning,
	ETimerExpire
};

class ITimerHeartBeat 
{
	public:
		virtual void publishClockHeartBeat () = 0;
};

class ITimerExpire
{
	public:
		virtual void notifyTimerExpire (ETimerStatus timerStatus) = 0;
};

class ITimerMngr
{
	public:
		
		virtual void timerStart (int nMilliSec) = 0;
		virtual void timerStop () = 0;
		virtual ETimerStatus timerStatus () = 0;	
};

class ITimerHeartBeatMngr 
{
	public:

		virtual void subscribeForClockHeartBeat (ITimerHeartBeat * subscriber,
												 int nHeartBeatInterval = 1000) = 0;
		
		virtual void unsubscribeForClockHeartBeat (ITimerHeartBeat * subscriber) = 0;

};
