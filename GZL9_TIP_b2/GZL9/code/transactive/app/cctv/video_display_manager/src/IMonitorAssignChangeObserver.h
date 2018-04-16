#pragma once

namespace TA_IRS_App
{
	class Monitor;
	class IMonitorAssignChangeObserver
	{
	public:
	    virtual void monitorAssignChanged(Monitor* monitor) = 0;
	};
}
