/*
 * The source code in this file is the property of Ripple Systems and is not for redistribution in any form.
 *
 * Source : $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_agent/src/DatapointTriggerObserver.h $
 * @author  Huang Qi
 * @version $Revision: #3 $
 * Last modification : $DateTime: 2013/05/10 10:14:54 $
 * Last modified by : $Author: qi.huang $
 *
 * 
 */

#ifndef DATAPOINTTRIGGEROBSERVER_H
#define DATAPOINTTRIGGEROBSERVER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/response_plans/plan_agent/src/ITriggerObserver.h"
#include "bus/scada/proxy_library/src/IEntityUpdateEventProcessor.h"
#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"

namespace TA_Base_App
{

class IDssEngine;

class DatapointTriggerObserver : public ITriggerObserver, TA_Base_Bus::IEntityUpdateEventProcessor
{
public:
    DatapointTriggerObserver( IDssEngine& refEngine, const unsigned long ulKey );
    virtual ~DatapointTriggerObserver();

    virtual void processEntityUpdateEvent( unsigned long entityKey, TA_Base_Bus::ScadaEntityUpdateType updateType );

private:
    DatapointTriggerObserver();
	DatapointTriggerObserver( const DatapointTriggerObserver& );
	const DatapointTriggerObserver& operator=( const DatapointTriggerObserver& );

    void init();
    void uninit();

private:
    IDssEngine& m_refEngine;
    TA_Base_Bus::DataPointProxySmartPtr m_ptrDatapointProxy;
    unsigned long m_ulTriggerKey;
    unsigned long m_ulLastestTimestamp;
    bool m_bIsInitial;
    bool m_bCurValue;
};

} // namespace TA_Base_App

#endif // DATAPOINTTRIGGEROBSERVER_H
