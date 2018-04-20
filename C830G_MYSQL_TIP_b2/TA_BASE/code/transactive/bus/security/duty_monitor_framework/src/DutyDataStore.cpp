/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Bart Golab
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  * 
  * Hides the singleton implementation of DutyDataStoreSingleton from clients.
  *
  */

#include "bus/security/duty_monitor_framework/src/DutyMonitorFramework.h"

using namespace TA_Base_Bus;

void DutyDataStore::cleanUp()
{
	DutyDataStoreSingleton::cleanUp();
}

void DutyDataStore::populate(bool ignoreSystemOperators, bool ignoreAppSubsystems, bool ignoreOtherSessionSubsystemDuties)
{
    DutyDataStoreSingleton::getInstance().populate(ignoreSystemOperators, ignoreAppSubsystems,ignoreOtherSessionSubsystemDuties);
}


bool DutyDataStore::ignoreSystemOperators() 
{
    return DutyDataStoreSingleton::getInstance().ignoreSystemOperators();
}


bool DutyDataStore::ignoreAppSubsystems() 
{
    return DutyDataStoreSingleton::getInstance().ignoreAppSubsystems();
}


DataCache& DutyDataStore::getDataCache()
{
    return DutyDataStoreSingleton::getInstance().getDataCache();
}


SessionCache& DutyDataStore::getSessionCache()
{
    return DutyDataStoreSingleton::getInstance().getSessionCache();
}


DefaultRegionAssignmentMatrix& DutyDataStore::getDefRegionAssignmentMatrix()
{
    return DutyDataStoreSingleton::getInstance().getDefRegionAssignmentMatrix();
}


RegionDutyMatrix& DutyDataStore::getRegionDutyMatrix()
{
    return DutyDataStoreSingleton::getInstance().getRegionDutyMatrix();
}


SubsystemDutyMatrix& DutyDataStore::getSubsystemDutyMatrix()
{
    return DutyDataStoreSingleton::getInstance().getSubsystemDutyMatrix();
}
