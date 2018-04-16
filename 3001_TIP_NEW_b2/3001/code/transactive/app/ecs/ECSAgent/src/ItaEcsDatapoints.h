/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/ecs/ECSAgent/src/ItaEcsDatapoints.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */
// ItaEcsDatapoints.h: interface for the ItaEcsDatapoints class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(ItaEcsDatapoints_INCLUDED)
#define ItaEcsDatapoints_INCLUDED

#include <set>

class ICongestionObserver
{
public:
	virtual void processCongestionUpdate(unsigned long zoneId, unsigned long value)=0;
};

class IEmergencyObserver
{
public:
	virtual void processEmergencyUpdate(unsigned long zoneId, bool value)=0;
};




class ItaEcsDatapoints  
{

public:

    virtual void initialise() = 0;
// Common ECS datapoint access

    virtual void setCongestion(unsigned long zoneId, unsigned long value, bool fInit=false)=0;
    virtual unsigned long getCongestion(unsigned long zoneId)=0;
    virtual std::set<unsigned long> getZonesInCongestion()=0;
	virtual void attachCongestionObserver(ICongestionObserver* observer)=0;
	virtual void detachCongestionObserver(ICongestionObserver* observer)=0;

    virtual void setEmergency(unsigned long zoneId, bool value, bool fInit=false)=0;
    virtual bool getEmergency(unsigned long zoneId)=0;
    virtual std::set<unsigned long> getZonesInEmergency()=0;
	virtual void attachEmergencyObserver(IEmergencyObserver* observer)=0;
	virtual void detachEmergencyObserver(IEmergencyObserver* observer)=0;

// Station ECS Agent datapoint access

    virtual bool getMasterModeLocked()=0;
    virtual bool getLocalMode()=0;
    virtual unsigned long getCurrentStationMode()=0;

    virtual void setLastAppliedMasterMode(long value, bool fInit=false)=0;
    virtual void setLastAppliedStationMode(long value, bool fInit=false)=0;
    virtual void setSelectedMasterMode(long value, bool fInit=false)=0;
    virtual void setSelectedOverridingStationMode(long value, bool fInit=false)=0;

    virtual void setMasterModeLock(bool value, bool fInit = false)=0;
    virtual void setMasterModeLockControl(bool value, bool fInit = false)=0;

    virtual void setIscsAlive(unsigned long value)=0;

// Master ECS Agent datapoint access

    virtual void setExternalMasterModeLock(unsigned long locationKey, bool value)=0;
    virtual void setExternalLastAppliedMasterMode(unsigned long locationKey, long  value)=0;
    virtual void setExternalLastAppliedStationMode(unsigned long locationKey, long  value)=0;
    virtual void setExternalSelectedMasterMode(unsigned long locationKey, long  value)=0;
    //virtual void setExternalSelectedMasterMode(unsigned long locationKey, long  value)=0;
    virtual void setExternalSelectedOverridingStationMode(unsigned long locationKey, long  value)=0;
    //virtual void setExternalSelectedOverridingStationMode(unsigned long locationKey, long  value)=0;

    virtual bool getExternalStationInLocalMode(unsigned long locationKey)=0;

	virtual void setMasterModeInProgress(unsigned long value, bool fInit=false)=0;
    virtual unsigned long getMasterModeInProgress() =0;

    // CurrentMasterMode == MasterModeStatus
    virtual unsigned long getCurrentMasterMode()=0;
    virtual void setCurrentMasterMode(unsigned long value, bool fInit=false)=0;

    //Datapoint is removed
    //virtual void setCongestionApplied(unsigned long zoneId, bool value, bool fInit=false)=0;
	//virtual void setCongestionAppliedGivenEntityKey( unsigned long entityKey, bool value )=0;
	
	virtual bool getEmergencyApplied(unsigned long zoneId)=0;
    virtual void setEmergencyApplied(unsigned long zoneId, bool value, bool fInit = false)=0;
    
	virtual void setEvacuationSelected(unsigned long zoneId, unsigned long value, bool fInit=false)=0;
    virtual void setAirflowSelected(unsigned long zoneId, unsigned long value, bool fInit=false)=0;
    virtual void setEvacuationApplied(unsigned long zoneId, unsigned long value, bool fInit=false)=0;
    virtual void setAirflowApplied(unsigned long zoneId, unsigned long value, bool fInit=false)=0;
};


#endif // !defined(ItaEcsDatapoints_INCLUDED)
