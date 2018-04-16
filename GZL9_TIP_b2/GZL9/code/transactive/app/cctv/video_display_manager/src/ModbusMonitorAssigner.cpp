#include "app/cctv/video_display_manager/src/ModbusMonitorAssigner.h"
#include "app/cctv/video_display_manager/src/Monitor.h"
#include "app/cctv/video_display_manager/src/VideoInput.h"
#include "app/cctv/video_display_manager/src/SwitchAgentCommunicator.h"
#include "app/cctv/video_display_manager/src/IMonitorAssignChangeObserver.h"
#include "app/cctv/video_display_manager/src/ModbusVideoPlayerControl.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_App
{

	ModbusMonitorAssigner::ModbusMonitorAssigner(const AllMonitorsArray& monitors)
	{
		m_monitors = monitors;
	}

	ModbusMonitorAssigner::~ModbusMonitorAssigner()
	{

	}

	bool ModbusMonitorAssigner::assignMonitor( Monitor* monitor, VideoInput* input )
	{
        bool success = false;
		TA_ASSERT(m_monitorAssignChangeOb != NULL, "m_monitorAssignChangeOb should not be null");
		if(!monitor->isQuadMonitor())
        {
            success = assignVideoToSingle(monitor, input);
        }
        else
        {
			int displayOrder = monitor->getDisplayOrder();
			int i = 5*(displayOrder/5) + 1;

			Monitor* oTopLeft = m_monitors[i];
			Monitor* oTopRight = m_monitors[i+1];
			Monitor* oBotLeft = m_monitors[i+2];
			Monitor* oBotRight = m_monitors[i+3];

			VideoInput* iTopLeft = oTopLeft ? oTopLeft->getInput() : NULL;
			VideoInput* iTopRight = oTopRight ? oTopRight->getInput() : NULL;
			VideoInput* iBotLeft = oBotLeft ? oBotLeft->getInput() : NULL;
			VideoInput* iBotRight = oBotRight ? oBotRight->getInput() : NULL;

			if(monitor == oTopLeft)
			{
				iTopLeft = input;
			}
			else if(monitor == oTopRight)
			{
				iTopRight = input;
			}
			else if(monitor == oBotLeft)
			{
				iBotLeft = input;
			}
			else if(monitor == oBotRight)
			{
				iBotRight = input;
			}
			Monitor* singleMonitor = m_monitors[(monitor->getDisplayOrder() / 5) * 5];
			if(singleMonitor == NULL)
			{
				return success;
			}
            success = assignVideoToQuad(singleMonitor,
				oTopRight, oTopLeft, oBotRight, oBotLeft,
				iTopRight, iTopLeft, iBotRight, iBotLeft);
        }
		checkShorOrHideWindow(monitor);
        return success;
	}

	void ModbusMonitorAssigner::setMonitorAssignChangedObserver( IMonitorAssignChangeObserver& assignChangedObserver )
	{
		m_monitorAssignChangeOb = &assignChangedObserver;
	}

	bool ModbusMonitorAssigner::assignVideoToSingle(Monitor* monitor, VideoInput* videoInput )
	{
        bool success = false;
        do
        {
			if(videoInput == NULL)
			{
				try
				{
					SwitchAgentCommunicator::getInstance().clearMonitor(monitor);
				}
				catch (...)
				{
					break;
				}
				
			}
			else
			{
				try
				{
					SwitchAgentCommunicator::getInstance().assignMonitor(monitor, videoInput);
				}
				catch (...)
				{
					break;
				}
			}
            monitor->assignInput(videoInput);
			m_monitorAssignChangeOb->monitorAssignChanged(monitor);
			const int displayOrder = 5*(monitor->getDisplayOrder()/5) + 1;
			for(int i = displayOrder; i < displayOrder + 4; ++i)
			{
				Monitor* monitor = m_monitors[i];
				if(monitor != NULL)
				{
					monitor->assignInput(NULL);
				    m_monitorAssignChangeOb->monitorAssignChanged(monitor);
				}
			}
            success = true;
        }
        while(0);
        return success;
	}

	bool ModbusMonitorAssigner::assignVideoToQuad( Monitor* monitorSingle, Monitor* monitorTopRight, Monitor* monitorTopLeft, Monitor* monitorBotRight, Monitor* monitorBotLeft, VideoInput* videoInputTopRight, VideoInput* videoInputTopLeft, VideoInput* videoInputBotRight, VideoInput* videoInputBotLeft )
	{
		do 
		{
			try
			{

				SwitchAgentCommunicator::getInstance().assignMonitor(monitorSingle, videoInputTopRight, videoInputTopLeft, videoInputBotRight, videoInputBotLeft);

				if(monitorTopLeft) monitorTopLeft->assignInput(videoInputTopLeft);
				if(monitorTopRight) monitorTopRight->assignInput(videoInputTopRight);
				if(monitorBotLeft) monitorBotLeft->assignInput(videoInputBotLeft);
				if(monitorBotRight) monitorBotRight->assignInput(videoInputBotRight);

				m_monitorAssignChangeOb->monitorAssignChanged(monitorTopLeft);
				m_monitorAssignChangeOb->monitorAssignChanged(monitorTopRight);
				m_monitorAssignChangeOb->monitorAssignChanged(monitorBotLeft);
				m_monitorAssignChangeOb->monitorAssignChanged(monitorBotRight);

				monitorSingle->assignInput(NULL);
				m_monitorAssignChangeOb->monitorAssignChanged(monitorSingle);
				return true;
			}
			catch (...)
			{
			}
		} while (0);
		return false;
	}

	void ModbusMonitorAssigner::checkShorOrHideWindow(Monitor* changedMonitor)
	{
		if(changedMonitor->getDisplayOrder() > 4)
		{
			return;
		}
		for(int i = 0; i < 5; ++i)
		{
			if(m_monitors[i] && m_monitors[i]->getInput())
			{
				showPlayer();
				return;
			}
		}
		hidePlayer();
	}

}

