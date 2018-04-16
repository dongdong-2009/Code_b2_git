/**
  * The source code in this file is the property of
  * Ripple Systems Pty Ltd and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/tis_agent/src/DisplayRequestData.cpp $
  * @author Robin Ashcroft
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This class contains the database retrievals and writes for Display Requests
  * for the Traveller Information System (TIS) Agent
  * It implements the IDisplayRequest interface.
  */

#pragma warning( disable : 4786 )

#include <string>
#include <sstream>

#include "core/data_access_interface/tis_agent/src/DisplayRequestData.h"

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"

#include "core/utilities/src/TAAssert.h"


namespace TA_IRS_Core
{

    DisplayRequestData::DisplayRequestData()
    {
        FUNCTION_ENTRY( "DisplayRequestData" );

        m_displayRequestHelper = new DisplayRequestHelper();

        FUNCTION_EXIT;
    }


    // 
    // DisplayRequestData
    // 
    DisplayRequestData::DisplayRequestData( std::string pKey )
    {
        FUNCTION_ENTRY( "DisplayRequestData" );

        m_displayRequestHelper = new DisplayRequestHelper( pKey );

        FUNCTION_EXIT;
    }


    DisplayRequestData::~DisplayRequestData()
    {
        FUNCTION_ENTRY( "~DisplayRequestData" );

        if( m_displayRequestHelper != NULL )
        {
            delete m_displayRequestHelper;
            m_displayRequestHelper = NULL;
        }

        FUNCTION_EXIT;
    }


    std::string DisplayRequestData::getKey()
    {
        FUNCTION_ENTRY( "getKey" );

        TA_ASSERT(m_displayRequestHelper != NULL,"The DisplayRequestHelper pointer is null.");

        FUNCTION_EXIT;
        return m_displayRequestHelper->getKey();
    }


    std::string DisplayRequestData::getTimestamp()
    {
        FUNCTION_ENTRY( "getTimestamp" );

        TA_ASSERT(m_displayRequestHelper != NULL,"The DisplayRequestHelper pointer is null.");

        FUNCTION_EXIT;
        return m_displayRequestHelper->getTimestamp();
    }


    std::string DisplayRequestData::getDestinationSystem()
    {
        FUNCTION_ENTRY( "getDestinationSystem" );

        TA_ASSERT(m_displayRequestHelper != NULL,"The DisplayRequestHelper pointer is null.");

        FUNCTION_EXIT;
        return m_displayRequestHelper->getDestinationSystem();
    }


    std::string DisplayRequestData::getSubmittedBy()
    {
        FUNCTION_ENTRY( "getSubmittedBy" );

        TA_ASSERT(m_displayRequestHelper != NULL,"The DisplayRequestHelper pointer is null.");

        FUNCTION_EXIT;
        return m_displayRequestHelper->getSubmittedBy();

    }


    int DisplayRequestData::getMessageType()
    {
        FUNCTION_ENTRY( "getMessageType" );

        TA_ASSERT(m_displayRequestHelper != NULL,"The DisplayRequestHelper pointer is null.");

        FUNCTION_EXIT;
        return m_displayRequestHelper->getMessageType();
    }


    int DisplayRequestData::getMessageTag()
    {
        FUNCTION_ENTRY( "getMessageTag" );

        TA_ASSERT(m_displayRequestHelper != NULL,"The DisplayRequestHelper pointer is null.");

        FUNCTION_EXIT;
        return m_displayRequestHelper->getMessageTag();
    }


    int DisplayRequestData::getLibrarySection()
    {
        FUNCTION_ENTRY( "getLibrarySection" );

        TA_ASSERT(m_displayRequestHelper != NULL,"The DisplayRequestHelper pointer is null.");

        FUNCTION_EXIT;
        return m_displayRequestHelper->getLibrarySection();
    }


    unsigned short DisplayRequestData::getLibraryVersion()
    {
        FUNCTION_ENTRY( "getLibraryVersion" );

        TA_ASSERT(m_displayRequestHelper != NULL,"The DisplayRequestHelper pointer is null.");

        FUNCTION_EXIT;
        return m_displayRequestHelper->getLibraryVersion();
    }


