/**
  * The source code in this file is the property of 
  * Ripple Systems Pty Ltd and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/tis_agent/src/RATISMessageData.cpp $
  * @author Robin Ashcroft
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This class contains the database retrievals and writes for incoming RATIS messages
  * for the Traveller Information System (TIS) Agent
  * It implements the IRATISMessage interface.
  */

#pragma warning( disable : 4786 )

#include <string>
#include <sstream>

#include "core/data_access_interface/tis_agent/src/RATISMessageData.h"

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"

#include "core/utilities/src/TAAssert.h"


namespace TA_IRS_Core
{
    const int RATISMessageData::STATION_SIZE = 3;
    const int RATISMessageData::STATION_START_INDEX = 3;
    const int RATISMessageData::ID_SIZE = 3;
    const int RATISMessageData::STATION_END_INDEX = STATION_START_INDEX + STATION_SIZE;
    const int RATISMessageData::NUM_ID_SIZE = 3;
    const int RATISMessageData::NUM_ID_END = STATION_END_INDEX + NUM_ID_SIZE;
    const int RATISMessageData::STIS_MESSAGE_MAX_LENGTH = 256;
    const int RATISMessageData::TTIS_MESSAGE_MAX_LENGTH = 124;

	RATISMessageData::RATISMessageData()
	{
		m_ratisMessageHelper = new RATISMessageHelper();

	}
    //
    // DisplayRequestData
    //
    RATISMessageData::RATISMessageData( unsigned long pKey )
    {
        m_ratisMessageHelper = new RATISMessageHelper( pKey );
    }  

	RATISMessageData::~RATISMessageData()
	{
		if( m_ratisMessageHelper != NULL )
		{
			delete m_ratisMessageHelper;
			m_ratisMessageHelper = NULL;
		}

	}

    unsigned long RATISMessageData::getKey()
	{
		TA_ASSERT(m_ratisMessageHelper != NULL,"The RATISMessageHelper pointer is null.");

		return m_ratisMessageHelper->getKey();
	}

	long RATISMessageData::getSessionRef()
	{
		TA_ASSERT(m_ratisMessageHelper != NULL,"The RATISMessageHelper pointer is null.");

		return m_ratisMessageHelper->getSessionRef();
	}

	std::string RATISMessageData::getStartTime()
	{
		TA_ASSERT(m_ratisMessageHelper != NULL,"The RATISMessageHelper pointer is null.");

		return m_ratisMessageHelper->getStartTime();
	}

	std::string RATISMessageData::getEndTime()
	{
		TA_ASSERT(m_ratisMessageHelper != NULL,"The RATISMessageHelper pointer is null.");

		return m_ratisMessageHelper->getEndTime();
	}

	bool RATISMessageData::getRequiresVetting()
	{
		TA_ASSERT(m_ratisMessageHelper != NULL,"The RATISMessageHelper pointer is null.");

		return m_ratisMessageHelper->getRequiresVetting();
	}

	bool RATISMessageData::getOverridable()
	{
		TA_ASSERT(m_ratisMessageHelper != NULL,"The RATISMessageHelper pointer is null.");

		return m_ratisMessageHelper->getOverridable();

	}

	TA_Base_Core::ERATISMessageType RATISMessageData::getType()
	{
		TA_ASSERT(m_ratisMessageHelper != NULL,"The RATISMessageHelper pointer is null.");

		return m_ratisMessageHelper->getType();
	}

	TA_Base_Core::ERATISMessageStatus RATISMessageData::getStatus()
	{
		TA_ASSERT(m_ratisMessageHelper != NULL,"The RATISMessageHelper pointer is null.");

		return m_ratisMessageHelper->getStatus();
	}

	long RATISMessageData::getCreationDate()
	{
		TA_ASSERT(m_ratisMessageHelper != NULL,"The RATISMessageHelper pointer is null.");

		return m_ratisMessageHelper->getCreationDate();
	}

	std::string RATISMessageData::getMessageText()
	{
		TA_ASSERT(m_ratisMessageHelper != NULL,"The RATISMessageHelper pointer is null.");

		return m_ratisMessageHelper->getMessageText();
	}

	int RATISMessageData::getPriority()
	{
		TA_ASSERT(m_ratisMessageHelper != NULL,"The RATISMessageHelper pointer is null.");

		return m_ratisMessageHelper->getPriority();
	}

	std::string RATISMessageData::getTag()
	{
		TA_ASSERT(m_ratisMessageHelper != NULL,"The RATISMessageHelper pointer is null.");

		return m_ratisMessageHelper->getTag();
	}


	std::string RATISMessageData::getDestination()
	{
		TA_ASSERT(m_ratisMessageHelper != NULL,"The RATISMessageHelper pointer is null.");

		return m_ratisMessageHelper->getDestination();
	}

    bool RATISMessageData::isForTTIS()
	{
        std::string station = getDestination();
        TA_ASSERT(!station.empty(), "Destination has to be set before using this method");

        // the first 3 is the system Id, then the next 3 is the destination, followed by the PIDs
        station = station.substr(STATION_START_INDEX, STATION_SIZE);

        if ((station.compare("ATR") == 0) || (station.compare("TRN") == 0))
        {
            return true;
        }

        return false;
	}

