/*
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source : $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAAgent/src/DvaMsgLib.h $
 * @author  J. Dalin
 * @version $Revision: #2 $
 * Last modification : $DateTime: 2012/02/06 16:15:14 $
 * Last modified by : $Author: haijun.li $
 *
 * 
 */

#ifndef DVAMSGLIB_H
#define DVAMSGLIB_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <map>
#include <list>

#include "app/pa/PAAgent/src/IPasDataProcessor.h"
#include "app/pa/PAAgent/src/IPasPollingObserver.h"
#include "app/pa/PAAgent/src/ITransactionWorkerUser.h"

namespace TA_IRS_App
{

struct DvaMsgHwContent 
{
    unsigned short usMsgId;
    unsigned short usMsgType;
    std::string strMsgName;
};

typedef std::map<unsigned short, DvaMsgHwContent> MsgIdToDvaMsgHwMap;
typedef std::map<unsigned short, DvaMsgHwContent>::iterator MsgIdToDvaMsgHwMapIt;

class DvaMsgLib : public IPasPollingObserver, public IPasSerialDataProcessor, public ITransactionWorkerUser
{
public:
    DvaMsgLib();
    virtual ~DvaMsgLib();

    virtual void processPollingUpdate( const TA_Base_Bus::DataBlock<unsigned short>& dbData, const unsigned long ulPolledTimeStamp  );
    virtual bool processNewData( const TA_Base_Bus::DataBlock<unsigned short>& dbData );
    virtual bool processRemainData( const TA_Base_Bus::DataBlock<unsigned short>& dbData );
    virtual void processFinalData( const TA_Base_Bus::DataBlock<unsigned short>& dbData );
    virtual void serialProcessFailed( int nReason );

    void setDvaMsgLibCounter( const std::string& strCounter );
    void setDvaMsgLibCounter( const unsigned short usCounter );

private:
	DvaMsgLib( const DvaMsgLib& );
	const DvaMsgLib& operator=( const DvaMsgLib& );

    void readDvaLibData();
    bool parseDvaMsgHwContent( const TA_Base_Bus::DataBlock<unsigned short>& dbData );
    void updateDvaMsgToDatabase();
    void updatePasParamsToDatabase();
    void updateDvaLibVersionToDatabase();

protected:
    std::string m_strLibVersionBuffer;
    bool m_bProcessingLibUpdate;
    bool m_bForceUpdateDvaLib;
    unsigned short m_usDvaMsgLibCounter;
    MsgIdToDvaMsgHwMap m_mapDvaMsgsBuffer;
    std::list<unsigned short> m_lstProcessingCounters;
};

} // namespace TA_IRS_App

#endif // DVAMSGLIB_H