    std::string DisplayRequestData::getAdhocMessageContent()
    {
        FUNCTION_ENTRY( "getAdhocMessageContent" );

        TA_ASSERT(m_displayRequestHelper != NULL,"The DisplayRequestHelper pointer is null.");

        FUNCTION_EXIT;
        return m_displayRequestHelper->getAdhocMessageContent();
    }


    std::string DisplayRequestData::getStartTime()
    {
        FUNCTION_ENTRY( "getStartTime" );

        TA_ASSERT(m_displayRequestHelper != NULL,"The DisplayRequestHelper pointer is null.");

        FUNCTION_EXIT;
        return m_displayRequestHelper->getStartTime();
    }


    std::string DisplayRequestData::getEndTime()
    {
        FUNCTION_ENTRY( "getEndTime" );

        TA_ASSERT(m_displayRequestHelper != NULL,"The DisplayRequestHelper pointer is null.");

        FUNCTION_EXIT;
        return m_displayRequestHelper->getEndTime();
    }


    bool DisplayRequestData::getContinuous()
    {
        FUNCTION_ENTRY( "getContinuous" );

        TA_ASSERT(m_displayRequestHelper != NULL,"The DisplayRequestHelper pointer is null.");

        FUNCTION_EXIT;
        return m_displayRequestHelper->getContinuous();
    }


    int DisplayRequestData::getPriority()
    {
        FUNCTION_ENTRY( "getPriority" );

        TA_ASSERT(m_displayRequestHelper != NULL,"The DisplayRequestHelper pointer is null.");

        FUNCTION_EXIT;
        return m_displayRequestHelper->getPriority();
    }


    std::string DisplayRequestData::getRatisTag()
    {
        FUNCTION_ENTRY( "getRatisTag" );

        TA_ASSERT(m_displayRequestHelper != NULL,"The DisplayRequestHelper pointer is null.");

        FUNCTION_EXIT;
        return m_displayRequestHelper->getRatisTag();
    }


    std::string DisplayRequestData::getRatisDestination()
    {
        FUNCTION_ENTRY( "getRatisDestination" );

        TA_ASSERT(m_displayRequestHelper != NULL,"The DisplayRequestHelper pointer is null.");

        FUNCTION_EXIT;
        return m_displayRequestHelper->getRatisDestination();
    }


    bool DisplayRequestData::getRatisOverridable()
    {
        FUNCTION_ENTRY( "getRatisOverridable" );

        TA_ASSERT(m_displayRequestHelper != NULL,"The DisplayRequestHelper pointer is null.");

        FUNCTION_EXIT;
        return m_displayRequestHelper->getRatisOverridable();
    }


    std::string DisplayRequestData::getStisDestinationStation()
    {
        FUNCTION_ENTRY( "getStisDestinationStation" );

        TA_ASSERT(m_displayRequestHelper != NULL,"The DisplayRequestHelper pointer is null.");

        FUNCTION_EXIT;
        return m_displayRequestHelper->getStisDestinationStation();
    }


    std::string DisplayRequestData::getStisDestinationLevels()
    {
        FUNCTION_ENTRY( "getStisDestinationLevels" );

        TA_ASSERT(m_displayRequestHelper != NULL,"The DisplayRequestHelper pointer is null.");

        FUNCTION_EXIT;
        return m_displayRequestHelper->getStisDestinationLevels();
    }


    std::string DisplayRequestData::getStisDestinationPids()
    {
        FUNCTION_ENTRY( "getStisDestinationPids" );

        TA_ASSERT(m_displayRequestHelper != NULL,"The DisplayRequestHelper pointer is null.");

        FUNCTION_EXIT;
        return m_displayRequestHelper->getStisDestinationPids();
    }


    std::string DisplayRequestData::getTtisDestinationTrainId()
    {
        FUNCTION_ENTRY( "getTtisDestinationTrainId" );

        TA_ASSERT(m_displayRequestHelper != NULL,"The DisplayRequestHelper pointer is null.");

        FUNCTION_EXIT;
        return m_displayRequestHelper->getTtisDestinationTrainId();
    }


    std::string DisplayRequestData::getTtisDestinationPids()
    {
        FUNCTION_ENTRY( "getTtisDestinationPids" );

        TA_ASSERT(m_displayRequestHelper != NULL,"The DisplayRequestHelper pointer is null.");

        FUNCTION_EXIT;
        return m_displayRequestHelper->getTtisDestinationPids();
    }


