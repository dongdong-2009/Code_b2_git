/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/ecs/ECSAgent/src/ItaEcsAlarms.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
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
