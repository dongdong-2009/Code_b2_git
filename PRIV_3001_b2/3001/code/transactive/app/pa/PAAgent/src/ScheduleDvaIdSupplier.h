/*
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source : $File: //depot/PRIV_3001/3001/transactive/app/pa/PAAgent/src/ScheduleDvaIdSupplier.h $
 * @author  HuangQi
 * @version $Revision: #1 $
 * Last modification : $DateTime: 2018/03/14 13:51:20 $
 * Last modified by : $Author: lim.cy $
 *
 * 
 */

#ifndef SCHEDULEDVAIDSUPPLIER_H
#define SCHEDULEDVAIDSUPPLIER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <set>
#include <map>
#include "ace/RW_Mutex.h"

#include "app/pa/PAAgent/src/ITransactionOwner.h"
#include "app/pa/PAAgent/src/ITransactionWorkerUser.h"

namespace TA_IRS_App
{

class ScheduledDvaDownload;

class ScheduleDvaIdSupplier : public ITransactionWorkerUser, public ITransactionOwner
{
public:
    static void createInstance();
    static void destoryInstance();
    static ScheduleDvaIdSupplier& getInstance();

    virtual ~ScheduleDvaIdSupplier();

    void enable( const bool bEnable = true );

    virtual void transactionExecuted( int nResult );

    unsigned short applyAvailableEventId();
    void refreshEventIdAvaliableState( const unsigned short usEventId );

    bool getAllAvailableEventIds( std::set<unsigned short>& setAllIds );
    void setAllAvailableEventIds( const std::set<unsigned short>& setAllIds );

    void changeEventId( const unsigned short usFrom, const unsigned short usTo );

	void reserveEventId(const unsigned short usEventId);

private:
    ScheduleDvaIdSupplier();
	ScheduleDvaIdSupplier( const ScheduleDvaIdSupplier& );
    const ScheduleDvaIdSupplier& operator=( const ScheduleDvaIdSupplier& );

    void refreshAvailableEventIds();

protected:
    static ScheduleDvaIdSupplier* s_pInstance;

    bool m_bEnable;
    bool m_bInitialized;
    ScheduledDvaDownload* m_pHistoryDownload;
    std::set<unsigned short> m_setAvailableEventId;
	std::set<unsigned short> m_setReservedEventId;
    mutable ACE_RW_Mutex m_mtxEventIdSet;
    mutable ACE_RW_Mutex m_mtxReservedEventIdSet;
};

} // namespace TA_IRS_App

#endif // SCHEDULEDVAIDSUPPLIER_H
