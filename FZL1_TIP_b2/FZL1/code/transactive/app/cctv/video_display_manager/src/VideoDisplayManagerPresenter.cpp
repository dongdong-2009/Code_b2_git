#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#pragma warning( disable : 4503 )
#endif

#include "StdAfx.h"
#include "core/utilities/src/RunParams.h"
#include "core/data_access_interface/entity_access/src/ConsoleAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IConsole.h"
#include "core/data_access_interface/src/ILocation.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/data_access_interface/entity_access/src/DisplayManagerEntityData.h"
#include "core/exceptions/src/GenericGUIException.h"
#include "core/data_access_interface/entity_access/src/Sequence.h"
#include "core/data_access_interface/entity_access/src/VideoSwitchAgent.h"
#include "core/exceptions/src/TransactiveException.h"
#include "app/cctv/video_display_manager/src/VideoDisplayManagerPresenter.h"
#include "app/cctv/video_display_manager/src/CCTVCommon.h"
#include "app/cctv/video_display_manager/src/SequenceEditorDlg.h"
#include "app/cctv/video_display_manager/src/videoDisplayManager.h"
#include "app/cctv/video_display_manager/src/PlayBackSetDlg.h"
#include "app/cctv/video_display_manager/src/SDKCommunicator.h"
#include "app/cctv/video_display_manager/src/PTZButton.h"
#include "app/cctv/video_display_manager/src/VirtualMonitorAssigner.h"
#include <string>
#include "ViTriggeringMap.h"
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
#include "core/synchronisation/src/ThreadGuard.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::TransactiveException;
using TA_Base_Core::RunParams;
using TA_Base_Core::DebugUtil;
using std::vector;
namespace TA_IRS_App
{
    bool compareDisplayOrder(Monitor* m1, Monitor* m2)
    {
		return m1->getDisplayOrder() < m2->getDisplayOrder();
    }

    VideoDisplayManagerPresenter:: VideoDisplayManagerPresenter(IVideoDisplayManagerDAI* videoManagerDai,
                                                                ILoadStationDiagramService* loadStationDiagramService)
        : m_currentSelectSequenceId(0),
          m_seqEditorView(NULL),
          m_localMonitor(NULL),
		  m_virtualMonitorAssigner(NULL),
		  m_sdkService(NULL),
          m_checkUpdateSequenceTreeCtrl(NULL)
    {
        m_videoDisplayDai = videoManagerDai;
        m_locationType = TA_Base_Core::ILocation::OCC;
        for(int times = 0; times < 3; ++times)
        {
            std::string sessionID = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
            m_location = m_videoDisplayDai->getLocation(sessionID);
            if(m_location == NULL)
            {
                LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "VideoDisplayManagerPresenter::VideoDisplayManagerPresenter getLocation return NULL");
                if(times == 2)
                {
                    CString message;
                    message.FormatMessage(IDS_UE010016_CANNOT_LOAD_DATA, TA_Base_Core::DisplayManagerEntityData::getStaticType().c_str(), "Entity");
                    TA_THROW(TA_Base_Core::GenericGUIException(std::string(message), TA_Base_Core::GenericGUIException::ENTITY_CONFIGURATION_INVALID));
                }
                else
                {
                    Sleep(1000);
                    continue;
                }
            }
            else
            {
                break;
            }
        }
        m_locationType = m_location ->getLocationType();
        m_locationId = m_location->getKey();
        m_LoadStationDiagramService = loadStationDiagramService;
        m_hightedMonitorWnd = NULL;
        m_prvHilitedMonitorWnd = NULL;
        m_localMonitorKey = 0l;
        m_mouseTracking = false;
        m_contextActive = false;
        m_isAssignedBySchematic = false;
        m_currentSelectedMonitor = NULL;
        m_sdkService = & (SDKCommunicator::getInstance());
    }

    VideoDisplayManagerPresenter::~VideoDisplayManagerPresenter(void)
    {
		m_sdkService->removeInstance();
    }

    TA_Base_Core::ILocation::ELocationType  VideoDisplayManagerPresenter::getLocationType()
    {
        return m_locationType;
    }

    void VideoDisplayManagerPresenter::SetIView(IGFVideoSwitchDisplayView * view)
    {
        m_IView = view;
		//m_sdkService->SetIView(view);
    }

    void VideoDisplayManagerPresenter::SetSeqEditorView(ISequenceEditorView* seqEditorview)
    {
        m_seqEditorView = seqEditorview;
    }

    void VideoDisplayManagerPresenter::setLocationType(TA_Base_Core::ILocation::ELocationType locationType)
    {
        m_locationType = locationType;
    }

    unsigned long VideoDisplayManagerPresenter::getLocationId()
    {
        return m_locationId;
    }

    BOOL VideoDisplayManagerPresenter::isOcc()
    {
        return m_locationType == TA_Base_Core::ILocation::OCC;
    }

    void VideoDisplayManagerPresenter::closeSchematic()
    {
        if(m_LoadStationDiagramService != NULL)
        {
            m_LoadStationDiagramService->closeSchematic();
            delete m_LoadStationDiagramService;
        }
    }

    void VideoDisplayManagerPresenter::stopSchematic()
    {
        if(m_LoadStationDiagramService != NULL)
        {
            m_LoadStationDiagramService->stopSchematic();
        }
    }

    void VideoDisplayManagerPresenter::refreshSchematic()
    {
        if(m_LoadStationDiagramService != NULL)
        {
            m_LoadStationDiagramService->refreshSchematic();
        }
    }

    void VideoDisplayManagerPresenter::loadStationDiagram(unsigned long level)
    {
#ifndef LOCAL_TEST
        unsigned long stationId = m_IView->GetStationIdForGwax();
        if(0 < stationId)
        {
            std::string fullPath;
            DisplayItemManager::getInstance().getGraphicfullPath(stationId, fullPath, level);
            if(!fullPath.empty())
            {
		        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Begin load schematic");
                CRect rect ;
                m_IView->GetSchematicCRET(rect);
                HWND hwnd = m_IView->GetWndHandle();
                m_LoadStationDiagramService->stopSchematic();
                m_LoadStationDiagramService->loadStationDiagram(fullPath , rect , hwnd);
		        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "End load schematic");
            }
        }
