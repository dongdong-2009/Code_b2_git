#pragma once

#include "app/cctv/video_display_manager/src/IMonitorAssigner.h"
#include "app/cctv/video_display_manager/src/CCTVCommon.h"
#include <vector>
//#include <string>

namespace TA_IRS_App
{
    class Monitor;
    class VideoInput;
    class IMonitorAssignChangeObserver;

    class ModbusMonitorAssigner : public IMonitorAssigner
    {
        public:
			ModbusMonitorAssigner(const AllMonitorsArray& monitors);
            virtual ~ModbusMonitorAssigner();
            virtual bool assignMonitor(Monitor* monitor, VideoInput* input);
            virtual void setMonitorAssignChangedObserver(IMonitorAssignChangeObserver& assignChangedObserver);
        protected:
            bool assignVideoToSingle(Monitor* monitor, TA_IRS_App::VideoInput* videoInput);
            bool assignVideoToQuad(
				Monitor* monitorSingle,
				Monitor* monitorTopRight,
				Monitor* monitorTopLeft,
				Monitor* monitorBotRight,
				Monitor* monitorBotLeft,
				VideoInput* videoInputTopRight,
				VideoInput* videoInputTopLeft,
				VideoInput* videoInputBotRight,
				VideoInput* videoInputBotLeft);

			void checkShorOrHideWindow(Monitor* changedMonitor);

			IMonitorAssignChangeObserver* m_monitorAssignChangeOb;
			AllMonitorsArray m_monitors;
    };
}