/*
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source : $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAAgent/src/PasZonesState.h $
 * @author  J. Dalin
 * @version $Revision: #5 $
 * Last modification : $DateTime: 2014/04/10 18:43:22 $
 * Last modified by : $Author: qi.huang $
 *
 * 
 */

#ifndef PASZONESSTATE_H
#define PASZONESSTATE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/pa/PAAgent/src/IPasDataProcessor.h"
#include "app/pa/PAAgent/src/IPasPollingObserver.h"
#include "app/pa/PAAgent/src/ITransactionWorkerUser.h"

namespace TA_IRS_App
{

typedef std::map<unsigned int, unsigned short> MapZoneIdToState;
typedef std::map<unsigned short, MapZoneIdToState> MapStationToZoneState;
typedef std::map<unsigned long, unsigned short> MapZoneKeyToZoneState;
typedef std::map<unsigned short, unsigned short> MapPagingConsoleState;
//added by lixiaoxia
typedef std::map<unsigned short, MapZoneIdToState> MapStationToTransactionState;
typedef std::map<unsigned long, unsigned short> MapZoneKeyToTransactionState;
typedef std::map<unsigned long, unsigned short*> MapZoneKeyToTransactionStatePtr;

class PasZonesState : public IPasPollingObserver, public IPasDataProcessor, ITransactionWorkerUser
{
public:
    virtual ~PasZonesState();
    static void createInstance();
    static void destoryInstance();
    static PasZonesState& getInstance();

    virtual void processPollingUpdate( const TA_Base_Bus::DataBlock<unsigned short>& dbData, const unsigned long ulPolledTimeStamp  );
    virtual void processData( const TA_Base_Bus::DataBlock<unsigned short>& dbData );
    virtual void getDataFailed( int nReason );

    void getZoneStatesByZoneKey( MapZoneKeyToTransactionState& mapTransactionStates );//modified by lixiaoxia
    bool isZoneStateFault( const unsigned short usZoneState );
    //added by lixiaoxia
	bool isZoneFree( const unsigned short ulTransationState );
	bool areZonesFreeOfTransaction( const std::vector<unsigned long>& vecZones, const unsigned short usTransactionId );
	bool isZoneFreeOfTransaction( const unsigned short usTransactionState,const unsigned short usTransactionId );

    bool isConsolePlaybacking( const unsigned short usSourceId );
    bool isConsoleRecording( const unsigned short usSourceId );
    bool isConsoleLiving( const unsigned short usSourceId );

    unsigned long getZoneUpdateTime();
    bool isPagingConsoleUpdated( const unsigned long ulSigTime );

    unsigned long notifyReadPagingConsoleState( const bool bRead = true );

    void enable( const bool bEnable = true );

private:
    PasZonesState();
	PasZonesState( const PasZonesState& );
	const PasZonesState& operator=( const PasZonesState& );

    void initZoneState();
	void initTransactionState();//added by lixiaoxia
    void initPagingConsoleState();
    void readZoneData();
    void refreshPagingConsole( const TA_Base_Bus::DataBlock<unsigned short>& dbData );
    void refreshZoneStates( const TA_Base_Bus::DataBlock<unsigned short>& dbData );
	void refreshTransationStates( const TA_Base_Bus::DataBlock<unsigned short>& dbData );//added by lixiaoxia
    void refreshZoneDatapoint( const unsigned int unZoneId, const unsigned short usNewState );

private:
    static PasZonesState* s_pInstance;
    MapStationToZoneState m_mapStationZoneIdToZoneState;
	//added by lixiaoxia
	MapStationToTransactionState m_mapStationZoneIdToTransactionState;
	MapZoneKeyToTransactionStatePtr m_mapZoneKeyToTransactionState;
    MapPagingConsoleState m_mapPagingConsoleStates;
    unsigned long m_ulZoneRefreshTime;
    unsigned long m_ulPagingConsoleRefreshTime;
    unsigned short m_usZoneStateCounter;
    bool m_bForceUpdateZonesState;
    bool m_bZonesStateDpInitialized;
};

} // namespace TA_IRS_App

#endif // PASZONESSTATE_H
