/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/3001_TIP_NEW/3001/transactive/app/ecs/ECSAgent/src/DataPointWriteQueueProcessor.cpp $
  * @author:	HoaVu
  * @versoin:	$Revisoin: 1.1.2.5 $
  *
  * Last modification:	$DateTime: 2014/06/04 11:12:46 $
  * Last modified by:	$Author: qi.huang $
  *
  * Description:
  *
  *	The DataPointWriteQueueProcessor object is responsible for processing write data commands to the PA Agent
  *
  */

#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290 4503)
#endif // defined _MSC_VER

#include <time.h>
#include "core/utilities/src/DebugUtil.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"
#include "app/ecs/ECSAgent/src/ItaEcsCachedConfig.h"
#include "app/ecs/ECSAgent/src/DataPointWriteQueueProcessor.h"


DataPointWriteQueueProcessor::DataPointWriteQueueProcessor ( ItaEcsCachedConfig& theConfig )
: m_theConfig(theConfig)
{

    m_oiPairs.clear();
}


DataPointWriteQueueProcessor::~DataPointWriteQueueProcessor()
{
	TA_Base_Core::ThreadGuard guard( m_lock );

    m_oiPairs.clear();
}

void DataPointWriteQueueProcessor::registerOiAssociation(TA_Base_Bus::DataPoint* outputDataPoint, TA_Base_Bus::DataPoint* inputDataPoint)
{
	TA_Base_Core::ThreadGuard guard( m_lock );

    m_oiPairs.insert(OiPairMap::value_type(outputDataPoint, inputDataPoint));
}

void DataPointWriteQueueProcessor::deregisterOiAssociation(TA_Base_Bus::DataPoint* outputDataPoint)
{
	TA_Base_Core::ThreadGuard guard( m_lock );

    OiPairMapIt it = m_oiPairs.find(outputDataPoint);

    if (it != m_oiPairs.end())
    {
        m_oiPairs.erase(it);
    }
}

void DataPointWriteQueueProcessor::processEvent( TA_Base_Bus::DataPointWriteRequest * newEvent )
{
    // prevent multiple threads atoutputting to delete/create
	// simultaneously
	TA_Base_Core::ThreadGuard guard( m_lock );

    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo,
        "DataPoint write request found.");

    TA_Base_Bus::DataPoint* outputDataPoint = newEvent->getDataPoint();

    if ( NULL == outputDataPoint )
    {
        return;
    }

    bool bControlSucceed = false;

	try
	{
        // Locate datapoint in OiPairMap
        OiPairMapIt it = m_oiPairs.find(outputDataPoint);

        if (it != m_oiPairs.end())
        {
            // ToDo - rights check

            // found
            TA_Base_Bus::DataPoint* inputDataPoint = it->second;
            TA_Base_Bus::DpValue dpValue = newEvent->getValue();
            // set up DataPointState with the default timestamp and quality
            TA_Base_Bus::DataPointState newState ( dpValue, time( NULL ), TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON );

	        inputDataPoint->updateFieldState( newState );

            bControlSucceed = true;
        }
	}
    catch ( const TA_Base_Core::TransactiveException & te )
	{
        // Not expecting any failures...
        LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", te.what() );
	}
    catch ( const std::exception & e )
	{
        // Not expecting any failures...
        LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", e.what() );
	}
	catch ( ... )
	{
        // Not expecting any failures...
        LOG_EXCEPTION_CATCH( SourceInfo, "Unknown exception", "Unknown reason" );
	}

    outputDataPoint->updateControlState( bControlSucceed ? TA_Base_Bus::success : TA_Base_Bus::failed );
}
