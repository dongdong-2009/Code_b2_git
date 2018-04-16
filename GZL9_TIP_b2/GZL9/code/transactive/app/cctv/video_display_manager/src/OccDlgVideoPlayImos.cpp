#include "stdafx.h"
#include "OccDlgVideoPlayImos.h"
#include "DisplayItemManager.h"
#include "IMonitorAssignChangeObserver.h"
#include "core/utilities/src/DebugUtil.h"
//#include "app/cctv/video_switch_agent/protocols/coe/src/IMOSCommunicationHandler.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/utilities/src/RunParams.h"
#include "app/cctv/video_display_manager/src/Monitor.h"
#include "app/cctv/video_display_manager/src/Camera.h"
#include "app/cctv/video_display_manager/src/Sequence.h"
#include "app/cctv/video_switch_agent/protocols/imos_sdk_win/include/mw_ptzlib_pub.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/VideoSwitchAgent.h"

using TA_Base_Core::DebugUtil;

namespace TA_IRS_App
{
    IMPLEMENT_DYNAMIC(OccDlgVideoPlayImos, DlgVideoPlayImos)

    OccDlgVideoPlayImos::OccDlgVideoPlayImos(TA_IRS_App::Monitor* local,
	                                 CWnd* pParent)
        : DlgVideoPlayImos(pParent), m_localMonitor(local)
    {
    }

    OccDlgVideoPlayImos::~OccDlgVideoPlayImos()
    {
    }

    BEGIN_MESSAGE_MAP(OccDlgVideoPlayImos, DlgVideoPlayImos)
		ON_WM_CLOSE()
    END_MESSAGE_MAP()

	bool OccDlgVideoPlayImos::assignMonitor(TA_IRS_App::Monitor* monitor, TA_IRS_App::VideoInput* input)
	{
		TA_ASSERT(NULL != monitor, "Monitor should not be null")
		TA_ASSERT(monitor == m_localMonitor, "Monitor is not local monitor");
		bool success = assignVideoToLocal(input);
		CheckShowOrHideWindow();
		return success;
	}

    void OccDlgVideoPlayImos::CheckShowOrHideWindow()
	{
		TA_ASSERT(NULL != m_localMonitor, "Monitor should not be null");
		int showOrHideWindow = ((NULL == m_localMonitor->getInput()) ? SW_HIDE : SW_SHOW);
		ShowWindow(showOrHideWindow);
	}

	BOOL OccDlgVideoPlayImos::OnInitDialog()
	{
		DlgVideoPlayImos::OnInitDialog();
		return TRUE;
	}

	void OccDlgVideoPlayImos::OnClose()
	{
		ShowWindow(SW_HIDE);
		//assignMonitor(m_localMonitor, NULL);
	}

    bool OccDlgVideoPlayImos::assignVideoToLocal(TA_IRS_App::VideoInput* videoInput)
    {
        bool success = false;
        unsigned long errorCode = 0;
		TA_ASSERT(m_monitorAssignChangeOb != NULL, "m_monitorAssignChangeOb should not be null");
        do
        {
			// Occ only one local monitor, force change layout to single in case user change it
            if(false == setLayoutToSingle())
            {
                break;
            }
            TA_IRS_App::VideoInput* currentInput = m_localMonitor->getInput();
            if(currentInput == videoInput)
            {
                success = true;
                break;
            }
            // first clear current
            if(NULL != currentInput)
            {
                // check the input type
                TA_IRS_App::Camera* cam = dynamic_cast<TA_IRS_App::Camera*>(currentInput);
                TA_IRS_App::Sequence* seq = dynamic_cast<TA_IRS_App::Sequence*>(currentInput);
                if(NULL != cam)
                {
					LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "m_imosCtrl.IMOSAX_StopFrameLive(1)");
                    errorCode = IMOS_CALL(m_imosCtrl.IMOSAX_StopFrameLive(1));
                }
                else if(NULL != seq)
                {
					LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "m_imosCtrl.IMOSAX_StopFrameSwitch(1)");
                    errorCode = IMOS_CALL(m_imosCtrl.IMOSAX_StopFrameSwitch(1));
                }
                else
                {
                    TA_ASSERT(false, "Invalid input type");
                }
                LOG_ERRORCODE_ERROR(errorCode);
                if(ERR_COMMON_SUCCEED != errorCode)
                {
                    break;
                }
                m_localMonitor->assignInput(NULL);
				m_monitorAssignChangeOb->monitorAssignChanged(m_localMonitor);
				try
				{
					DisplayItemManager::getInstance().delEntityRelationship(m_localMonitor->getKey());
				}
				catch (...)
				{
				}
            }
            // assign new camera/sequence to local monitor
            if(NULL != videoInput)
            {
                TA_IRS_App::Camera* cam = dynamic_cast<TA_IRS_App::Camera*>(videoInput);
                TA_IRS_App::Sequence* seq = dynamic_cast<TA_IRS_App::Sequence*>(videoInput);
                if(NULL != cam)
                {
					LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Frame no:1, Camera address:%s", videoInput->getAddress().c_str());
                    errorCode = IMOS_CALL(m_imosCtrl.IMOSAX_StartFrameLive(1, videoInput->getAddress().c_str()));
                }
                else if(NULL != seq)
                {
					LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Frame no:1, Sequence address:%s", videoInput->getAddress().c_str());
                    errorCode = IMOS_CALL(m_imosCtrl.IMOSAX_StartFrameSwitch(1, videoInput->getAddress().c_str()));
                }
                else
                {
                    TA_ASSERT(false, "Invalid input type");
                }
                LOG_ERRORCODE_ERROR(errorCode);
                if(ERR_COMMON_SUCCEED != errorCode)
                {
                    break;
                }
                m_localMonitor->assignInput(videoInput);
				m_monitorAssignChangeOb->monitorAssignChanged(m_localMonitor);
				try
				{
					DisplayItemManager::getInstance().saveEntityRelationship(m_localMonitor->getKey(), videoInput->getKey());
				}
				catch (...)
				{
				}
            }
            success = true;
        }
        while(0);
        return success;
    }
}