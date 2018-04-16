// DlgVideoPlay.cpp : implementation file
//

#include "stdafx.h"
#include "StationDlgVideoPlayImos.h"
#include "DisplayItemManager.h"
#include "IMonitorAssignChangeObserver.h"
#include "IPTZControlView.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/utilities/src/RunParams.h"
#include "app/cctv/video_display_manager/src/Monitor.h"
#include "app/cctv/video_display_manager/src/Camera.h"
#include "app/cctv/video_display_manager/src/Sequence.h"
#include "app/cctv/video_switch_agent/protocols/imos_sdk_win/include/mw_ptzlib_pub.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/VideoSwitchAgent.h"

using TA_Base_Core::DebugUtil;

namespace
{
/* global constant
const unsigned short G_PAN_LEFT = 1;
const unsigned short G_PAN_RIGHT = 2;

const unsigned short G_TILT_UP = 1;
const unsigned short G_TILT_DOWN = 2;

const unsigned short G_ZOOM_IN = 1;
const unsigned short G_ZOOM_OUT = 2;

const unsigned short G_PT_LEFTUP = 1;
const unsigned short G_PT_RIGHTUP = 2;
const unsigned short G_PT_LEFTDOWN = 3;
const unsigned short G_PT_RIGHTDOWN = 4;
*/
// Position of these values in array dependent on upon values defined in cctvcommon.h
const unsigned long panCommands[2][3] =
{
    {0, MW_PTZ_PANLEFT, MW_PTZ_PANRIGHT},
    {0, MW_PTZ_PANLEFTSTOP, MW_PTZ_PANRIGHTSTOP}
};
const unsigned long tiltCommands[2][3] =
{
    {0, MW_PTZ_TILTUP, MW_PTZ_TILTDOWN},
    {0, MW_PTZ_TILTUPSTOP, MW_PTZ_TILTDOWNSTOP}
};
const unsigned long zoomCommands[2][3] =
{
    {0, MW_PTZ_ZOOMTELE, MW_PTZ_ZOOMWIDE},
    {0, MW_PTZ_ZOOMTELESTOP, MW_PTZ_ZOOMWIDESTOP}
};
const unsigned long ptCommands[2][5] =
{
    {0, MW_PTZ_LEFTUP, MW_PTZ_RIGHTUP, MW_PTZ_LEFTDOWN, MW_PTZ_RIGHTDOWN},
    {0, MW_PTZ_LEFTUPSTOP, MW_PTZ_RIGHTUPSTOP, MW_PTZ_LEFTDOWNSTOP, MW_PTZ_RIGHTDOWNSTOP}
};

const unsigned long PTZ_SPEED = 5;
}

