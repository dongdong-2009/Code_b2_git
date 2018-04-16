/*
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source : $File: //depot/PRIV_3001/3001/transactive/app/pa/PAAgent/src/ScheduledDvaDownload.h $
 * @author  Huang Qi
 * @version $Revision: #1 $
 * Last modification : $DateTime: 2018/03/14 13:51:20 $
 * Last modified by : $Author: lim.cy $
 *
 * 
 */

#ifndef SCHEDULEDDVADOWNLOAD_H
#define SCHEDULEDDVADOWNLOAD_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>
#include <set>

#include "ace/Mutex.h"

#include "app/pa/PAAgent/src/PasTransaction.h"

namespace TA_IRS_App
{

class ScheduledDvaDownload : public PasTransaction
{
public:
    ScheduledDvaDownload( const bool bAtOcc, ITransactionOwner* pOwner );
    virtual ~ScheduledDvaDownload();

    bool isEventIdAvailable( const unsigned short usEventId );
    void setMinEventId( const unsigned short usMin );
    void setRefreshEventId( const unsigned short usEventId );
    void setRefreshEventIds( const unsigned short usMinId, const unsigned short usMaxId );

    std::map<unsigned short, bool> getEventIdAvaliableMap();

private:
    ScheduledDvaDownload();
	ScheduledDvaDownload( const ScheduledDvaDownload& );
	const ScheduledDvaDownload& operator=( const ScheduledDvaDownload& );

    virtual int executeOccImp( PasConnectionAgency& refConnection );
    virtual int executeStnImp( PasConnectionAgency& refConnection );

private:
    ACE_RW_Mutex m_mtxEventIds;
    std::map<unsigned short, bool> m_mapEventIdAvailable;
    unsigned short m_usMinEventId;
};

} // namespace TA_IRS_App

#endif // SCHEDULEDDVADOWNLOAD_H
