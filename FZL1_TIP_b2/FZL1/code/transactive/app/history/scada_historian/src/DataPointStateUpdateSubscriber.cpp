/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/history/scada_historian/src/DataPointStateUpdateSubscriber.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#include "app/history/scada_historian/src/ScadaHistorianAgentProcess.h"
#include "app/history/scada_historian/src/DataPointStateUpdateSubscriber.h"
#include "core/message/types/DataPointStateUpdate_MessageTypes.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/utilities/src/DebugUtil.h"
#include "bus/scada/DataPointCorbaDef/src/DataPointCorbaDef.h"
#include "core/utilities/src/TAAssert.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"

#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"

using TA_Core::DebugUtil;


namespace TA_App
{
    //
	// DataPointStateUpdateSubscriber
	//
	DataPointStateUpdateSubscriber::DataPointStateUpdateSubscriber(ScadaHistorianAgentProcess* parent, unsigned long entitykey)
		: m_parentProcess(parent),
          m_entitykey(entitykey)
	{
		LOG ( SourceInfo, DebugUtil::FunctionEntry, "DataPointStateUpdateSubscriber" );

        TA_ASSERT(parent != NULL, "Null pointer");

        m_locationkey = 0;

        try
        {
            m_locationkey = EntityAccessFactory::getInstance().getEntity(m_entitykey)->getLocation();
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

		//
		// Create the message subscriber and subscribe the subscriber with the
        // MessageSubscriptionManager by message context
		//
        MessageSubscriptionManager::getInstance().subscribeToCommsMessage(
            TA_Core::DataPointStateUpdate::DataPointValueStatusUpdate,
			this, m_entitykey, 0, m_locationkey, NULL);

		LOG ( SourceInfo, DebugUtil::FunctionExit, "DataPointStateUpdateSubscriber" );
	}


	//
	// ~DataPointStateUpdateSubscriber
	//
	DataPointStateUpdateSubscriber::~DataPointStateUpdateSubscriber()
	{
		LOG ( SourceInfo, DebugUtil::FunctionEntry, "~DataPointStateUpdateSubscriber" );

		MessageSubscriptionManager::getInstance().unsubscribeToMessages(
			this);

		LOG ( SourceInfo, DebugUtil::FunctionExit, "~DataPointStateUpdateSubscriber" );
	}


    void DataPointStateUpdateSubscriber::unsubscribe()
    {
        MessageSubscriptionManager::getInstance().unsubscribeToMessages(
			this);
    }


	//
	// receiveSpecialisedMessage
	//
    void DataPointStateUpdateSubscriber::receiveSpecialisedMessage(const CommsMessageCorbaDef& message)
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "receiveSpecialisedMessage" );

        unsigned long entityKey = message.assocEntityKey;
        if( entityKey != m_entitykey)
        {
            return;
        }

        try
        {
            //TA_Bus::DataPointCorbaDef::DataPointInternalState* dpState = NULL;
            //TA_Bus::DataPointCorbaDef::DataPointState* dpState = NULL;
            TA_Bus::DataPointCorbaDef::ClientDataPointState* dpState = NULL;

		    if ( ( message.messageState >>= dpState ) != 0 )
		    {
                m_parentProcess->processUpdatedDataPoint(entityKey, *dpState);

                //switch(dpState->_d())
                //{
                //    case TA_Bus::DataPointCorbaDef::dpValueStatus:
                //    {
                //        m_parentProcess->processUpdatedDataPoint(entityKey, dpState->valueState());

                        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "Message Processed");
                //        break;
                //    }
                //    case TA_Bus::DataPointCorbaDef::dpInhibitedStatus:
				//	case TA_Bus::DataPointCorbaDef::dpControlStatus:
                //    default:
                //        break;
                //}
            }
            else
            {
                LOG(SourceInfo, DebugUtil::GenericLog,  DebugUtil::DebugDebug, "Event does not contain data point event bundle");
    			return;
            }
	    }
        catch (...)
	    {
		    LOG(SourceInfo, DebugUtil::GenericLog,  DebugUtil::DebugFatal, "Exception thrown while receiving event");
	    }

        LOG ( SourceInfo, DebugUtil::FunctionExit, "receiveSpecialisedMessage" );
    }

} // TA_App


