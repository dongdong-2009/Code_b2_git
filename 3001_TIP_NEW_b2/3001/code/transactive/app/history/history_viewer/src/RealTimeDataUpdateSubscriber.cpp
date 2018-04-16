#if !defined(REAL_TIME_DATA_UPDATE_SUBSCRIBER_CPP)
#define REAL_TIME_DATA_UPDATE_SUBSCRIBER_CPP

/**
  * RealTimeDataUpdateSubscriber.h
  *
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/history/history_viewer/src/RealTimeDataUpdateSubscriber.cpp $
  * @author:  Anita Lee
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * This class listens for DataPointStateUpdate events and updates the graph for any
  * state changes.
  *
  */

#include <time.h>
#include "app/history/history_viewer/src/stdafx.h"
#include "app/history/history_viewer/src/RealTimeDataUpdateSubscriber.h"
#include "app/history/history_viewer/src/HistoryViewerDlg.h"

//#include "core/message/types/AllDatapoint_MessageTypes.h"
#include "core/message/types/DataPointStateUpdate_MessageTypes.h"
#include "core/message/src/MessageSubscriptionManager.h"

#include "core/utilities/src/DebugUtil.h"

#include "core/threads/src/Thread.h"

#include "bus/scada/DataPointCorbaDef/src/DataPointCorbaDef.h"

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"

#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"

using namespace TA_Core;

namespace TA_App
{

    //
    // RealTimeDataUpdateSubscriber
    //
    RealTimeDataUpdateSubscriber::RealTimeDataUpdateSubscriber(CHistoryViewerDlg* parentDlg, unsigned long datapointKey)
        : m_parentProcess(parentDlg), m_datapointEntitykey(datapointKey), m_isSubscribed(false)
    {
        m_locationKey = 0;
        TA_Core::IEntityData* entityData = NULL;

        try
        {
            // TD 3006: Make sure pointer returned from EntityAccessFactory can be deleted to
            // prevent memory leaks.
            entityData = EntityAccessFactory::getInstance().getEntity(m_datapointEntitykey);
            m_locationKey = entityData->getLocation();
        }
        catch(const DatabaseException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", ex.what());
        }
        catch(const DataException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", ex.what());
        }
        catch(...)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Unknown exception caught while getting location key");
        }

        if(entityData != NULL)
        {
            delete entityData;
        }
    }


    //
    // ~RealTimeDataUpdateSubscriber
    //
    RealTimeDataUpdateSubscriber::~RealTimeDataUpdateSubscriber()
    {
        //
        // Sleep until we have unsubscribed
        //
        while(m_isSubscribed)
        {
            Thread::sleep(100);
        }
    }


    //
    // receiveSpecialisedMessage
    //
    void RealTimeDataUpdateSubscriber::receiveSpecialisedMessage(const CommsMessageCorbaDef& message)
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "receiveSpecialisedMessage" );
        try
        {
            //TA_Bus::DataPointCorbaDef::DataPointState* dpState = NULL;
            TA_Bus::DataPointCorbaDef::ClientDataPointState* dpState = NULL;

            if ( ( message.messageState >>= dpState ) != 0 )
		    {
                unsigned long entityKey = message.assocEntityKey;

                if(entityKey != m_datapointEntitykey)
                {
                    return;
                }

                time_t timestamp;
                unsigned long status;
                double value;

                //switch(dpIntState->_d())
                //{
                //    case TA_Bus::DataPointCorbaDef::dpValueStatus:
                //    {
                        timestamp = dpState->timeStamp;
                        status = dpState->dataPointStatus;

                        switch(dpState->value._d() )
	                    {
                            case TA_Bus::DataPointCorbaDef::dpvFloat:
			                {
				                value = (double) (dpState->value).floatValue();
                                break;
			                }
		                    case TA_Bus::DataPointCorbaDef::dpvBoolean:
			                {
				                if((bool) (dpState->value).booleanValue())
                                {
                                    value = 1;
                                }
                                else
                                {
                                    value = 0;
                                }

                                break;
			                }
		                    case TA_Bus::DataPointCorbaDef::dpvText:
			                {
				                // Shouldn't go in here because we don't trend text datapoints
				                return;
			                }
		                    default:
			                {
				                return;
			                }


	                    }
                    //}
                    //case TA_Bus::DataPointCorbaDef::dpInhibitedStatus:
					//case TA_Bus::DataPointCorbaDef::dpControlStatus:
                    //default:
                    //    break;

                //}

                LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo,
                    "Received DataPointStateUpdate message with entity key: %lu, timestamp: %lu and value: %lf",
                    entityKey, timestamp, value );

                long* entitykey = new long(m_datapointEntitykey);

                CHistoryViewerDlg::DataPointInfo* dataPointInfo = new CHistoryViewerDlg::DataPointInfo;
                dataPointInfo->timestamp = timestamp;
                dataPointInfo->value = value;
                dataPointInfo->status = status;

	            // Tell the app that a datapoint has changed value
                //m_parentProcess->updateRealTimePlot(m_datapointEntitykey, timestamp, value, status);
                ::PostMessage( AfxGetApp()->GetMainWnd()->m_hWnd, WM_HISTORY_UPDATE, reinterpret_cast<WPARAM>(entitykey), reinterpret_cast<LPARAM>(dataPointInfo) );
            }
        }
        catch (...)
        {
	        LOG(SourceInfo, DebugUtil::GenericLog,  DebugUtil::DebugFatal, "Exception thrown while receiving event");

            // ANITA: Redo
            //GenericGUI::CGenericGUIManagedProcess::GetInstance()->UpdateStatus(nexus::ProcessManagerDmIf::DependencyFail);
        }

        LOG ( SourceInfo, DebugUtil::FunctionExit, "receiveSpecialisedMessage" );
    }


    //
    // subscribeToMessage
    //
    void RealTimeDataUpdateSubscriber::subscribeToMessage()
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, "subscribeToMessage");

        if(!m_isSubscribed)
        {
            //
		    // Create the message subscriber and subscribe the subscriber with the
            // MessageSubscriptionManager by message context
		    //
            MessageSubscriptionManager::getInstance().subscribeToCommsMessage(
                TA_Core::DataPointStateUpdate::DataPointValueStatusUpdate,
			    this, m_datapointEntitykey, 0, m_locationKey, NULL);
            m_isSubscribed = true;
        }

        LOG(SourceInfo, DebugUtil::FunctionExit, "subscribeToMessage");
    }


    //
    // unsubscribeToMessage
    //
    void RealTimeDataUpdateSubscriber::unsubscribeToMessage()
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, "unsubscribeToMessage");

        if(m_isSubscribed)
        {
            MessageSubscriptionManager::getInstance().unsubscribeToMessages(
			    this);
            m_isSubscribed = false;
        }

        LOG(SourceInfo, DebugUtil::FunctionExit, "unsubscribeToMessage");
    }


}   // TA_History

#endif //REAL_TIME_DATA_UPDATE_SUBSCRIBER_CPP