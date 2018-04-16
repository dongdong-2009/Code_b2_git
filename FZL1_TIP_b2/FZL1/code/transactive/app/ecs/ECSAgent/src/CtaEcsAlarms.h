/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/ecs/ECSAgent/src/CtaEcsAlarms.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// CachedConfig.h: interface for the CachedConfig class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(CtaEcsAlarms_INCLUDED)
#define CtaEcsAlarms_INCLUDED

#include <map>
#include <set>

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "app/ecs/ECSAgent/src/AbstractFailoverable.h"
#include "app/ecs/ECSAgent/src/ItaEcsAlarms.h"

class ItaEcsCachedConfig;
class ItaEcsCachedMaps;

namespace TA_Base_Core 
{ 
    class NonUniqueAlarmHelper; 
    class AlarmHelper; 
}

class CtaEcsAlarms : public virtual ItaEcsAlarms,
					 public virtual AbstractFailoverable
                         
{

public:

    CtaEcsAlarms(ItaEcsCachedConfig& cachedConfigInstance, ItaEcsCachedMaps& cachedMapsInstance);
    virtual ~CtaEcsAlarms();

	// AbstractFailoverable overrides
    virtual void setControl(bool isOnStartup=false);
    //virtual void setMonitor(bool isOnStartup=false);


    virtual bool getIsEcsInterAgentCommsFailureRaised(unsigned long locationKey)
    {
        TA_Base_Core::ThreadGuard guard( m_lock );
        return (m_locationKeyToAlarmIdMap[locationKey]!="");
    }

    virtual bool getIsEcsInvalidConfigurationRaised()
    {
        return m_ecsInvalidConfigurationRaised;
    }

    virtual void raiseEcsInterAgentCommsFailure( unsigned long locationKey );
    virtual void closeEcsInterAgentCommsFailure( unsigned long locationKey );
    virtual void closeEcsInterAgentCommsFailure();

    virtual void raiseEcsZoneEmergencyNormalisation( unsigned long zoneId );
    virtual void closeEcsZoneEmergencyNormalisation();

    virtual void raiseEcsInvalidConfiguration( const std::string& detail );
    virtual void closeEcsInvalidConfiguration( bool isForced = false );


    virtual std::set<unsigned long> getProposedMasterModeAlarmsRaised()
    {
        return m_proposedMasterModeAlarmsRaised;
    }

protected:

    ItaEcsCachedConfig&                         m_cachedConfigInstance;
    ItaEcsCachedMaps&                           m_cachedMapsInstance;

    typedef std::map<unsigned long, std::string>        
        KeyToAlarmId;
    typedef std::map<unsigned long, std::string>::iterator        
        KeyToAlarmIdIt;


    TA_Base_Core::ReEntrantThreadLockable       m_lock;
	/*/TD18093,jianghp
    TA_Base_Core::NonUniqueAlarmHelper&         m_nonUniqueAlarmHelper;
    TA_Base_Core::AlarmHelper&                  m_alarmHelper;
	//TD18093,jianghp*/

    KeyToAlarmId                                m_locationKeyToAlarmIdMap;

    bool                                        m_ecsInvalidConfigurationRaised;
    std::set<unsigned long>                     m_proposedMasterModeAlarmsRaised;

};


#endif // !defined(CtaEcsAlarms_INCLUDED)
