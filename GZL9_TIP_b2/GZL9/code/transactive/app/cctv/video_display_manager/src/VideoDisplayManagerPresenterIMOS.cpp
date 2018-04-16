#include "StdAfx.h"
#include "VideoSourcesTreeCtrl.h"
#include "VideoDisplayManagerPresenterIMOS.h"
#include "IMonitorAssigner.h"
#include "Configuration.h"
#include "core/utilities/src/RunParams.h"
#include "core/data_access_interface/src/ILocation.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/data_access_interface/entity_access/src/DisplayManagerEntityData.h"
#include "core/exceptions/src/GenericGUIException.h"
#include "core/data_access_interface/entity_access/src/Sequence.h"
#include "core/data_access_interface/entity_access/src/VideoSwitchAgent.h"
#include "core/exceptions/src/TransactiveException.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include <string>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

namespace TA_IRS_App
{

VideoDisplayManagerPresenterIMOS::VideoDisplayManagerPresenterIMOS(ILoadStationDiagramService* loadStationDiagramService) :
        VideoDisplayManagerPresenter(loadStationDiagramService)
{

}

VideoDisplayManagerPresenterIMOS::~VideoDisplayManagerPresenterIMOS()
{

}

bool VideoDisplayManagerPresenterIMOS::assignMonitor(Monitor* monitor, VideoInput* input)
{
    bool success = false;
    if (NULL != monitor)
    {
        if (m_localMonitorKey == monitor->getKey() || monitor->isQuadMonitor())
        {
            success = m_monitorAssigner->assignMonitor(monitor, input);
            if (!success)
            {
                CString message;
                message.Format("显示控制服务错误");
                m_IView->displayErrorMessage(message);
            }
        }
        else // Normal monitor
        {
            try
            {
                m_agentService->assignMonitor(monitor, input);
                success = true;
            }
            catch (TA_Base_Bus_GZL9::VideoSwitchAgentException& e)
            {
                this->handleVideoSwitchException(e);
                LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::VideoSwitchException", e.what.in());
            }
            catch (TA_Base_Core::TransactiveException& te)
            {
                this->handleTransactiveException(te);
                LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::TransactiveException", te.what());
            }
            catch (...)
            {
            }
        }
    }
    return success;
}

bool VideoDisplayManagerPresenterIMOS::clearMonitor(Monitor * monitor)
{
    //start
    bool success = false;
    if (NULL != monitor)
    {
        if (m_localMonitorKey == monitor->getKey() || monitor->isQuadMonitor())
        {
            success = m_monitorAssigner->assignMonitor(monitor, NULL);
            if (!success)
            {
                CString message;
                message.Format("显示控制服务错误");
                m_IView->displayErrorMessage(message);
            }
        }
        else // Normal monitor
        {
            try
            {
                m_agentService->clearMonitor(monitor);
                success = true;
            }
            catch (TA_Base_Bus_GZL9::VideoSwitchAgentException& e)
            {
                this->handleVideoSwitchException(e);
                LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::VideoSwitchException", e.what.in());
            }
            catch (TA_Base_Core::TransactiveException& te)
            {
                this->handleTransactiveException(te);
                LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::TransactiveException", te.what());
            }
            catch (...)
            {
            }
        }
    }
    return success;
}

void VideoDisplayManagerPresenterIMOS::onCompleteInitStation()
{
	/*
    std::vector < TA_Base_Core::ILocation*> stations = DisplayItemManager::getInstance().getStations();
    if (stations.empty()) return;
	*/
	if(CCTVConfiguration::getinstance().getLocationType() != TA_Base_Core::ILocation::OCC)
    {
        m_IView->enableStationSelection(FALSE);
    }
    else
    {
        m_IView->enableStationSelection(TRUE);
    }
    SelectStation();
    loadStationDiagram();
    loadMonitorRelationship();
}

void VideoDisplayManagerPresenterIMOS::OnTimer(UINT nEventID, VideoSourcesTreeCtrl * sequenceTreeCtrl /*= NULL*/)
{
    switch (nEventID)
    {
        case HALF_MINUTE_EVENT:
            loadMonitorRelationship();
            DisplayItemManager::getInstance().LoadSequenceRelationFromDatabase();
            CheckSequeceUpdate(sequenceTreeCtrl);
            sequenceTreeCtrl->handleSelectionChanged();
            break;
        default:
            break;
    }
}

void VideoDisplayManagerPresenterIMOS::setUpMonitor()
{
    FUNCTION_ENTRY("setUpMonitor");
	TA_Base_Core::IConsole* console = CCTVConfiguration::getinstance().getConsole();
    unsigned long localConsoleKey = console->getKey();
    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "ConsoleKey =  %ld ", localConsoleKey);

    std::map<std::string, Monitor*> allMonitors = DisplayItemManager::getInstance().getAllMonitors();
    for (std::map<std::string, Monitor*>::iterator it = allMonitors.begin(); it != allMonitors.end(); ++it)
    {
        Monitor* mon = (*it).second;
        MonitorDisplayItem* item = NULL;
        int displayOrder = mon->getDisplayOrder();
        unsigned long key = mon->getKey();
        unsigned long consoleKey = mon->getConsoleKey();
        if (localConsoleKey == consoleKey && displayOrder == 0)
        {
            // Display order of console monitor is 0
            item = m_IView->getMonitorDisplayitem(0 , (*it).second);
            m_localMonitorKey = key;
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Set LocalMonitor.pkey = %lu ", key);
            m_localMonitor = mon;
        }
        else if (displayOrder > 0)
        {
            item = m_IView->getMonitorDisplayitem(displayOrder , (*it).second);
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Set Monitor[%d].pkey = %lu ", displayOrder, key);
        }
        if (item != NULL)
        {
            m_monitorDisplayItems.push_back(item);
        }
    }
    FUNCTION_EXIT;
}

}
