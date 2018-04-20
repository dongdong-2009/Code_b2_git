/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/ecs/ECSAgent/src/ItaEcsAlarms.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
// CachedConfig.h: interface for the CachedConfig class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(ItaEcsAlarms_INCLUDED)
#define ItaEcsAlarms_INCLUDED

#include <set>


class ItaEcsAlarms  
{

public:

    virtual bool getIsEcsInterAgentCommsFailureRaised(unsigned long locationKey)=0;
    virtual bool getIsEcsInvalidConfigurationRaised()=0;

    virtual void raiseEcsInterAgentCommsFailure( unsigned long locationKey )=0;
    virtual void closeEcsInterAgentCommsFailure( unsigned long locationKey )=0;
    virtual void closeEcsInterAgentCommsFailure()=0;

    virtual void raiseEcsZoneEmergencyNormalisation( unsigned long zoneId )=0;
    virtual void closeEcsZoneEmergencyNormalisation()=0;

    virtual void raiseEcsInvalidConfiguration( const std::string& detail )=0;
    virtual void closeEcsInvalidConfiguration( bool isForced = false )=0;


    virtual std::set<unsigned long> getProposedMasterModeAlarmsRaised()=0;
};


#endif // !defined(ItaEcsAlarms_INCLUDED)
