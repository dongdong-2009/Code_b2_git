/**
  * The source code in this file is the property of 
  * Ripple Systems Pty Ltd and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/tis_agent/src/TrainTimeScheduleEntryData.cpp $
  * @author Adam Radics
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This class contains the database retrievals and writes for Train Time Schedule Entries
  * for the Traveller Information System (TIS) Agent
  * It implements the ITrainTimeScheduleEntry interface.
  */

#pragma warning( disable : 4786 )

#include <string>

#include "core/data_access_interface/tis_agent/src/TrainTimeScheduleEntryData.h"

#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_IRS_Core
{

    //
    // TrainTimeScheduleEntryData
    //
    TrainTimeScheduleEntryData::TrainTimeScheduleEntryData()
    {
        // create a new helper
        m_trainTimeScheduleEntryHelper = new TrainTimeScheduleEntryHelper();
    }


    TrainTimeScheduleEntryData::TrainTimeScheduleEntryData( unsigned long pKey )
    {
        // assign the key to the helper
        m_trainTimeScheduleEntryHelper = new TrainTimeScheduleEntryHelper( pKey );
    }


    TrainTimeScheduleEntryData::TrainTimeScheduleEntryData( const TrainTimeScheduleEntryData& ttseData)
    {
        m_trainTimeScheduleEntryHelper = new TrainTimeScheduleEntryHelper( *ttseData.m_trainTimeScheduleEntryHelper );
    }


    TrainTimeScheduleEntryData::~TrainTimeScheduleEntryData()
    {
        if( m_trainTimeScheduleEntryHelper != NULL )
		{
			delete m_trainTimeScheduleEntryHelper;
			m_trainTimeScheduleEntryHelper = NULL;
		}
    }


    unsigned long TrainTimeScheduleEntryData::getKey()
    {
        TA_ASSERT(m_trainTimeScheduleEntryHelper != NULL,"The TrainTimeScheduleHelper pointer is null.");

		return m_trainTimeScheduleEntryHelper->getKey();
    }


    time_t TrainTimeScheduleEntryData::getStartTime()
    {
        TA_ASSERT(m_trainTimeScheduleEntryHelper != NULL,"The TrainTimeScheduleHelper pointer is null.");

		return m_trainTimeScheduleEntryHelper->getStartTime();
    }


	time_t TrainTimeScheduleEntryData::getEndTime()
    {
        TA_ASSERT(m_trainTimeScheduleEntryHelper != NULL,"The TrainTimeScheduleHelper pointer is null.");

		return m_trainTimeScheduleEntryHelper->getEndTime();
    }


	unsigned short TrainTimeScheduleEntryData::getRepeatInterval()
    {
        TA_ASSERT(m_trainTimeScheduleEntryHelper != NULL,"The TrainTimeScheduleHelper pointer is null.");

		return m_trainTimeScheduleEntryHelper->getRepeatInterval();
    }


    ITrainTimeScheduleEntry::MessageTableType TrainTimeScheduleEntryData::getMessageTable()
    {
        TA_ASSERT(m_trainTimeScheduleEntryHelper != NULL,"The TrainTimeScheduleHelper pointer is null.");

		return m_trainTimeScheduleEntryHelper->getMessageTable();
    }


    unsigned short TrainTimeScheduleEntryData::getMessageId()
    {
        TA_ASSERT(m_trainTimeScheduleEntryHelper != NULL,"The TrainTimeScheduleHelper pointer is null.");

		return m_trainTimeScheduleEntryHelper->getMessageId();
    }


    unsigned short TrainTimeScheduleEntryData::getPriority()
    {
        TA_ASSERT(m_trainTimeScheduleEntryHelper != NULL,"The TrainTimeScheduleHelper pointer is null.");

		return m_trainTimeScheduleEntryHelper->getPriority();
    }


    void TrainTimeScheduleEntryData::invalidate()
    {
        TA_ASSERT(m_trainTimeScheduleEntryHelper != NULL,"The TrainTimeScheduleHelper pointer is null.");

		m_trainTimeScheduleEntryHelper->invalidate();
    }

    void TrainTimeScheduleEntryData::setParentKey(unsigned long parentKey)
    {
        TA_ASSERT(m_trainTimeScheduleEntryHelper != NULL,"The TrainTimeScheduleHelper pointer is null.");

		m_trainTimeScheduleEntryHelper->setParentKey(parentKey);
    }

	void TrainTimeScheduleEntryData::setStartTime(time_t startTime)
    {
        TA_ASSERT(m_trainTimeScheduleEntryHelper != NULL,"The TrainTimeScheduleHelper pointer is null.");

		m_trainTimeScheduleEntryHelper->setStartTime(startTime);
    }


	void TrainTimeScheduleEntryData::setEndTime(time_t endTime)
    {
        TA_ASSERT(m_trainTimeScheduleEntryHelper != NULL,"The TrainTimeScheduleHelper pointer is null.");

		m_trainTimeScheduleEntryHelper->setEndTime(endTime);
    }


	void TrainTimeScheduleEntryData::setRepeatInterval(unsigned short repeatInterval)
    {
        TA_ASSERT(m_trainTimeScheduleEntryHelper != NULL,"The TrainTimeScheduleHelper pointer is null.");

		m_trainTimeScheduleEntryHelper->setRepeatInterval(repeatInterval);
    }


    void TrainTimeScheduleEntryData::setMessageTable(MessageTableType messageTable)
    {
        TA_ASSERT(m_trainTimeScheduleEntryHelper != NULL,"The TrainTimeScheduleHelper pointer is null.");

		m_trainTimeScheduleEntryHelper->setMessageTable(messageTable);
    }


    void TrainTimeScheduleEntryData::setMessageId(unsigned short messageId)
    {
        TA_ASSERT(m_trainTimeScheduleEntryHelper != NULL,"The TrainTimeScheduleHelper pointer is null.");

		m_trainTimeScheduleEntryHelper->setMessageId(messageId);
    }


    void TrainTimeScheduleEntryData::setPriority(unsigned short priority)
    {
        TA_ASSERT(m_trainTimeScheduleEntryHelper != NULL,"The TrainTimeScheduleHelper pointer is null.");

		m_trainTimeScheduleEntryHelper->setPriority(priority);
    }


    bool TrainTimeScheduleEntryData::hasChanged()
    {
        TA_ASSERT(m_trainTimeScheduleEntryHelper != NULL,"The TrainTimeScheduleHelper pointer is null.");

		return m_trainTimeScheduleEntryHelper->hasChanged();
    }


    bool TrainTimeScheduleEntryData::isNew()
    {
        TA_ASSERT(m_trainTimeScheduleEntryHelper != NULL,"The TrainTimeScheduleHelper pointer is null.");

		return m_trainTimeScheduleEntryHelper->isNew();
    }


    void TrainTimeScheduleEntryData::applyChanges()
    {
        TA_ASSERT(m_trainTimeScheduleEntryHelper != NULL,"The TrainTimeScheduleHelper pointer is null.");

		m_trainTimeScheduleEntryHelper->writeTrainTimeSheduleEntryData();
    }


    void TrainTimeScheduleEntryData::deleteThisObject()
    {
        TA_ASSERT(m_trainTimeScheduleEntryHelper != NULL,"The TrainTimeScheduleHelper pointer is null.");

		m_trainTimeScheduleEntryHelper->deleteTrainTimeSheduleEntryData();
    }


} //end namespace TA_IRS_Core
