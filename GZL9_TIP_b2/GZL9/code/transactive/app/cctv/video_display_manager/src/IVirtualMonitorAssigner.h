#pragma once
namespace TA_IRS_App
{
	class Monitor;
	class VideoInput;
	class IMonitorAssignChangeObserver;

    class IVirtualMonitorAssigner
    {
        public:
			virtual ~IVirtualMonitorAssigner() {}
            virtual bool assignMonitor(TA_IRS_App::Monitor* monitor, TA_IRS_App::VideoInput* input) = 0;
            virtual void setMonitorAssignChangedObserver(IMonitorAssignChangeObserver& assignChangedObserver) = 0;
    };
}