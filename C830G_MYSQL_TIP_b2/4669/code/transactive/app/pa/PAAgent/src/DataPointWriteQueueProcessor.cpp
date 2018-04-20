/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/4669_T01271350/4669/transactive/app/pa/PAAgent/src/DataPointWriteQueueProcessor.cpp $
  * @author:	HoaVu
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2008/11/28 16:26:01 $
  * Last modified by:	$Author: builder $
  *
  * Description:
  *
  *	The DataPointWriteQueueProcessor object is responsible for processing write data commands to the PA Agent
  *
  */

#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290)
#endif // defined _MSC_VER

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"
#include "app/pa/PAAgent/src/Table570.h"
#include "app/pa/PAAgent/src/CachedConfig.h"
#include "app/pa/PAAgent/src/DataPointWriteQueueProcessor.h"
#include "app/pa/PAAgent/src/CachedConfig.h"

namespace TA_IRS_App
{

DataPointWriteQueueProcessor::DataPointWriteQueueProcessor ( Table570& table570 )
: m_cachedConfigInstance(NULL),
  m_table570( table570 )
{
    m_cachedConfigInstance = CachedConfig::getInstance();
}


DataPointWriteQueueProcessor::~DataPointWriteQueueProcessor()
{
}


void DataPointWriteQueueProcessor::processEvent( TA_Base_Bus::DataPointWriteRequest * newEvent )
{
	// prevent multiple threads attempting to delete/create
	// simultaneously
	TA_Base_Core::ThreadGuard guard( m_lock );

    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo,
        "DataPoint write request found.");

    TA_Base_Bus::DataPoint* dp = newEvent->getDataPoint();

    // We are only expecting a value of false from the Fire Countdown DIO datapoint.

	try
	{
        //TD15174
		if (!CachedConfig::getInstance()->getIsMaster() && !CachedConfig::getInstance()->getIsDepot())
        {
			// Pointer comparison should be okay.
			if (dp == m_cachedConfigInstance->getFireCountdownActiveOutputDP())
			{
				if (true == newEvent->getValue().getBoolean())
				{
					m_table570.resetFireAlarmStatusAndWrite();
				}
			}
        } 
        //TD15174
	}
	catch ( ... )
	{
		// do nothing
	}
}

} // namespace TA_IRS_App

