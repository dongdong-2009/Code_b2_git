/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/security/duty_monitor_framework/src/IRegionDutyObserver.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
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