    TA_Base_Core::DisplayAttributes DisplayRequestData::getDisplayAttributes()
    {
        FUNCTION_ENTRY( "getDisplayAttributes" );

        TA_ASSERT(m_displayRequestHelper != NULL,"The DisplayRequestHelper pointer is null.");

        FUNCTION_EXIT;
        return m_displayRequestHelper->getDisplayAttributes();
    }


    TA_Base_Core::PlasmaAttributes DisplayRequestData::getPlasmaAttributes()
    {
        FUNCTION_ENTRY( "getPlasmaAttributes" );

        TA_ASSERT(m_displayRequestHelper != NULL,"The DisplayRequestHelper pointer is null.");

        FUNCTION_EXIT;
        return m_displayRequestHelper->getPlasmaAttributes();
    }


    TA_Base_Core::LEDAttributes DisplayRequestData::getLEDAttributes()
    {
        FUNCTION_ENTRY( "getLEDAttributes" );

        TA_ASSERT(m_displayRequestHelper != NULL,"The DisplayRequestHelper pointer is null.");

        FUNCTION_EXIT;
        return m_displayRequestHelper->getLEDAttributes();
    }


    time_t DisplayRequestData::getCreatedTime()
    {
        FUNCTION_ENTRY( "getCreatedTime" );

        TA_ASSERT(m_displayRequestHelper != NULL,"The DisplayRequestHelper pointer is null.");

        FUNCTION_EXIT;
        return m_displayRequestHelper->getCreatedTime();
    }


    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 
    // 
    // SET methods
    // 
    // 
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void DisplayRequestData::setTimestamp( const std::string& timestamp )
    {
        FUNCTION_ENTRY( "setTimestamp" );

        TA_ASSERT(m_displayRequestHelper != NULL,"The DisplayRequestHelper pointer is null.");

        m_displayRequestHelper->setTimestamp( timestamp );

        FUNCTION_EXIT;
    }


    void DisplayRequestData::setDestinationSystem( const std::string& destinationSystem )
    {
        FUNCTION_ENTRY( "setDestinationSystem" );

        TA_ASSERT(m_displayRequestHelper != NULL,"The DisplayRequestHelper pointer is null.");

        m_displayRequestHelper->setDestinationSystem( destinationSystem );

        FUNCTION_EXIT;
    }


    void DisplayRequestData::setSubmittedBy( const std::string& submittedBy )
    {
        FUNCTION_ENTRY( "setSubmittedBy" );

        TA_ASSERT(m_displayRequestHelper != NULL,"The DisplayRequestHelper pointer is null.");

        m_displayRequestHelper->setSubmittedBy( submittedBy );

        FUNCTION_EXIT;
    }


    void DisplayRequestData::setMessageType( int messageType )
    {
        FUNCTION_ENTRY( "setMessageType" );

        TA_ASSERT(m_displayRequestHelper != NULL,"The DisplayRequestHelper pointer is null.");

        m_displayRequestHelper->setMessageType( messageType );

        FUNCTION_EXIT;
    }


    void DisplayRequestData::setMessageTag( int messageTag )
    {
        FUNCTION_ENTRY( "setMessageTag" );

        TA_ASSERT(m_displayRequestHelper != NULL,"The DisplayRequestHelper pointer is null.");

        m_displayRequestHelper->setMessageTag( messageTag );

        FUNCTION_EXIT;
    }


    void DisplayRequestData::setLibrarySection( int librarySection )
    {
        FUNCTION_ENTRY( "setLibrarySection" );

        TA_ASSERT(m_displayRequestHelper != NULL,"The DisplayRequestHelper pointer is null.");

        m_displayRequestHelper->setLibrarySection( librarySection );

        FUNCTION_EXIT;
    }


    void DisplayRequestData::setLibraryVersion( unsigned short libraryVersion )
    {
        FUNCTION_ENTRY( "setLibraryVersion" );

        TA_ASSERT(m_displayRequestHelper != NULL,"The DisplayRequestHelper pointer is null.");

        m_displayRequestHelper->setLibraryVersion( libraryVersion );

        FUNCTION_EXIT;
    }


    void DisplayRequestData::setAdhocMessageContent( const std::string& adhocMessageContent )
    {
        FUNCTION_ENTRY( "setAdhocMessageContent" );

        TA_ASSERT(m_displayRequestHelper != NULL,"The DisplayRequestHelper pointer is null.");

        m_displayRequestHelper->setAdhocMessageContent( adhocMessageContent );

        FUNCTION_EXIT;
    }