namespace TA_IRS_App
{
IMPLEMENT_DYNAMIC(StationDlgVideoPlayImos, DlgVideoPlayImos)

StationDlgVideoPlayImos::StationDlgVideoPlayImos(TA_IRS_App::Monitor* local,
                                                 const VirtualQuadArray& quad,
                                                 IPTZControlView* ptzCtrl,
                                                 CWnd* pParent)
        : DlgVideoPlayImos(pParent),
        m_localMonitor(local),
        m_isQuadMode(true),
        m_ptzCtrl(ptzCtrl)
{
    m_quad = quad;
}

StationDlgVideoPlayImos::~StationDlgVideoPlayImos()
{
}

BEGIN_MESSAGE_MAP(StationDlgVideoPlayImos, DlgVideoPlayImos)
    ON_WM_CLOSE()
END_MESSAGE_MAP()

bool StationDlgVideoPlayImos::assignMonitor(TA_IRS_App::Monitor* monitor, TA_IRS_App::VideoInput* input)
{
    bool success = false;
    TA_ASSERT(m_monitorAssignChangeOb != NULL, "m_monitorAssignChangeOb should not be null");
    if (monitor == m_localMonitor)
    {
        success = assignVideoToLocal(input);
    }
    else if (monitor->isQuadMonitor())
    {
        success = assignVideoToQuad(input, monitor->getDisplayOrder());
    }
    else
    {
        TA_ASSERT(false, "Invalid monitor type");
    }
    CheckShowOrHideWindow();
    return success;
}

void StationDlgVideoPlayImos::OnClose()
{
    ShowWindow(SW_HIDE);
    //assignMonitor(m_localMonitor, NULL);
}

bool StationDlgVideoPlayImos::assignVideoToLocal(TA_IRS_App::VideoInput* videoInput)
{
    bool success = false;
    unsigned long errorCode = 0;
    do
    {
        // if quad mode
        if (true == m_isQuadMode)
        {
            // In case user change the layout, force it to quad layout
            if (false == setLayoutToQuad())
            {
                break;
            }
            // clear all video in quad
            for (int i = 1; i <= 4; ++i)
            {
                if (false == assignVideoToQuad(NULL, i))
                {
                    break;
                }
            }
        }
        // force change layout to single
        if (false == setLayoutToSingle())
        {
            break;
        }
        m_isQuadMode = false;
        TA_IRS_App::VideoInput* currentInput = m_localMonitor->getInput();
        if (currentInput == videoInput)
        {
            success = true;
            break;
        }
        // first clear current
        if (NULL != currentInput)
        {
            // check the input type
            TA_IRS_App::Camera* cam = dynamic_cast<TA_IRS_App::Camera*>(currentInput);
            TA_IRS_App::Sequence* seq = dynamic_cast<TA_IRS_App::Sequence*>(currentInput);
            if (NULL != cam)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "m_imosCtrl.IMOSAX_StopFrameLive(1)");
                errorCode = IMOS_CALL(m_imosCtrl.IMOSAX_StopFrameLive(1));
            }
            else if (NULL != seq)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "m_imosCtrl.IMOSAX_StopFrameSwitch(1)");
                errorCode = IMOS_CALL(m_imosCtrl.IMOSAX_StopFrameSwitch(1));
            }
            else
            {
                TA_ASSERT(false, "Invalid input type");
            }
            LOG_ERRORCODE_ERROR(errorCode);
            if (ERR_COMMON_SUCCEED != errorCode)
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
            m_ptzCtrl->enablePTZButton(false);
        }
        // assign new camera/sequence to local monitor
        if (NULL != videoInput)
        {
            TA_IRS_App::Camera* cam = dynamic_cast<TA_IRS_App::Camera*>(videoInput);
            TA_IRS_App::Sequence* seq = dynamic_cast<TA_IRS_App::Sequence*>(videoInput);
            if (NULL != cam)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Frame no:1, Camera address:%s", videoInput->getAddress().c_str());
                errorCode = IMOS_CALL(m_imosCtrl.IMOSAX_StartFrameLive(1, videoInput->getAddress().c_str()));
            }
            else if (NULL != seq)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Frame no:1, Sequence address:%s", videoInput->getAddress().c_str());
                errorCode = IMOS_CALL(m_imosCtrl.IMOSAX_StartFrameSwitch(1, videoInput->getAddress().c_str()));
            }
            else
            {
                TA_ASSERT(false, "Invalid input type");
            }
            LOG_ERRORCODE_ERROR(errorCode);
            if (ERR_COMMON_SUCCEED != errorCode)
            {
                break;
            }
            m_localMonitor->assignInput(videoInput);
            m_monitorAssignChangeOb->monitorAssignChanged(m_localMonitor);
            if (NULL != cam)
            {
                m_ptzCtrl->enablePTZButton(true);
            }
        }
        success = true;
    }
    while (0);
    return success;
}
// quadNumber = 1 mean quad1
bool StationDlgVideoPlayImos::assignVideoToQuad(TA_IRS_App::VideoInput* videoInput, int quadNumber)
{
    TA_ASSERT(quadNumber >= 1 && quadNumber <= 4, "Invalid quad number");
    bool success = false;
    unsigned long errorCode = 0;
    do
    {
        // if in single monitor mode
        if (!m_isQuadMode)
        {
            // In case user change the layout, force it to single layout
            if (!setLayoutToSingle())
            {
                break;
            }
            // clear video in single monitor
            if (!assignVideoToLocal(NULL))
            {
                break;
            }
        }
        // force change layout to quad
        if (!setLayoutToQuad())
        {
            break;
        }
        m_isQuadMode = true;
        TA_IRS_App::Monitor* mon = m_quad[quadNumber - 1];
        TA_IRS_App::VideoInput* currentInput = mon->getInput();
        if (currentInput == videoInput)
        {
            success = true;
            break;
        }
        // first clear current
        if (NULL != currentInput)
        {
            // check the input type
            TA_IRS_App::Camera* cam = dynamic_cast<TA_IRS_App::Camera*>(currentInput);
            TA_IRS_App::Sequence* seq = dynamic_cast<TA_IRS_App::Sequence*>(currentInput);
            if (NULL != cam)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "m_imosCtrl.IMOSAX_StopFrameLive(%d)", quadNumber);
                errorCode = IMOS_CALL(m_imosCtrl.IMOSAX_StopFrameLive(quadNumber));
            }
            else if (NULL != seq)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "m_imosCtrl.IMOSAX_StopFrameSwitch(%d)", quadNumber);
                errorCode = IMOS_CALL(m_imosCtrl.IMOSAX_StopFrameSwitch(quadNumber));
            }
            else
            {
                TA_ASSERT(false, "Invalid input type");
            }
            LOG_ERRORCODE_ERROR(errorCode);
            if (ERR_COMMON_SUCCEED != errorCode)
            {
                break;
            }
            mon->assignInput(NULL);
            m_monitorAssignChangeOb->monitorAssignChanged(mon);
            try
            {
                DisplayItemManager::getInstance().delEntityRelationship(mon->getKey());
            }
            catch (...)
            {
            }
        }
        // assign new camera/sequence to select quad screen
        if (NULL != videoInput)
        {
            TA_IRS_App::Camera* cam = dynamic_cast<TA_IRS_App::Camera*>(videoInput);
            TA_IRS_App::Sequence* seq = dynamic_cast<TA_IRS_App::Sequence*>(videoInput);
            if (NULL != cam)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Frame no:%d, Camera address:%s",
                            quadNumber, videoInput->getAddress().c_str());
                errorCode = IMOS_CALL(m_imosCtrl.IMOSAX_StartFrameLive(quadNumber, videoInput->getAddress().c_str()));
            }
            else if (NULL != seq)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Frame no:%d, Sequence address:%s",
                            quadNumber, videoInput->getAddress().c_str());
                errorCode = IMOS_CALL(m_imosCtrl.IMOSAX_StartFrameSwitch(quadNumber, videoInput->getAddress().c_str()));
            }
            else
            {
                TA_ASSERT(false, "Invalid input type");
            }
            LOG_ERRORCODE_ERROR(errorCode);
            if (ERR_COMMON_SUCCEED != errorCode)
            {
                break;
            }
            mon->assignInput(videoInput);
            m_monitorAssignChangeOb->monitorAssignChanged(mon);
        }
        success = true;
    }
    while (0);
    return success;
}

