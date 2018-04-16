/*
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source : $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAAgent/src/PasFaultStatus.h $
 * @author  J. Dalin
 * @version $Revision: #3 $
 * Last modification : $DateTime: 2014/11/19 14:38:40 $
 * Last modified by : $Author: qi.huang $
 *
 * 
 */

#ifndef PasFaultZones_H
#define PasFaultZones_H

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
typedef std::map<unsigned short, unsigned short> MapStationIdToSystemFault;

class PasFaultStatus : public IPasPollingObserver, public IPasDataProcessor, ITransactionWorkerUser
{
public:
    PasFaultStatus();
    virtual ~PasFaultStatus();

    void initZoneState();
    void initSystemFault();

    virtual void processPollingUpdate( const TA_Base_Bus::DataBlock<unsigned short>& dbData, const unsigned long ulPolledTimeStamp  );
    virtual void processData( const TA_Base_Bus::DataBlock<unsigned short>& dbData );
    virtual void getDataFailed( int nReason );

    void enable( const bool bEnable = true );

private:
	PasFaultStatus( const PasFaultStatus& );
	const PasFaultStatus& operator=( const PasFaultStatus& );

    void readFaultZoneData();
    void refreshFaultZones( const TA_Base_Bus::DataBlock<unsigned short>& dbData );
    void refreshSystemFaults( const TA_Base_Bus::DataBlock<unsigned short>& dbData );
    void refreshSystemFaultDatapoint( const unsigned short usFaultCode );
    void refreshMajorAlarmDatapoint( const unsigned short usStationId, const unsigned short usFaultCode );
    void refreshMinorAlarmDatapoint( const unsigned short usStationId, const unsigned short usFaultCode );
    void refreshFaultZoneDatapoint( const unsigned int unZoneId, const unsigned short usNewState );

private:
    MapStationToZoneState m_mapStationZoneIdToZoneState;
    MapStationIdToSystemFault m_mapStationIdToSystemFault;
    unsigned short m_usFaultZoneCounter;
    unsigned short m_usSystemFaultCode;
    bool m_bForceUpdateFaultZones;
    bool m_bForceUpdateSysFault;
    bool m_bFaultZonesDpInitialized;
};

} // namespace TA_IRS_App

#endif // PasFaultZones_H