    void DisplayRequestData::setStartTime( const std::string& startTime )
    {
        FUNCTION_ENTRY( "setStartTime" );

        TA_ASSERT(m_displayRequestHelper != NULL,"The DisplayRequestHelper pointer is null.");

        m_displayRequestHelper->setStartTime( startTime );

        FUNCTION_EXIT;
    }


    void DisplayRequestData::setEndTime( const std::string& endTime )
    {
        FUNCTION_ENTRY( "setEndTime" );

        TA_ASSERT(m_displayRequestHelper != NULL,"The DisplayRequestHelper pointer is null.");

        m_displayRequestHelper->setEndTime( endTime );

        FUNCTION_EXIT;
    }


    void DisplayRequestData::setContinuous( bool continuous )
    {
        FUNCTION_ENTRY( "setContinuous" );

        TA_ASSERT(m_displayRequestHelper != NULL,"The DisplayRequestHelper pointer is null.");

        m_displayRequestHelper->setContinuous( continuous );

        FUNCTION_EXIT;
    }


    void DisplayRequestData::setPriority( int priority )
    {
        FUNCTION_ENTRY( "setPriority" );

        TA_ASSERT(m_displayRequestHelper != NULL,"The DisplayRequestHelper pointer is null.");

        m_displayRequestHelper->setPriority( priority );

        FUNCTION_EXIT;
    }


    void DisplayRequestData::setRatisTag( const std::string& ratisTag )
    {
        FUNCTION_ENTRY( "setRatisTag" );

        TA_ASSERT(m_displayRequestHelper != NULL,"The DisplayRequestHelper pointer is null.");

        m_displayRequestHelper->setRatisTag( ratisTag );

        FUNCTION_EXIT;
    }


    void DisplayRequestData::setRatisDestination( const std::string& destination )
    {
        FUNCTION_ENTRY( "setRatisDestination" );

        TA_ASSERT(m_displayRequestHelper != NULL,"The DisplayRequestHelper pointer is null.");

        m_displayRequestHelper->setRatisDestination( destination );

        FUNCTION_EXIT;
    }


    void DisplayRequestData::setRatisOverridable( bool overridable )
    {
        FUNCTION_ENTRY( "setRatisOverridable" );

        TA_ASSERT(m_displayRequestHelper != NULL,"The DisplayRequestHelper pointer is null.");

        m_displayRequestHelper->setRatisOverridable( overridable );

        FUNCTION_EXIT;
    }


    void DisplayRequestData::setStisDestinationStation( const std::string& stisDestinationStation )
    {
        FUNCTION_ENTRY( "setStisDestinationStation" );

        TA_ASSERT(m_displayRequestHelper != NULL,"The DisplayRequestHelper pointer is null.");

        m_displayRequestHelper->setStisDestinationStation( stisDestinationStation );

        FUNCTION_EXIT;
    }


    void DisplayRequestData::setStisDestinationLevels( const std::string& stisDestinationLevels )
    {
        FUNCTION_ENTRY( "setStisDestinationLevels" );

        TA_ASSERT(m_displayRequestHelper != NULL,"The DisplayRequestHelper pointer is null.");

        m_displayRequestHelper->setStisDestinationLevels( stisDestinationLevels );

        FUNCTION_EXIT;
    }


    void DisplayRequestData::setStisDestinationPids( const std::string& stisDestinationPids )
    {
        FUNCTION_ENTRY( "setStisDestinationPids" );

        TA_ASSERT(m_displayRequestHelper != NULL,"The DisplayRequestHelper pointer is null.");

        m_displayRequestHelper->setStisDestinationPids( stisDestinationPids );

        FUNCTION_EXIT;
    }


    void DisplayRequestData::setTtisDestinationTrainId( const std::string& ttisDestinationTrainId )
    {
        FUNCTION_ENTRY( "setTtisDestinationTrainId" );

        TA_ASSERT(m_displayRequestHelper != NULL,"The DisplayRequestHelper pointer is null.");

        m_displayRequestHelper->setTtisDestinationTrainId( ttisDestinationTrainId );

        FUNCTION_EXIT;
    }