// IPTZCommandListener method
bool StationDlgVideoPlayImos::onPTCommand(unsigned short ptDirection, bool isStopCommand)

{
    TA_ASSERT(1 <= ptDirection && ptDirection <= 4, "Invalid pt direction value");
    TA_IRS_App::Camera* ca = NULL;
    bool success = false;
    unsigned long errorCode = 0;
    do
    {
        ca = dynamic_cast<Camera*>(m_localMonitor->getInput());
        if (NULL == ca)
        {
            success = true;
            break;
        }
        TCHAR camCode[IMOS_RES_CODE_LEN];
        strncpy(camCode, ca->getAddress().c_str(), IMOS_RES_CODE_LEN);
        PTZCtrlStartStop ptz(m_imosCtrl, camCode);
        errorCode = ptz.getErrorCode();
        LOG_ERRORCODE_ERROR(errorCode);
        if (ERR_COMMON_SUCCEED != errorCode)
        {
            break;
        }
        /*
        unsigned short panSpeed = m_ptzCtrl->getPanSpeed();
        unsigned short tiltSpeed = m_ptzCtrl->getTiltSpeed();
        // If speed change, send set speed command
        if(ca->getPanSeep() != panSpeed  || ca->getTiltSeep() != tiltSpeed)
        {
            errorCode = m_imosCtrl.IMOSAX_SendPtzCtrlCommand(camCode, MW_PTZ_SPEEDSET, panSpeed, tiltSpeed, 0);
            LOG_ERRORCODE_ERROR(errorCode);
            if(ERR_COMMON_SUCCEED != errorCode)
            {
                break;
            }
            ca->setPanSeep(panSpeed);
            ca->setTiltSeep(tiltSpeed);
        }
        */
        int row = ((true == isStopCommand) ? 1 : 0);
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "IMOSAX_SendPtzCtrlCommand, Cam address:%s, Cmd:%lu, Param1:%lu, Param2:%lu, Param3:%lu",
                    camCode, ptCommands[row][ptDirection], PTZ_SPEED, PTZ_SPEED, 0);
        errorCode = IMOS_CALL(m_imosCtrl.IMOSAX_SendPtzCtrlCommand(camCode, ptCommands[row][ptDirection], PTZ_SPEED, PTZ_SPEED, 0));
        LOG_ERRORCODE_ERROR(errorCode);
        if (ERR_COMMON_SUCCEED != errorCode)
        {
            break;
        }
        success = true;
    }
    while (0);
    return success;
}

