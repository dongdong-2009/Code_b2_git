/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/ecs/ECSAgent/src/EcsZoneDataSource.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */
///////////////////////////////////////////////////////////
//
//  EcsZoneDataSource.h
//  Implementation of the Class EcsZoneDataSource
//  Generated by Enterprise Architect
//  Created on:      25-Sep-2003 18:53:34
//  Original author: 
//  
///////////////////////////////////////////////////////////
//  Modification history:
//  
//
///////////////////////////////////////////////////////////


#if !defined(EcsZoneDataSource__INCLUDED_)
#define EcsZoneDataSource__INCLUDED_


#include "app/ecs/ECSAgent/src/TrainState.h"
#include "app/ecs/ECSAgent/src/AbstractFailoverable.h"

#include "bus/ats/ats_agent/IDL/src/IAtsEcsCorbaDef.h"
#include "bus/ecs/ecs_agent/IDL/src/CommonEcsAgentCorbaDef.h"

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/message/IDL/src/CommsMessageCorbaDef.h"
#include "core/naming/src/NamedObject.h"



class ItaEcsCachedConfig;
class ItaEcsCachedMaps;
class ItaEcsDatapoints;

namespace TA_Base_Bus
{
    class DataPoint;
}

typedef TA_Base_Core::NamedObject<TA_IRS_Bus::IAtsEcsCorbaDef,
	   					     TA_IRS_Bus::IAtsEcsCorbaDef_ptr, 
	           			     TA_IRS_Bus::IAtsEcsCorbaDef_var> AtsAgentNamedObject;


typedef std::multimap<unsigned long, TrainState*>   ZoneIdToTrainStatesMap;
typedef ZoneIdToTrainStatesMap::iterator            ZoneIdToTrainStatesMapIt;
typedef ZoneIdToTrainStatesMap::value_type          zoneId2trainState;


class EcsZoneDataSource
: public virtual TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>,
  public virtual AbstractFailoverable
  
{

public:

    EcsZoneDataSource(
        ItaEcsDatapoints& theDatapoints,
        ItaEcsCachedMaps& theMaps,
        ItaEcsCachedConfig& theConfig);

    virtual ~EcsZoneDataSource();

    // AbstractFailoverable overrides
    virtual void setControl(bool isOnStartup=false);
    virtual void setMonitor(bool isOnStartup=false);

    void refreshCompleteAtsData();

    virtual void receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef&  message);

	void removeEmergencyIfAlarmManuallyClosed(unsigned long zoneId);

	void updateAllEcsZoneDataPoints();



protected:


    void processAtsPacket(const TA_IRS_Bus::IAtsEcsCorbaDef::AtsEcsMessageCorbaDef& packet,
                          bool updateAffectedDataPoints = true);

    void clearTrainStates();

    // Will ignore unknown ECS Zones
    void updateEcsZoneDataPoints(unsigned long ecsZoneId);

    ZoneIdToTrainStatesMapIt getRecordOfTrainId(unsigned long trainId);
    // throws TrainIdNotFoundException


private:

    EcsZoneDataSource( const EcsZoneDataSource& theEcsZoneDataSource);


protected:

	TA_Base_Core::ReEntrantThreadLockable               m_trainStatesMapLock;
    ZoneIdToTrainStatesMap                              m_trainStatesMap;
    AtsAgentNamedObject*                                m_atsAgent;
    AtsAgentNamedObject*                                m_spareAtsAgent;
    ItaEcsCachedMaps&                                   m_theMaps;
    ItaEcsCachedConfig&                                 m_theConfig;
    ItaEcsDatapoints&									m_theDatapoints;
};

#endif // !defined(EcsZoneDataSource__INCLUDED_)