    void DisplayRequestData::setTtisDestinationPids( const std::string& ttisDestinationPids )
    {
        FUNCTION_ENTRY( "setTtisDestinationPids" );

        TA_ASSERT(m_displayRequestHelper != NULL,"The DisplayRequestHelper pointer is null.");

        m_displayRequestHelper->setTtisDestinationPids( ttisDestinationPids );

        FUNCTION_EXIT;
    }


    void DisplayRequestData::setDisplayAttributes( TA_Base_Core::DisplayAttributes attributes )
    {
        FUNCTION_ENTRY( "setDisplayAttributes" );

        TA_ASSERT(m_displayRequestHelper != NULL,"The DisplayRequestHelper pointer is null.");

        m_displayRequestHelper->setDisplayAttributes( attributes );

        FUNCTION_EXIT;
    }


    void DisplayRequestData::setPlasmaAttributes( TA_Base_Core::PlasmaAttributes attributes )
    {
        FUNCTION_ENTRY( "setPlasmaAttributes" );

        TA_ASSERT(m_displayRequestHelper != NULL,"The DisplayRequestHelper pointer is null.");

        m_displayRequestHelper->setPlasmaAttributes( attributes );

        FUNCTION_EXIT;
    }


    void DisplayRequestData::setLEDAttributes( TA_Base_Core::LEDAttributes attributes )
    {
        FUNCTION_ENTRY( "setLEDAttributes" );

        TA_ASSERT(m_displayRequestHelper != NULL,"The DisplayRequestHelper pointer is null.");

        m_displayRequestHelper->setLEDAttributes( attributes );

        FUNCTION_EXIT;
    }


    bool DisplayRequestData::isNew()
    {
        FUNCTION_ENTRY( "isNew" );

        TA_ASSERT(m_displayRequestHelper != NULL,"The DisplayRequestHelper pointer is null.");

        FUNCTION_EXIT;
        return m_displayRequestHelper->isNew();
    }


    void DisplayRequestData::invalidate()
    {
        FUNCTION_ENTRY( "invalidate" );

        TA_ASSERT(m_displayRequestHelper != NULL,"The DisplayRequestHelper pointer is null.");
        m_displayRequestHelper->invalidate();

        FUNCTION_EXIT;
    }


    void DisplayRequestData::applyChanges()
    {
        FUNCTION_ENTRY( "applyChanges" );

        TA_ASSERT(m_displayRequestHelper != NULL,"The DisplayRequestHelper pointer is null.");

        m_displayRequestHelper->writeDisplayRequestData();

        FUNCTION_EXIT;
    }


    // Helper methods

    std::string DisplayRequestData::timestampToString(const std::string& timestamp)
    {
        FUNCTION_ENTRY( "timestampToString" );

        std::string formattedTime;

        if (timestamp.length() == 14)
        {
            // characters 6 and 7 are the date
            formattedTime += timestamp.substr(6,2);

            // characters 4 and 5 are the month
            std::stringstream temp;
            temp << timestamp.substr(4,2);
            int month = 1;
            temp >> month;
            switch (month)
            {
            case 1:
                formattedTime += " Jan ";
                break;
            case 2:
                formattedTime += " Feb ";
                break;
            case 3:
                formattedTime += " Mar ";
                break;
            case 4:
                formattedTime += " Apr ";
                break;
            case 5:
                formattedTime += " May ";
                break;
            case 6:
                formattedTime += " Jun ";
                break;
            case 7:
                formattedTime += " Jul ";
                break;
            case 8:
                formattedTime += " Aug ";
                break;
            case 9:
                formattedTime += " Sep ";
                break;
            case 10:
                formattedTime += " Oct ";
                break;
            case 11:
                formattedTime += " Nov ";
                break;
            case 12:
                formattedTime += " Dec ";
                break;
            }

            // first 4 digits are the year (0-3)
            formattedTime += timestamp.substr(0,4);
            formattedTime += ' ';

            // characters 8 and 9 are the hour
            formattedTime += timestamp.substr(8,2);
            formattedTime += ':';

            // characters 10 and 11 are the minute
            formattedTime += timestamp.substr(10,2);
            formattedTime += ':';

            // characters 12 and 13 are the seconds
            formattedTime += timestamp.substr(12,2);
        }
        else
        {
            formattedTime = timestamp;
        }

        FUNCTION_EXIT;
        return formattedTime;
    }