    bool RATISMessageData::isForAllTrains()
	{
        std::string station = getDestination();
        TA_ASSERT(!station.empty(), "Destination has to be set before using this method");

        // the first 3 is the system Id, then the next 3 is the destination, followed by the PIDs
        station = station.substr(3, 3);
        return (station.compare("ATR") == 0);
	}

    std::vector<unsigned char> RATISMessageData::getDestinationIds()
	{
        std::string destination = getDestination();
        TA_ASSERT(!destination.empty(), "Destination has to be set before using this method");

        // the first 3 is the system Id, then the next 3 is the destination, followed by the PIDs
        int numIds = atol (destination.substr(STATION_END_INDEX, NUM_ID_SIZE).c_str());
        std::vector<unsigned char> ids;
        for (int i = 0; i < numIds; i++)
        {
            ids.push_back((unsigned char)atol(destination.substr(NUM_ID_END + i * ID_SIZE, ID_SIZE).c_str()));
        }
        return ids;
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	//
	// SET methods 
	//
	//
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void RATISMessageData::setSessionRef( long sessionRef )
	{
		TA_ASSERT(m_ratisMessageHelper != NULL,"The RATISMessageHelper pointer is null.");

		m_ratisMessageHelper->setSessionRef( sessionRef );
	}

	void RATISMessageData::setStartTime( const std::string& startTime )
	{
		TA_ASSERT(m_ratisMessageHelper != NULL,"The RATISMessageHelper pointer is null.");

		m_ratisMessageHelper->setStartTime( startTime );
	}

	void RATISMessageData::setEndTime( const std::string& endTime )
	{
		TA_ASSERT(m_ratisMessageHelper != NULL,"The RATISMessageHelper pointer is null.");

		m_ratisMessageHelper->setEndTime( endTime );
	}

	void RATISMessageData::setRequiresVetting( bool requiresVetting )
	{
		TA_ASSERT(m_ratisMessageHelper != NULL,"The RATISMessageHelper pointer is null.");

		m_ratisMessageHelper->setRequiresVetting( requiresVetting );
	}

	void RATISMessageData::setOverridable( bool overridable )
	{
		TA_ASSERT(m_ratisMessageHelper != NULL,"The RATISMessageHelper pointer is null.");

		m_ratisMessageHelper->setOverridable( overridable );

	}

	void RATISMessageData::setType( TA_Base_Core::ERATISMessageType type )
	{
		TA_ASSERT(m_ratisMessageHelper != NULL,"The RATISMessageHelper pointer is null.");

		m_ratisMessageHelper->setType( type );
	}

	void RATISMessageData::setStatus( TA_Base_Core::ERATISMessageStatus status )
	{
		TA_ASSERT(m_ratisMessageHelper != NULL,"The RATISMessageHelper pointer is null.");

		m_ratisMessageHelper->setStatus( status );
	}

	void RATISMessageData::setCreationDate( long creationDate )
	{
		TA_ASSERT(m_ratisMessageHelper != NULL,"The RATISMessageHelper pointer is null.");

		m_ratisMessageHelper->setCreationDate( creationDate );
	}

	void RATISMessageData::setMessageText( const std::string& messageText )
	{
		TA_ASSERT(m_ratisMessageHelper != NULL,"The RATISMessageHelper pointer is null.");

        std::string message(messageText);
        if (isForTTIS())
        {
            if (messageText.size() > TTIS_MESSAGE_MAX_LENGTH)
            {
                message = messageText.substr(0, TTIS_MESSAGE_MAX_LENGTH);
            }
        }
        else
        {
            if (messageText.size() > STIS_MESSAGE_MAX_LENGTH)
            {
                message = messageText.substr(0, STIS_MESSAGE_MAX_LENGTH);
            }
        }                
        m_ratisMessageHelper->setMessageText(message);
	}

	void RATISMessageData::setPriority( int priority )
	{
		TA_ASSERT(m_ratisMessageHelper != NULL,"The RATISMessageHelper pointer is null.");

		m_ratisMessageHelper->setPriority( priority );
	}

	void RATISMessageData::setTag( const std::string& tag )
	{
		TA_ASSERT(m_ratisMessageHelper != NULL,"The RATISMessageHelper pointer is null.");

		m_ratisMessageHelper->setTag( tag );
	}


	void RATISMessageData::setDestination( const std::string& destination )
	{
		TA_ASSERT(m_ratisMessageHelper != NULL,"The RATISMessageHelper pointer is null.");

		m_ratisMessageHelper->setDestination( destination );
	}


	bool RATISMessageData::isNew()
	{
		TA_ASSERT(m_ratisMessageHelper != NULL,"The RATISMessageHelper pointer is null.");

		return m_ratisMessageHelper->isNew();
	}

	void RATISMessageData::invalidate()
	{
        TA_ASSERT(m_ratisMessageHelper != NULL,"The RATISMessageHelper pointer is null.");
        m_ratisMessageHelper->invalidate();
	}

	void RATISMessageData::applyChanges()
	{
		TA_ASSERT(m_ratisMessageHelper != NULL,"The RATISMessageHelper pointer is null.");

		m_ratisMessageHelper->writeRATISMessageData();
	}

	void RATISMessageData::deleteThisRATISMessage()
	{
		TA_ASSERT(m_ratisMessageHelper != NULL,"The RATISMessageHelper pointer is null.");

		m_ratisMessageHelper->deleteThisRATISMessage();
    }
} //end namespace TA_IRS_Core
