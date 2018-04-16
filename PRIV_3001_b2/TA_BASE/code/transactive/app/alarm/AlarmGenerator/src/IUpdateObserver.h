
#ifndef __IUPDATE_OBSERVER_INCLUDED__
#define __IUPDATE_OBSERVER_INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class IUpdateObserver
{
public:
	virtual void UpdateToAlarmStore(TA_Base_Core::AlarmUpdateCorbaDef& alarmUpdate) = 0;
	virtual ~IUpdateObserver(){};
};


#endif //__IUPDATE_OBSERVER_INCLUDED__