bool StationDlgVideoPlayImos::onZoomCommand(unsigned short zoomDirection, bool isStopCommand)
{
    TA_ASSERT(1 <= zoomDirection && zoomDirection <= 2, "Invalid zoom direction value");
    TA_IRS_App::Camera* ca = NULL;
    bool success = false;
    unsigned long errorCode = 0;
    do
    {
        ca = dynamic_cast<Camera*>(m_localMonitor->getInput());
        if (NULL == ca)
        {
            success = true;
            break;
        }
        TCHAR camCode[IMOS_RES_CODE_LEN];
        strncpy(camCode, ca->getAddress().c_str(), IMOS_RES_CODE_LEN);
        PTZCtrlStartStop ptz(m_imosCtrl, camCode);
        errorCode = ptz.getErrorCode();
        LOG_ERRORCODE_ERROR(errorCode);
        if (ERR_COMMON_SUCCEED != errorCode)
        {
            break;
        }
        int row = ((true == isStopCommand) ? 1 : 0);
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "IMOSAX_SendPtzCtrlCommand, Cam address:%s, Cmd:%lu, Param1:%lu, Param2:%lu, Param3:%lu",
                    camCode, zoomCommands[row][zoomDirection], PTZ_SPEED, PTZ_SPEED, 0);
        errorCode = IMOS_CALL(m_imosCtrl.IMOSAX_SendPtzCtrlCommand(camCode, zoomCommands[row][zoomDirection], PTZ_SPEED, PTZ_SPEED, 0));
        LOG_ERRORCODE_ERROR(errorCode);
        if (ERR_COMMON_SUCCEED != errorCode)
        {
            break;
        }
        success = true;
    }
    while (0);
    return success;
}

bool StationDlgVideoPlayImos::onPanCommand(unsigned short panDirection, bool isStopCommand)
{
    TA_ASSERT(1 <= panDirection && panDirection <= 2, "Invalid pt direction value");
    TA_IRS_App::Camera* ca = NULL;
    bool success = false;
    unsigned long errorCode = 0;
    do
    {
        ca = dynamic_cast<Camera*>(m_localMonitor->getInput());
        if (NULL == ca)
        {
            success = true;
            break;
        }
        TCHAR camCode[IMOS_RES_CODE_LEN];
        strncpy(camCode, ca->getAddress().c_str(), IMOS_RES_CODE_LEN);
        PTZCtrlStartStop ptz(m_imosCtrl, camCode);
        errorCode = ptz.getErrorCode();
        LOG_ERRORCODE_ERROR(errorCode);
        if (ERR_COMMON_SUCCEED != errorCode)
        {
            break;
        }
        /*
        unsigned short panSpeed = m_ptzCtrl->getPanSpeed();
        unsigned short tiltSpeed = m_ptzCtrl->getTiltSpeed();
        // If speed change, send set speed command
        if(ca->getPanSeep() != panSpeed  || ca->getTiltSeep() != tiltSpeed)
        {
            errorCode = m_imosCtrl.IMOSAX_SendPtzCtrlCommand(camCode, MW_PTZ_SPEEDSET, panSpeed, tiltSpeed, 0);
            LOG_ERRORCODE_ERROR(errorCode);
            if(ERR_COMMON_SUCCEED != errorCode)
            {
                break;
            }
            ca->setPanSeep(panSpeed);
            ca->setTiltSeep(tiltSpeed);
        }
        */
        int row = ((true == isStopCommand) ? 1 : 0);
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "IMOSAX_SendPtzCtrlCommand, Cam address:%s, Cmd:%lu, Param1:%lu, Param2:%lu, Param3:%lu",
                    camCode, panCommands[row][panDirection], PTZ_SPEED, PTZ_SPEED, 0);
        errorCode = IMOS_CALL(m_imosCtrl.IMOSAX_SendPtzCtrlCommand(camCode, panCommands[row][panDirection], PTZ_SPEED, PTZ_SPEED, 0));
        LOG_ERRORCODE_ERROR(errorCode);
        if (ERR_COMMON_SUCCEED != errorCode)
        {
            break;
        }
        success = true;
    }
    while (0);
    return success;
}