    std::string DisplayRequestData::startEndTimeToString(const std::string& time)
    {
        FUNCTION_ENTRY( "startEndTimeToString" );

        // YYYYMMDDHHMMSS
        if (time.length() == 14)
        {
            FUNCTION_EXIT;
            return timestampToString(time);
        }
        // HHMM
        else if (time.length() == 4)
        {
            // insert a ':' between HH and MM
            std::string newTime(time);
            newTime.insert(2, ":");

            FUNCTION_EXIT;
            return newTime;
        }
        // unknown
        else
        {
            FUNCTION_EXIT;
            return time;
        }
    }


    std::string DisplayRequestData::messageTypeToString(int messageType, int librarySection)
    {
        FUNCTION_ENTRY( "messageTypeToString" );

        switch (messageType)
        {
        case TA_IRS_Core::IDisplayRequest::PREDEFINED:
            // check whether its normal or emergency
            if (librarySection == 0)
            {
                FUNCTION_EXIT;
                return std::string("Emergency");
            }
            else
            {
                FUNCTION_EXIT;
                return std::string("Normal");
            }
            break;
        case TA_IRS_Core::IDisplayRequest::ADHOC:
            FUNCTION_EXIT;
            return std::string("Ad Hoc");
            break;
        case TA_IRS_Core::IDisplayRequest::RATIS_IN_NEW:
            FUNCTION_EXIT;
            return std::string("RXH-I-New");
            break;
        case TA_IRS_Core::IDisplayRequest::RATIS_OUT_NEW:
            FUNCTION_EXIT;
            return std::string("RXH-O-New");
            break;
        case TA_IRS_Core::IDisplayRequest::RATIS_IN_UPDATE:
            FUNCTION_EXIT;
            return std::string("RXH-I-Update");
            break;
        case TA_IRS_Core::IDisplayRequest::RATIS_OUT_UPDATE:
            FUNCTION_EXIT;
            return std::string("RXH-O-Update");
            break;
        case TA_IRS_Core::IDisplayRequest::RATIS_IN_CLEAR:
            FUNCTION_EXIT;
            return std::string("RXH-I-Clear");
            break;
        case TA_IRS_Core::IDisplayRequest::RATIS_OUT_CLEAR:
            FUNCTION_EXIT;
            return std::string("RXH-O-Clear");
            break;
        default:
            FUNCTION_EXIT;
            return std::string("Unknown");
            break;
        }
    }


    std::string DisplayRequestData::priorityToString(int priority)
    {
        FUNCTION_ENTRY( "priorityToString" );

        std::stringstream result;
        result << priority;

        FUNCTION_EXIT;
        return result.str();
    }


    std::string DisplayRequestData::ratisTagToString(std::string ratisTag)
    {
        FUNCTION_ENTRY( "ratisTagToString" );

        // if there is a ratis tag, return it.
        // if not, return na
        if (ratisTag.size() == 0)
        {
            FUNCTION_EXIT;
            return std::string("N/A");
        }
        else
        {
            FUNCTION_EXIT;
            return ratisTag;
        }
    }


    // hongran++ TD17502
    TA_Base_Core::IfSendSuccess DisplayRequestData::getIfSendSuccess()
    {
        FUNCTION_ENTRY( "getIfSendSuccess" );

        TA_ASSERT(m_displayRequestHelper != NULL,"The DisplayRequestHelper pointer is null.");

        FUNCTION_EXIT;
        return m_displayRequestHelper->getIfSendSuccess();
    }


    void DisplayRequestData::setIfSendSuccess(TA_Base_Core::IfSendSuccess successful)
    {
        FUNCTION_ENTRY( "setIfSendSuccess" );

        TA_ASSERT(m_displayRequestHelper != NULL,"The DisplayRequestHelper pointer is null.");

        m_displayRequestHelper->setIfSendSuccess( successful );

        FUNCTION_EXIT;
    }
    // ++hongran TD17502


    void DisplayRequestData::populateData( boost::shared_ptr< TA_Base_Core::IData > data, unsigned long index )
    {
        FUNCTION_ENTRY( "populateData" );

        m_displayRequestHelper->populateData( data, index );

        FUNCTION_EXIT;
    }


} //end namespace TA_IRS_Core
