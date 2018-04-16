/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/bus/security/duty_monitor_framework/src/ISubsystemDutyObserver.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * This class should be implemented by clients who want to be notified when  
  * SubsystemDutyMatrix contents change due to subsystem duty add/remove events.
  *
  */

#include "bus/security/duty_monitor_framework/src/DutyMonitorFramework.h"

using namespace TA_Base_Bus;


ISubsystemDutyObserver::ISubsystemDutyObserver(SubsystemDutyMatrix& subsystemDutyMatrix) :
    SeObserver<SubsystemDutyMatrix>(subsystemDutyMatrix)
{
    FUNCTION_ENTRY("ISubsystemDutyObserver(SubsystemDutyMatrix& subsystemDutyMatrix)");
    FUNCTION_EXIT;
}


ISubsystemDutyObserver::~ISubsystemDutyObserver()
{
    FUNCTION_ENTRY("~ISubsystemDutyObserver()");
    FUNCTION_EXIT;
}


void ISubsystemDutyObserver::update(SubsystemDutyMatrix& subsystemDutyMatrix)
{
    FUNCTION_ENTRY("update(SubsystemDutyMatrix& subsystemDutyMatrix)");

    const SubsystemDutyUpdate* dutyUpdate = subsystemDutyMatrix.getDutyUpdate();

    if (dutyUpdate != NULL)
    {
        const SubsystemDutyUpdate dutyUpdateCopy(*dutyUpdate);

        dutyChanged(dutyUpdateCopy);
    }

    FUNCTION_EXIT;
}