bool StationDlgVideoPlayImos::onTiltCommand(unsigned short tiltDirection, bool isStopCommand)
{
    TA_ASSERT(1 <= tiltDirection && tiltDirection <= 2, "Invalid pt direction value");
    TA_IRS_App::Camera* ca = NULL;
    bool success = false;
    unsigned long errorCode = 0;
    do
    {
        ca = dynamic_cast<Camera*>(m_localMonitor->getInput());
        if (NULL == ca)
        {
            success = true;
            break;
        }
        TCHAR camCode[IMOS_RES_CODE_LEN];
        strncpy(camCode, ca->getAddress().c_str(), IMOS_RES_CODE_LEN);
        PTZCtrlStartStop ptz(m_imosCtrl, camCode);
        errorCode = ptz.getErrorCode();
        LOG_ERRORCODE_ERROR(errorCode);
        if (ERR_COMMON_SUCCEED != errorCode)
        {
            break;
        }
        /*
        unsigned short panSpeed = m_ptzCtrl->getPanSpeed();
        unsigned short tiltSpeed = m_ptzCtrl->getTiltSpeed();
        // If speed change, send set speed command
        if(ca->getPanSeep() != panSpeed  || ca->getTiltSeep() != tiltSpeed)
        {
            errorCode = m_imosCtrl.IMOSAX_SendPtzCtrlCommand(camCode, MW_PTZ_SPEEDSET, panSpeed, tiltSpeed, 0);
            LOG_ERRORCODE_ERROR(errorCode);
            if(ERR_COMMON_SUCCEED != errorCode)
            {
                break;
            }
            ca->setPanSeep(panSpeed);
            ca->setTiltSeep(tiltSpeed);
        }
        */
        int row = ((true == isStopCommand) ? 1 : 0);
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "IMOSAX_SendPtzCtrlCommand, Cam address:%s, Cmd:%lu, Param1:%lu, Param2:%lu, Param3:%lu",
                    camCode, tiltCommands[row][tiltDirection], PTZ_SPEED, PTZ_SPEED, 0);
        errorCode = IMOS_CALL(m_imosCtrl.IMOSAX_SendPtzCtrlCommand(camCode, tiltCommands[row][tiltDirection], PTZ_SPEED, PTZ_SPEED, 0));
        LOG_ERRORCODE_ERROR(errorCode);
        if (ERR_COMMON_SUCCEED != errorCode)
        {
            break;
        }
        success = true;
    }
    while (0);
    return success;
}


void StationDlgVideoPlayImos::CheckShowOrHideWindow()
{
    int showWindow = SW_HIDE;
    if (true == m_isQuadMode)
    {
        // Check quad
        for (int i = 0; i < 4; ++i)
        {
            if (NULL != m_quad[i]->getInput())
            {
                showWindow = SW_SHOW;
                break;
            }
        }
    }
    else
    {
        if (NULL != m_localMonitor->getInput())
        {
            showWindow = SW_SHOW;
        }
    }
    this->ShowWindow(showWindow);
}

std::string StationDlgVideoPlayImos::getErrorMessage()
{
    return m_strError;
}
}