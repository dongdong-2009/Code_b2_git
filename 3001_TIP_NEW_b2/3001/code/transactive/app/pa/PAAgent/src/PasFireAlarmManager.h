/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAAgent/src/PasFireAlarmManager.h $
  * @author:  HuangQi
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2013/04/05 10:12:01 $
  * Last modified by:  $Author: qi.huang $
  *
  */
#if !defined(PASFIREALARMMANAGER__INCLUDED_)
#define PASFIREALARMMANAGER__INCLUDED_

#include "ace/RW_Mutex.h"

#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"
#include "bus/scada/proxy_library/src/IEntityUpdateEventProcessor.h"
#include "bus/scada/common_library/src/EventProcessor.h"
#include "app/pa/PAAgent/src/IPasPollingObserver.h"

namespace TA_Base_Bus
{
    class DataPointWriteRequest;
}

namespace TA_IRS_App
{

typedef std::map<unsigned long, TA_Base_Bus::DataPointProxySmartPtr*> EntityKeyToProxyMap;
typedef EntityKeyToProxyMap::iterator EntityKeyToProxyMapIt;

typedef std::map<unsigned long, bool> EntityKeyToFireAckedMap;
typedef EntityKeyToFireAckedMap::iterator EntityKeyToFireAckedMapIt;

typedef std::map<unsigned long, unsigned short> EntityKeyToStationIdMap;
typedef EntityKeyToStationIdMap::iterator EntityKeyToStationIdMapIt;

typedef std::map<unsigned short, bool> StationIdToOnFiredMap;
typedef StationIdToOnFiredMap::iterator StationIdToOnFiredMapMapIt;

typedef std::map<unsigned short, unsigned short> StationToFireAlarmMap;
typedef StationToFireAlarmMap::iterator StationToFireAlarmMapIt;

class PasFireAlarmManager : public IPasPollingObserver, public virtual TA_Base_Bus::IEntityUpdateEventProcessor, 
    public EventProcessor<TA_Base_Bus::DataPointWriteRequest>
{
public:
    PasFireAlarmManager();
    virtual ~PasFireAlarmManager();

    void enable( bool bEnable = true );

    // Implementation of IEntityUpdateEventProcessor
    virtual void processEntityUpdateEvent(unsigned long entityKey, TA_Base_Bus::ScadaEntityUpdateType updateType);
    // Implementation of IPasPollingObserver
    virtual void processPollingUpdate( const TA_Base_Bus::DataBlock<unsigned short>& dbData, const unsigned long ulPolledTimeStamp  );
    // Implementation of EventProcessor
    virtual void processEvent( TA_Base_Bus::DataPointWriteRequest* newEvent );

private:
    PasFireAlarmManager( const PasFireAlarmManager& refSrc );
    const PasFireAlarmManager& operator=( const PasFireAlarmManager& refSrc );

    void updatePasFireAlarm( const unsigned short usAlarm, const unsigned short usStationId );
    void acknowledgeFireAlarm( const unsigned short usStationId );
    void initFireAlarmMaps();
    bool isOnFired( const unsigned short usFireState );

private:
    EntityKeyToProxyMap m_mapPasFireAlarm;
    StationToFireAlarmMap m_mapStationToFireAlarm;
    EntityKeyToFireAckedMap m_mapFireAlarmAcked;
    EntityKeyToStationIdMap m_mapFireActiveKeyToStation;
    mutable ACE_RW_Mutex m_mtxFireAlarmProxyMapLock;
    bool m_bForceUpdatePasFireAlarm;
    volatile bool m_bEnabled;
};

};

#endif // !defined(PASFIREALARMMANAGER__INCLUDED_)