#endif
    }

    void VideoDisplayManagerPresenter::editSequence(unsigned long seqId)
    {
        if(seqId > 0)
        {
            unsigned long stationId = m_IView->GetSelectedStation();
            Sequence* sequence = DisplayItemManager::getInstance().getSequence(seqId);
            if(!sequence->getReadOnly())
            {
                std::auto_ptr<SequenceEditorDlg> seqEditorView(new SequenceEditorDlg());
                SetSeqEditorView(seqEditorView.get());
                seqEditorView->SetPresenter(this);
                if(sequence != NULL)
                {
                    seqEditorView->Editsequence(sequence, stationId);
                }
                SetSeqEditorView(NULL);
            }
        }
    }

    void VideoDisplayManagerPresenter :: SelectStation()
    {
        unsigned long stationId = m_IView->GetSelectedStation();
        if(0 < stationId)
        {
            std::vector<VideoInput*> seqs;
            DisplayItemManager::getInstance().getSequenceByLocation(stationId , seqs);
            m_IView->DisplaySequences(seqs);
            m_IView->EnableEditSeqButton(false);
        }
    }

    void VideoDisplayManagerPresenter::SelectSequence(unsigned long sequenceId)
    {
        Sequence* seq = DisplayItemManager::getInstance().getSequence(sequenceId);
		if(seq != NULL)
		{
			m_currentSelectSequenceId = sequenceId;
			m_IView->DisplaySequence(seq);
			if(seq->getReadOnly())
			{
				m_IView->EnableEditSeqButton(false);
			}
			else
			{
				BOOST_FOREACH(MonitorDisplayItem* i, m_monitorDisplayItems)
	            {
	                if(i->inputIsSequence() && i->getVideoInput()->getKey() == sequenceId)
	                {
	                    m_IView->EnableEditSeqButton(false);
						return;
	                }
	            }
                m_IView->EnableEditSeqButton(true);
			}
		}
    }

    bool VideoDisplayManagerPresenter::assignQuad( Monitor* monitor,
								                   Camera* topLeftCam,
								                   Camera* topRightCam,
								                   Camera* bottomLeftCam,
								                   Camera* bottomRightCam )
	{
		TA_ASSERT(monitor->isVirtualQuadMonitor(), "");
		bool success = false;
		try
		{
			m_sdkService->assignQuad(monitor, topLeftCam, topRightCam, bottomLeftCam, bottomRightCam);
			success = true;
		}
        catch(TA_Base_Core::TransactiveException& te)
        {
            this->handleTransactiveException(te);
            LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::TransactiveException", te.what());
        }
		return success;
	}

	bool VideoDisplayManagerPresenter::playBackCtrl(Monitor* monitor, TA_Base_Bus::VideoOutputCorbaDef::PlayBackCtrlCmd cmd)
	{
		TA_ASSERT(monitor != NULL && monitor == m_localMonitor, "");
		bool success = false;
		try
		{
			m_sdkService->playBackCtrl(monitor, cmd);
			success = true;
		}
        catch(TA_Base_Core::TransactiveException& te)
        {
            this->handleTransactiveException(te);
            LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::TransactiveException", te.what());
        }
		return success;
	}

	bool VideoDisplayManagerPresenter::impPlayBack(Monitor* monitor, Camera* camera, PlayBackTimePair timePair)
	{
		TA_ASSERT(monitor != NULL && monitor == m_localMonitor, "");
		bool success = false;
		try
		{
			m_sdkService->requestRecordingPlayBack(monitor, camera, timePair);
			success = true;
		}
        catch(TA_Base_Core::TransactiveException& te)
        {
            this->handleTransactiveException(te);
            LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::TransactiveException", te.what());
        }
		return success;
	}

	bool VideoDisplayManagerPresenter::playBack(Monitor* monitor, Camera* camera, PlayBackTimePair timePair)
	{
		TA_ASSERT(monitor != NULL && monitor == m_localMonitor, "");
		return m_virtualMonitorAssigner->playBack(monitor, camera, timePair);
	}

	bool VideoDisplayManagerPresenter::impAssignMonitor(Monitor* monitor, VideoInput* input)
	{
		bool success = false;
        if(NULL != monitor)
        {
            try
            {
				success = m_sdkService->assignMonitor(monitor, input);
				if (success == false)
				{
					m_IView->displayErrorMessage(CString(m_sdkService->m_errorMessage.c_str()));
				}
            }
            catch(TA_Base_Core::TransactiveException& te)
            {
                this->handleTransactiveException(te);
                LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::TransactiveException", te.what());
            }
        }
        return success;
	}

    bool VideoDisplayManagerPresenter::assignMonitor(Monitor * monitor, VideoInput * input)
    {
        bool success = false;
		if(isVirtualMonitor(monitor))
		{
			success = m_virtualMonitorAssigner->assignMonitor(monitor, input);
		}
		else
		{
			success = impAssignMonitor(monitor, input);
			if(success)
			{
                std::string monitorDescription = (monitor->isSingleWallMonitor() ? DESC_SINGLE_WALL_MONITOR : DESC_SPLIT_WALL_MONITOR);
                DisplayItemManager::getInstance().saveEntityRelationship(monitor->getKey(), input->getKey(), monitorDescription);
				monitor->assignInput(input);
				monitorAssignChanged(monitor);
			}
		}
		return success;
    }

	bool VideoDisplayManagerPresenter::setMonitorPicCount(Monitor* monitor, int picCount)
	{
		bool success = false;
		if(NULL != monitor)
		{
			try
			{
				success = m_sdkService->setMonitorPicCount(monitor, picCount);
				if (success == false)
				{
					m_IView->displayErrorMessage(CString(m_sdkService->m_errorMessage.c_str()));
				}
				else
				{
                    VideoInput* input = monitor->getInput();
                    std::string monitorDescription = (monitor->isSingleWallMonitor() ? DESC_SINGLE_WALL_MONITOR : DESC_SPLIT_WALL_MONITOR);
                    DisplayItemManager::getInstance().saveEntityRelationship(monitor->getKey(), (NULL == input ? 0 : input->getKey()), monitorDescription);
				}
			}
			catch(TA_Base_Core::TransactiveException& te)
			{
				this->handleTransactiveException(te);
				LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::TransactiveException", te.what());
			}
		}
		return success;
	}

    void VideoDisplayManagerPresenter::saveSequenceConfig(Sequence* sequence, unsigned short dwellTime, const std::list< Camera* > cameraInputs)
    {
		bool success = m_sdkService->setSequenceConfig(sequence, dwellTime, cameraInputs);
		if(success == false)
		{
			m_IView->displayErrorMessage(CString(m_sdkService->m_errorMessage.c_str()));
			return;
		}
        sequence->setDwellTime(dwellTime);
        sequence->setCameras(cameraInputs);
        DisplayItemManager::getInstance().saveSequencesRelationship(sequence->getKey());
        DisplayItemManager::getInstance().updateSequence(sequence);
        DisplayItemManager::getInstance().notifyGUIofDisplayItemUpdate(sequence);
        m_IView->DisplaySequence(sequence);
    }

    void VideoDisplayManagerPresenter::setUpMonitor()
    {
        FUNCTION_ENTRY("setUpMonitor");
        std::string sessionID = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
        std::auto_ptr<TA_Base_Core::IConsole> console(TA_Base_Core::ConsoleAccessFactory::getInstance().getConsoleFromSession(sessionID));
        unsigned long localConsoleKey = console->getKey();
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "ConsoleKey =  %ld ", localConsoleKey);
        int maxMonitor = 0, countOcc = 0, countStation = 0;
        if(isOcc())
        {
            maxMonitor = MAX_OCC_MONITORS;
        }
        else
        {
            maxMonitor = MAX_STATION_MONITORS;
        }
		NameToMonitorMap allMonitors = DisplayItemManager::getInstance().getAllMonitors();
		std::vector<Monitor*> virtualQuadMonitors;
		BOOST_FOREACH(NameToMonitorMap::value_type& i, allMonitors)
        {
            Monitor* monitor = i.second;
            MonitorDisplayItem* item = NULL;
            int displayOrder = monitor->getDisplayOrder();
            unsigned long key = monitor->getKey();
            unsigned long consoleKey = monitor->getConsoleKey();
            if(localConsoleKey == consoleKey && displayOrder == 0)
            {
                if(0 == countOcc)
                {
                    // Display order of console monitor is 0
                    item = m_IView->getMonitorDisplayitem(0 , monitor);
                    m_localMonitorKey = key;
                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Set LocalMonitor.pkey = %lu ", key);
                    countOcc++;
                    m_localMonitor = monitor;
                }
            }
            else
            {
                if(countStation < maxMonitor - 1  && displayOrder > 0 && displayOrder < maxMonitor)
                {
                    item = m_IView->getMonitorDisplayitem(displayOrder , monitor);
                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Set Monitor[%d].pkey = %lu ", displayOrder, key);
                    countStation++;
                }
            }
            if(item != NULL)
            {
                m_monitorDisplayItems.push_back(item);
				if(item->getMonitor()->isVirtualQuadMonitor())
				{
					virtualQuadMonitors.push_back(item->getMonitor());
					std::sort(virtualQuadMonitors.begin(), virtualQuadMonitors.end(), compareDisplayOrder);
				}
            }
        }
		m_virtualMonitorAssigner = new VirtualMonitorAssigner(*this, virtualQuadMonitors, m_localMonitor);
        FUNCTION_EXIT;
    }

    void VideoDisplayManagerPresenter::setSequenceDescription(Sequence* sequence,
                                                              const std::string& newDescription)
    {
        m_sdkService->setSequenceDescription(sequence , newDescription);
        //sequence->setDescription(newDescription);
        sequence->setSequenceDescription(newDescription);
        DisplayItemManager::getInstance().updateSequence(sequence);
        DisplayItemManager::getInstance().saveSequencesRelationship(sequence->getKey());
        //Sequence* seq =  DisplayItemManager::getInstance().getSequence(sequence->getKey());
        m_IView->DisplaySequence(sequence);
        m_IView->UpdateSequenceTreeCtrl(sequence);
    }

    void VideoDisplayManagerPresenter::MainUILButtonDown(CPoint& point)
    {
        CPoint nullPt(0, 0);
        CPoint negPt(-1, -1);
        if(point == nullPt)
        {
            m_mouseTracking = true;
            m_IView->beginDragImage();
        }
        else
        {
            if(point == negPt)
            {
                if((m_hightedMonitorWnd != NULL) && (point == negPt))
                {
                    m_prvHilitedMonitorWnd = m_hightedMonitorWnd;
                    m_hightedMonitorWnd = NULL;
                    updateMonitorView();
                }
                return ;
            }
            MonitorDisplayItem* item = getOverMonitor(point);
            if(item != NULL)
            {
                m_prvHilitedMonitorWnd = m_hightedMonitorWnd;
                m_hightedMonitorWnd = item->getMonitorWnd();
                updateMonitorView();
            }
        }
    }

    void VideoDisplayManagerPresenter::updateMonitorView()
    {
        invalidateControlLabels();
        if(m_prvHilitedMonitorWnd != NULL)
        {
            m_IView->paintStandControl(m_prvHilitedMonitorWnd);
        }
        if(m_hightedMonitorWnd != NULL)
        {
            m_IView->paintHightedControl(m_hightedMonitorWnd);
        }
    }

    MonitorDisplayItem* VideoDisplayManagerPresenter::getOverMonitor(CPoint& point)
    {
		BOOST_FOREACH(MonitorDisplayItem* i, m_monitorDisplayItems)
		{
            if(!i->getMonitorWnd()->m_wnd->IsWindowVisible())
            {
                continue;
            }
			if(i->isInRect(point))
			{
				return i;
			}
		}
		return NULL;
    }

    MonitorDisplayItem* VideoDisplayManagerPresenter::getMonitor(unsigned long monitorId)
    {
		BOOST_FOREACH(MonitorDisplayItem* i, m_monitorDisplayItems)
		{
			if(i->getMonitor()->getKey() == monitorId)
			{
				return i;
			}
		}
		return NULL;
    }

    void VideoDisplayManagerPresenter::MainUILButtonUp(CPoint& point)
    {
        if(TRUE == m_mouseTracking)
        {
            m_IView->endDrage();
            m_mouseTracking = FALSE;
            m_contextActive = FALSE;
            MonitorDisplayItem *monItem = getOverMonitor(point);
            if(monItem != NULL)
            {
                VideoInput* input = m_IView->getSelectedVideoSource();
                if(NULL != input)
                {
                    Sequence* seq = dynamic_cast<Sequence*>(input);
					Monitor* monitor = monItem->getMonitor();
					if(NULL != seq)
					{
	                    if(!seq->getReadOnly())
	                    {
	                        std::list< Camera* > cameraList = seq->getCameras();
	                        if(cameraList.empty())
	                        {
	                            m_IView->displayErrorMessage(CString("请确认该序列已分配了摄像机"));
	                            return;
	                        }
	                    }/*
						if(monitor->isVirtualQuadMonitor())
						{
	                        m_IView->displayErrorMessage(CString("不能分配序列至四分屏"));
	                        return;
						}*/
					}
					assignMonitor(monitor, input);
                }
            }
        }
        /*
        else
        {
            MonitorDisplayItem *monItem = getOverMonitor(point);
            if(NULL != monItem)
            {
                handleHighLightMonitor(monItem);
            }
        }
        */
    }

    void VideoDisplayManagerPresenter::handleHighLightMonitor(MonitorDisplayItem * item)
    {
        m_currentSelectedMonitor = item;
        std::vector < MonitorDisplayItem *>::iterator it = m_monitorDisplayItems.begin() ;
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                    "item->inputIsCamera() = %d ,pkey=%ld,address:%s,supporeMovement:%d",
                    item->inputIsCamera(), NULL != item->getCamera() ? item->getCamera()->getKey() : -1,
                    NULL != item->getCamera() ? item->getCamera()->getAddress().c_str() : "--",
                    NULL != item->getCamera() ? item->getCamera()->getIsMovementSupported() : -1);
        if(item->inputIsCamera()
			&& NULL != item->getCamera()
			&& item->getCamera()->getIsMovementSupported()
			&& !item->getMonitor()->isVirtualQuadMonitor())
        {
			CPTZButton::setActiveMonitor(item->getMonitor());
            m_IView->enablePTZButton(TRUE);
        }
        else
        {
            m_IView->enablePTZButton(FALSE);
			CPTZButton::setActiveMonitor(NULL);
        }
        if(item->getIndexLable() != NULL)
        {
            item->getIndexLable()->Invalidate();
        }
        //m_IView->refreshIndexLable(item->getIndex(), item->isQuad());
        item->getTextLabel()->Invalidate();
        m_IView->paintHightedControl(item->getMonitorWnd());
    }


    void VideoDisplayManagerPresenter::handleTransactiveException(TA_Base_Core::TransactiveException& te)
    {
        m_IView->displayErrorMessage(CString(te.what()));
    }

    void VideoDisplayManagerPresenter::MainUIMouseMove(CPoint& point)
    {
        if(m_mouseTracking)
        {
            m_IView->setDragIcon();
            MonitorDisplayItem* item = getOverMonitor(point);
            MonitorWnd* hile = m_hightedMonitorWnd;
            MonitorWnd* prev = m_prvHilitedMonitorWnd;

            if(item  == NULL)
            {
                m_prvHilitedMonitorWnd = m_hightedMonitorWnd;
                m_hightedMonitorWnd = NULL;
            }
            else
            {
                m_prvHilitedMonitorWnd = m_hightedMonitorWnd;
                m_hightedMonitorWnd = item->getMonitorWnd();
            }
            if((hile != m_hightedMonitorWnd) || (prev != m_prvHilitedMonitorWnd))
            {
                m_contextActive = TRUE;
                m_IView->handleDragImageOnMouseMove(true , point);
                updateMonitorView();
            }
            else if(m_hightedMonitorWnd == NULL && m_prvHilitedMonitorWnd == NULL)
            {
                m_contextActive = TRUE;
                m_IView->handleDragImageOnMouseMove(true , point);
                updateMonitorView();
            }
            else
            {
                m_IView->handleDragImageOnMouseMove(false , point);
                m_contextActive = FALSE;
            }
        }
    }

    void VideoDisplayManagerPresenter::MainUIContextMenu(CPoint& point)
    {
		if(!m_contextActive && !m_mouseTracking)
		{
	        MonitorDisplayItem* item = getOverMonitor(point);
	        if(item != NULL)
	        {
		        bool isLocalMonitor = (m_localMonitorKey == item->getMonitor()->getKey());
		        updateMonitorView();
		        if(item->inputIsSequence())
		        {
		            m_IView->loadSequenceSubMenu(point , item->getMonitor(), item->getSequence() , isLocalMonitor);
		        }
		        else if(item->inputIsCamera())
		        {
		            m_IView->loadCameraSubMenu(point , item->getMonitor() , isLocalMonitor);
		        }
	        }
		}
    }

	bool VideoDisplayManagerPresenter::impClearMonitor(Monitor * monitor)
	{
        bool success = false;
        if(NULL != monitor)
        {
            try
            {
                m_sdkService->clearMonitor(monitor);
                success = true;
            }
            catch(TA_Base_Core::TransactiveException& te)
            {
                this->handleTransactiveException(te);
                LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::TransactiveException", te.what());
            }
            catch(...)
            {
            }
        }
        return success;
	}

	bool VideoDisplayManagerPresenter::impClearCameraFromMonitor(Monitor * monitor)
	{
		bool success = false;
		if(NULL != monitor)
		{
			try
			{
				success = m_sdkService->clearCameraFromMonitor(monitor);
				if (success == false)
				{
					m_IView->displayErrorMessage(CString(m_sdkService->m_errorMessage.c_str()));
				}
			}
			catch(TA_Base_Core::TransactiveException& te)
			{
				this->handleTransactiveException(te);
				LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::TransactiveException", te.what());
			}
			catch(...)
			{
			}
		}
		return success;
	}

	bool VideoDisplayManagerPresenter::impClearSequenceFromMonitor(Monitor * monitor)
	{
		bool success = false;
		if(NULL != monitor)
		{
			try
			{
				success = m_sdkService->clearSequenceFromMonitor(monitor);
				if (success == false)
				{
					m_IView->displayErrorMessage(CString(m_sdkService->m_errorMessage.c_str()));
				}
			}
			catch(TA_Base_Core::TransactiveException& te)
			{
				this->handleTransactiveException(te);
				LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::TransactiveException", te.what());
			}
			catch(...)
			{
			}
		}
		return success;
	}

    bool VideoDisplayManagerPresenter::clearMonitor(Monitor * monitor)
    {
		Sequence* sequence = dynamic_cast<Sequence*>(monitor->getInput());
        //start
        bool success = false;
		if(isVirtualMonitor(monitor))
		{
			success = m_virtualMonitorAssigner->assignMonitor(monitor, NULL);
		}
		else
		{
			success = impClearMonitor(monitor);
			if(success)
			{
				DisplayItemManager::getInstance().delEntityRelationship(monitor->getKey());
				monitor->assignInput(NULL);
				monitorAssignChanged(monitor);
			}
		}
		if(success && sequence != NULL && sequence->getKey() == m_currentSelectSequenceId)
		{
			SelectSequence(m_currentSelectSequenceId);
		}
		return success;
    }

	bool VideoDisplayManagerPresenter::clearCameraFromMonitor(Monitor * monitor)
	{
		//Sequence* sequence = dynamic_cast<Sequence*>(monitor->getInput());
		//start
		bool success = false;
		if(isVirtualMonitor(monitor))
		{
			success = m_virtualMonitorAssigner->assignMonitor(monitor, NULL);
		}
		else
		{
			success = impClearCameraFromMonitor(monitor);
			if(success)
			{
				DisplayItemManager::getInstance().delEntityRelationship(monitor->getKey());
				monitor->assignInput(NULL);
				monitorAssignChanged(monitor);
			}
		}
		/*if(success && sequence != NULL && sequence->getKey() == m_currentSelectSequenceId)
		{
			SelectSequence(m_currentSelectSequenceId);
		}*/
		return success;
	}

	bool VideoDisplayManagerPresenter::clearSequenceFromMonitor(Monitor * monitor)
	{
		Sequence* sequence = dynamic_cast<Sequence*>(monitor->getInput());
		//start
		bool success = false;
		if(isVirtualMonitor(monitor))
		{
			success = m_virtualMonitorAssigner->assignMonitor(monitor, NULL);
		}
		else
		{
			success = impClearSequenceFromMonitor(monitor);
			if(success)
			{
				DisplayItemManager::getInstance().delEntityRelationship(monitor->getKey());
				monitor->assignInput(NULL);
				monitorAssignChanged(monitor);
			}
		}
		if(success && sequence != NULL && sequence->getKey() == m_currentSelectSequenceId)
		{
			SelectSequence(m_currentSelectSequenceId);
		}
		return success;
	}

    MonitorDisplayItem * VideoDisplayManagerPresenter::getMonitorDisplayItemByIndex(unsigned short index)
    {
		MonitorDisplayItem* item = NULL;
		bool isQuad = false;
		unsigned short index_trim = index;
		if(index > QUAD_START_INDEX && index < QUAD_START_INDEX + INDEX_INTERVAL)
		{
			isQuad = true;
			index_trim = index - QUAD_START_INDEX;
		}
        if(index > SINGLEOPS_START_INDEX && index < SINGLEOPS_START_INDEX + INDEX_INTERVAL)
        {
            index_trim = index - SINGLEOPS_START_INDEX;
        }
		BOOST_FOREACH(MonitorDisplayItem* i, m_monitorDisplayItems)
		{
			if(i->getIndex() == index_trim)
			{
				if (isQuad == true)
				{	//if isQuad is true, must check quad also
					if(i->getMonitor()->isQuad() == true)
					{
						item = i;
						break;
					}
				}
				else
				{	//other case only need index match
					item = i;
					break;
				}
			}
		}
		return item;
    }

    //RPARAM_DISPLAYMONITOR==camera name;monitor index( 0-8)
    void VideoDisplayManagerPresenter::onRunParamChange(const std::string& name, const std::string& value)
    {
        FUNCTION_ENTRY("onRunParamChange");
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                    "onRunParamChange( name = %s, value = %s)",
                    name.c_str() , value.c_str());
        if(name == RPARAM_DISPLAYMONITOR)
        {
            std::vector<std::string> strs;
            boost::split(strs, value, boost::is_any_of(";"));
            if(strs.size() == 2)
            {
                std::string cameraAddress = strs[0];
                std::string monitorIndex = strs[1];
                unsigned short index = -1;
                try
                {
                    index = boost::lexical_cast<unsigned short> (monitorIndex);
                }
                catch(boost::bad_lexical_cast &)
                {
                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "wrong monitor index format %s ", monitorIndex.c_str());
                    return;
                }
                Camera* camera = DisplayItemManager::getInstance().getCameraByAddress(cameraAddress);
                if(camera != NULL)
                {
                    MonitorDisplayItem * item =  getMonitorDisplayItemByIndex(index);
                    if(NULL == item)
                    {
                        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
                                    "can not find the monitor  with the index %d ",
                                    index);
                        return;
                    }

                    if(index > SINGLEOPS_START_INDEX)
                    {
                        if(!item->getMonitor()->isSingleWallMonitor())
                        {
                            if(!setMonitorPicCount(item->getMonitor(), 1))
                            {
                                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
                                            "fail to set monitor pic count to 1, monitor is %lu", item->getMonitor()->getKey());
                                return;
                            }
                            m_IView->changeWallMontior(item, false);
                        }

                    }

                    if(!assignMonitor(item->getMonitor(), camera))
                    {
                        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
                                    "fail to assign camera(%s) to monitor %ld with index %d",
                                    cameraAddress.c_str() , item->getMonitor()->getKey(), item->getIndex());
                        return;
                    }
                    m_prvHilitedMonitorWnd = m_hightedMonitorWnd;
                    m_hightedMonitorWnd = item->getMonitorWnd();
                    updateMonitorView();
                    handleHighLightMonitor(item);
                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                                "onRunParamChange is done ");
                }
                else
                {
                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
                                "can not find the camera with the address %s ",
                                cameraAddress.c_str());
                }
            }
            else
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
                            "Incorect parameter number.(%d)", strs.size());
        }
		else if(name == RPARAM_TRIGGER_ENTITY)
		{
			unsigned long triggeringEntity = atol(value.c_str());
			try
			{
				ViTriggeringMap t(triggeringEntity);
				unsigned long cameraEntityKey = t.getCameraEntityKey();
				Camera* camera = DisplayItemManager::getInstance().getCamera(cameraEntityKey);
				if(NULL == camera)
				{
					LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Can't found the camera %lu", cameraEntityKey); 
					FUNCTION_EXIT;
					return;
				}
				// Get the monitor address if set
				std::string monitorId = "0";
				std::string paramMonitorId = RunParams::getInstance().get(RPARAM_MONITOR_ID);
				if(!paramMonitorId.empty())
				{
					monitorId = paramMonitorId;
				}
				std::string displayParamValue = camera->getAddress() + ";" + monitorId;
				RunParams::getInstance().set(RPARAM_DISPLAYMONITOR, displayParamValue.c_str());
			}
			catch (const TransactiveException& e)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", e.what());
			}
		}
		else if(name == RPARAM_CLEAR_MONITOR)
		{
			int monitorIndex = atoi(value.c_str());
            MonitorDisplayItem * item =  getMonitorDisplayItemByIndex(monitorIndex);
            if(item != NULL)
            {
                clearMonitor(item->getMonitor());
            }
        }
        else if(name == "DisplaySeq") //add for support test
        {
            std::vector<std::string> strs;
            boost::split(strs, value, boost::is_any_of(";"));
            if(strs.size() == 2)
            {
                std::string seqName = strs[0];
                unsigned short monindex = boost::lexical_cast<unsigned short> (strs[1]);
                MonitorDisplayItem* mon = getMonitorDisplayItemByIndex(monindex);
                Sequence* seq = DisplayItemManager::getInstance().getSequence(seqName);
                if(NULL != mon && NULL != seq)
                {
                    assignMonitor(mon->getMonitor(), seq);
                }
            }
        }
		else if(name == RPARAM_PLAYBACK)
		{
			PlayBackTimePair timePair;
			PlayBackSetDlg dlg(timePair);
			if(IDOK == dlg.DoModal())
			{
				Camera* camera = DisplayItemManager::getInstance().getCameraByAddress(value);
				if(NULL != camera)
				{
					playBack(m_localMonitor, camera, timePair);
				}
			}
		}
        else
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Unknown Param name");
        }
        FUNCTION_EXIT;
    }

    Camera * VideoDisplayManagerPresenter::getSelectedCamera()
    {
        return m_currentSelectedMonitor->getCamera();
    }

    BOOL VideoDisplayManagerPresenter::isCameraSelected()
    {
		return m_currentSelectedMonitor != NULL ? m_currentSelectedMonitor->inputIsCamera() : FALSE;
    }

    void VideoDisplayManagerPresenter::selectCameraFromSequenceList()
    {
    }

    void VideoDisplayManagerPresenter::beginDrage(CPoint & point , CImageList * image)
    {
        m_IView->setDragAdjust(point);
        m_IView->setDragImage(image);
    }

    void VideoDisplayManagerPresenter::onCompleteInitStation()
    {
        std::vector < TA_Base_Core::ILocation*> stations = DisplayItemManager::getInstance().getStations();
        if(!stations.empty())
		{
            m_IView->enableStationSelection(isOcc());
	        SelectStation();
            //m_IView->GetStationIdForGwax();
	        //loadStationDiagram();
	        loadMonitorRelationship();
		}
    }

    void VideoDisplayManagerPresenter::loadMonitorRelationship()
    {
        unsigned long pkey;
        int typekey;
        std::string description;
        BOOST_FOREACH(MonitorDisplayItem * i, m_monitorDisplayItems)
        {
            if(i != NULL)
            {
                try
                {
                    DisplayItemManager::getInstance().loadEntityRelationship(i->getMonitor()->getKey(), pkey, description, &typekey);
                }
                catch (...)
                {
                    continue;
                }
                

                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                            "load Monitor.Monitor key:[%ld],item key:[%ld].", i->getMonitor()->getKey(), pkey);

                // Wall monitor
                if(!i->isLocalMonitor() && !i->isQuad())
                {
                    i->getMonitor()->setSingleWallMonitor(DESC_SINGLE_WALL_MONITOR == description);
                    m_IView->changeWallMontior(i, !i->getMonitor()->isSingleWallMonitor());
                }
                
                if(pkey != 0)
                {
                    VideoInput* input = DisplayItemManager::getInstance().getInput(pkey);
                    if(input != NULL)
                    {
                        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Camera item key:(%ld) is not empty.", pkey);
                        if((i->getVideoInput() == NULL ||  i->getVideoInput()->getKey() != pkey) && pkey != m_localMonitorKey)
                        {
                            i->setVideoInput(input);				//monitorDisplayItems is for virtual label
    						i->getMonitor()->assignInput(input);		//This is used assigned to real monitor
                            m_IView->assignMonitor(i);
                            if(typekey == RL_MONITOR_CAMERA_HIGHLIGHT)
                            {
                                handleHighLightMonitor(i);
                            }
                        }
                    }
                }

            }
        }
    }

    void VideoDisplayManagerPresenter::CheckSequeceUpdate()
    {
        if(m_checkUpdateSequenceTreeCtrl == NULL)
        {
            return;
        }
        CString itemName, itemText, itemDes;
        HTREEITEM hitem;
        NameToSequenceMap SeqMap = DisplayItemManager::getInstance().getAllSequences();
        BOOST_FOREACH(NameToSequenceMap::value_type & i, SeqMap)
        {
            Sequence* seq = dynamic_cast<Sequence*>(i.second);
            itemName.Format("%s", seq->getName().c_str());
            hitem = m_checkUpdateSequenceTreeCtrl->findItemFromSubstr(itemName);
            if(hitem != NULL)
            {
                itemText = m_checkUpdateSequenceTreeCtrl->GetItemText(hitem);
                int pos = itemText.Find(" - ");
                if(pos >= 0)
                {
                    itemDes.Format("%s", seq->getSequenceDescription().c_str());
                    CString itemRight = itemText.Right(itemText.GetLength() - pos - 3);
                    if(itemRight != itemDes)
                    {
                        m_checkUpdateSequenceTreeCtrl->videoInputUpdated(seq);
                    }
                }
            }
        }
        m_checkUpdateSequenceTreeCtrl->handleSelectionChanged();
    }


    void VideoDisplayManagerPresenter::OnTimer(UINT nEventID)
    {
        switch(nEventID)
        {
            case HALF_MINUTE_EVENT:
                {
                    TA_Base_Core::ThreadGuard guard(m_condition.getLinkedThreadLockable());
                    m_condition.signal();
                }
                break;
            default:
                break;
        }
    }



    void VideoDisplayManagerPresenter::invalidateControlLabels()
    {
        FUNCTION_ENTRY("invalidateControlLabels");
        if(m_prvHilitedMonitorWnd != NULL)
        {
            MonitorDisplayItem* prvItem = getMonitorByCtrlId(m_prvHilitedMonitorWnd->m_wnd->GetDlgCtrlID());
            if(prvItem != NULL)
            {
                prvItem->getTextLabel()->Invalidate();
                if(prvItem->getIndexLable() != NULL)
                {
                    prvItem->getIndexLable()->Invalidate();
                }
                //m_IView->refreshIndexLable(prvItem->getIndex(), prvItem->isQuad());
            }
        }
        if(m_hightedMonitorWnd != NULL)
        {
            MonitorDisplayItem* hilitedItem = getMonitorByCtrlId(m_hightedMonitorWnd->m_wnd->GetDlgCtrlID());
            if(hilitedItem != NULL)
            {
                hilitedItem->getTextLabel()->Invalidate();
                if(hilitedItem->getIndexLable() != NULL)
                {
                    hilitedItem->getIndexLable()->Invalidate();
                }
                //m_IView->refreshIndexLable(hilitedItem->getIndex(), hilitedItem->isQuad());
            }
        }
        FUNCTION_EXIT;
    }

	bool VideoDisplayManagerPresenter::isLocalMonitor(Monitor* monitor)
	{
		return monitor == m_localMonitor;
	}

	void VideoDisplayManagerPresenter::monitorAssignChanged(Monitor* monitor)
	{
		if(NULL != monitor)
		{
			MonitorDisplayItem* monItem = getMonitor(monitor->getKey());
			if( NULL != monItem)
			{
				monItem->setVideoInput(monitor->getInput());
				m_IView->assignMonitor(monItem);
				Sequence* sequence = dynamic_cast<Sequence*>(monitor->getInput());
				Camera* camera = dynamic_cast<Camera*>(monitor->getInput());
				if(sequence != NULL && sequence->getKey() == m_currentSelectSequenceId)
				{
					SelectSequence(m_currentSelectSequenceId);
				}
				if(camera == NULL && CPTZButton::getActiveMonitor() == monitor)
				{
					m_IView->enablePTZButton(FALSE);
					CPTZButton::setActiveMonitor(NULL);
				}
			}
		}
	}

	bool VideoDisplayManagerPresenter::isVirtualMonitor(Monitor* monitor)
	{
		return false;		// there are no virtual monitor in FZL1
		//return (monitor->isVirtualQuadMonitor() || monitor == m_localMonitor);
	}

    void VideoDisplayManagerPresenter::run()
    {
        m_running = true;
        while(m_running)
        {
            {
                TA_Base_Core::ThreadGuard guard(m_condition.getLinkedThreadLockable());
                m_condition.wait();
            }
            if(m_running)
            {
                try
                {
                    loadMonitorRelationship();
                    DisplayItemManager::getInstance().LoadSequenceRelationFromDatabase();
                    CheckSequeceUpdate();
                }
                catch (...)
                {
                }
            }
        }
    }

    void VideoDisplayManagerPresenter::terminate()
    {
        m_running = false;
        m_condition.broadcast();
    }

    void VideoDisplayManagerPresenter::setUpdateSequenceTreeCtrl( VideoSourcesTreeCtrl* sequenceTreeCtrl )
    {
        m_checkUpdateSequenceTreeCtrl = sequenceTreeCtrl;
    }

    MonitorDisplayItem* VideoDisplayManagerPresenter::getMonitorByCtrlId( unsigned long monitorId )
    {
		BOOST_FOREACH(MonitorDisplayItem* i, m_monitorDisplayItems)
		{
			if(i->getMonitorWnd()->m_wnd->GetDlgCtrlID() == monitorId)
			{
				return i;
			}
		}
		return NULL;
    }

}
