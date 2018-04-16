/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/bus/security/duty_monitor_framework/src/IRegionDutyObserver.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2017/10/31 16:21:54 $
  * Last modified by:  $Author: CM $
  * 
  * This class should be implemented by clients who want to be notified when  
  * RegionDutyMatrix contents change due to region duty add/remove events.
  *
  */

#include "bus/security/duty_monitor_framework/src/DutyMonitorFramework.h"

using namespace TA_Base_Bus;


IRegionDutyObserver::IRegionDutyObserver(RegionDutyMatrix& regionDutyMatrix) :
    SeObserver<RegionDutyMatrix>(regionDutyMatrix)
{
    FUNCTION_ENTRY("IRegionDutyObserver(RegionDutyMatrix& regionDutyMatrix)");
    FUNCTION_EXIT;
}


IRegionDutyObserver::~IRegionDutyObserver()
{
    FUNCTION_ENTRY("~IRegionDutyObserver()");
    FUNCTION_EXIT;
}


void IRegionDutyObserver::update(RegionDutyMatrix& regionDutyMatrix)
{
    FUNCTION_ENTRY("update(RegionDutyMatrix& regionDutyMatrix)");

    const RegionDutyUpdate* dutyUpdate = regionDutyMatrix.getDutyUpdate();

    if (dutyUpdate != NULL)
    {
        const RegionDutyUpdate dutyUpdateCopy(*dutyUpdate);

        dutyChanged(dutyUpdateCopy);
    }

    FUNCTION_EXIT;
}
