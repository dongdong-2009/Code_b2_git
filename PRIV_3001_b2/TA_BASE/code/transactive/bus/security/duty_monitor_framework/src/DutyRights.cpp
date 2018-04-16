/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/bus/security/duty_monitor_framework/src/DutyRights.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * Hides the singleton implementation of DutyRightsSingleton from clients.
  *
  */

#include "bus/security/duty_monitor_framework/src/DutyMonitorFramework.h"

using namespace TA_Base_Bus;


DutyRights::DutyRights()
{
}


DutyRights::~DutyRights()
{
}


bool DutyRights::canTransferRegions()
{
    return DutyRightsSingleton::getInstance().canTransferRegions();
}


bool DutyRights::canTransferSubsystems(long lActionId)//TD19218 LiDan 27,Jun,2007
{
    return DutyRightsSingleton::getInstance().canTransferSubsystems(lActionId);//TD19218 LiDan 27,Jun,2007
}


bool DutyRights::canDegradeSubsystems()
{
    return DutyRightsSingleton::getInstance().canDegradeSubsystems();
}
