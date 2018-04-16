/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/tis_agent/src/DisplayRequestHelper.cpp $
  * @author Robin Ashcroft
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  *
  * DisplayRequestHelper is an object that is held by DisplayRequest and ConfigDisplayRequest objects.
  */

#pragma warning(disable:4786)
#pragma warning(disable:4503)  // warning C4503: decorated type length exceeded, type was truncated

#include <vector>
#include <memory>

#include "core/data_access_interface/tis_agent/src/DisplayRequestHelper.h"
#include "core/data_access_interface/tis_agent/src/DisplayRequestAccessFactory.h"
#include "core/data_access_interface/tis_agent/src/DisplayRequestTable.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/exceptions/src/DataConfigurationException.h"
#include "core/data_access_interface/src/DbConnectionStrings.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/uuid/src/TAuuid.h"
#include "core/utilities/src/RunParams.h"
#include "core/synchronisation/src/ThreadGuard.h"


namespace
{
    template< typename T > class TryGet
    {
    public:

        explicit TryGet( boost::shared_ptr< TA_Base_Core::IData >& data, T def = T() ) : m_data( data ), m_default( def ) {};

        long getIntegerData( unsigned long index, const std::string& columnName )
        {
            try
            {
                return m_data->getIntegerData( index, columnName );
            }
            catch(...)
            {
                return m_default;
            }
        }

    private:
        boost::shared_ptr< TA_Base_Core::IData > m_data;
        T m_default;
    };
}


namespace TA_IRS_Core
{

    TA_Base_Core::NonReEntrantThreadLockable DisplayRequestHelper::s_columnNamesLock;
    std::vector< std::string > DisplayRequestHelper::s_columnNames;

    std::string DisplayRequestHelper::s_columnNamesSql;
    TA_Base_Core::NonReEntrantThreadLockable DisplayRequestHelper::s_columnNamesSqlLock;


    DisplayRequestHelper::DisplayRequestHelper()
        : m_isValidData( false ), m_key( "" ), m_isNew( true ), m_hasChanged( false ), m_localDatabase("")
    {
        FUNCTION_ENTRY( "DisplayRequestHelper" );

        m_timestamp = "";
        m_destinationSystem = "";
        m_submittedBy = "";
        m_messageType = 0;
        m_messageTag = 0;
        m_librarySection = 0;
        m_libraryVersion = 0;
        m_adhocMessageContent = "";
        m_startTime = "";
        m_endTime = "";
        m_continuous = 0;
        m_priority = 0;
        m_ratisTag = "";
        m_ratisDestination = "";
        m_ratisOverridable = false;

        m_stisDestinationStation = "";
        m_stisDestinationLevels = "";
        m_stisDestinationPids = "";
        m_ttisDestinationTrainId = "";
        m_ttisDestinationPids = "";

        m_displayAttributes.displayMode = (TA_Base_Core::EDisplayMode)0;
        m_displayAttributes.displayTime = 0;
        m_displayAttributes.justification = (TA_Base_Core::EJustification)0;
        m_displayAttributes.repeatInterval = 0;
        m_displayAttributes.scrollSpeed = (TA_Base_Core::EScrollSpeed)0;

        m_plasmaAttributes.backgroundColour = (TA_Base_Core::EPlasmaColour)0;
        m_plasmaAttributes.fontColour = (TA_Base_Core::EPlasmaColour)0;
        m_plasmaAttributes.fontSize = (TA_Base_Core::EPlasmaFontSize)0;
        m_plasmaAttributes.fontType = (TA_Base_Core::EFontType)0;

        m_ledAttributes.backgroundColour = (TA_Base_Core::ELEDColour)0;
        m_ledAttributes.fontColour = (TA_Base_Core::ELEDColour)0;
        m_ledAttributes.fontSize = (TA_Base_Core::ELEDFontSize)0;
        m_ledAttributes.intensity = (TA_Base_Core::ELEDIntensity)0;

        FUNCTION_EXIT;
    }


    DisplayRequestHelper::DisplayRequestHelper( std::string displayRequestKey )
        : m_isValidData( false ),
          m_key( displayRequestKey ),
          m_isNew( false ),
          m_localDatabase("")
    {
        FUNCTION_ENTRY( "DisplayRequestHelper" );
        FUNCTION_EXIT;
    }


    std::string DisplayRequestHelper::getKey()
    {
        FUNCTION_ENTRY( "getKey" );

        // This method cannot be called until the key has been set.

        TA_ASSERT( "" != m_key, "The display request key has not been initialised." );

        FUNCTION_EXIT;
        return m_key;
    }


    std::string DisplayRequestHelper::getTimestamp()
    {
        FUNCTION_ENTRY( "getTimestamp" );

        reload();

        FUNCTION_EXIT;
        return m_timestamp;
    }


    std::string DisplayRequestHelper::getDestinationSystem()
    {
        FUNCTION_ENTRY( "getDestinationSystem" );

        reload();

        FUNCTION_EXIT;
        return m_destinationSystem;
    }


    std::string DisplayRequestHelper::getSubmittedBy()
    {
        FUNCTION_ENTRY( "getSubmittedBy" );

        reload();

        FUNCTION_EXIT;
        return m_submittedBy;

    }


    int DisplayRequestHelper::getMessageType()
    {
        FUNCTION_ENTRY( "getMessageType" );

        reload();

        FUNCTION_EXIT;
        return m_messageType;
    }


    int DisplayRequestHelper::getMessageTag()
    {
        FUNCTION_ENTRY( "getMessageTag" );

        reload();

        FUNCTION_EXIT;
        return m_messageTag;
    }


    int DisplayRequestHelper::getLibrarySection()
    {
        FUNCTION_ENTRY( "getLibrarySection" );

        reload();

        FUNCTION_EXIT;
        return m_librarySection;
    }


    unsigned short DisplayRequestHelper::getLibraryVersion()
    {
        FUNCTION_ENTRY( "getLibraryVersion" );

        reload();

        FUNCTION_EXIT;
        return m_libraryVersion;
    }


    std::string DisplayRequestHelper::getAdhocMessageContent()
    {
        FUNCTION_ENTRY( "getAdhocMessageContent" );

        reload();

        FUNCTION_EXIT;
        return m_adhocMessageContent;
    }


    std::string DisplayRequestHelper::getStartTime()
    {
        FUNCTION_ENTRY( "getStartTime" );

        reload();

        FUNCTION_EXIT;
        return m_startTime;
    }


    std::string DisplayRequestHelper::getEndTime()
    {
        FUNCTION_ENTRY( "getEndTime" );

        reload();

        FUNCTION_EXIT;
        return m_endTime;
    }


    bool DisplayRequestHelper::getContinuous()
    {
        FUNCTION_ENTRY( "getContinuous" );

        reload();

        FUNCTION_EXIT;
        return m_continuous;
    }


    int DisplayRequestHelper::getPriority()
    {
        FUNCTION_ENTRY( "getPriority" );

        reload();

        FUNCTION_EXIT;
        return m_priority;
    }


    std::string DisplayRequestHelper::getRatisTag()
    {
        FUNCTION_ENTRY( "getRatisTag" );

        reload();

        FUNCTION_EXIT;
        return m_ratisTag;
    }


    std::string DisplayRequestHelper::getRatisDestination()
    {
        FUNCTION_ENTRY( "getRatisDestination" );

        reload();

        FUNCTION_EXIT;
        return m_ratisDestination;
    }


    bool DisplayRequestHelper::getRatisOverridable()
    {
        FUNCTION_ENTRY( "getRatisOverridable" );

        reload();

        FUNCTION_EXIT;
        return m_ratisOverridable;
    }


    std::string DisplayRequestHelper::getStisDestinationStation()
    {
        FUNCTION_ENTRY( "getStisDestinationStation" );

        reload();

        FUNCTION_EXIT;
        return m_stisDestinationStation;
    }


    std::string DisplayRequestHelper::getStisDestinationLevels()
    {
        FUNCTION_ENTRY( "getStisDestinationLevels" );

        reload();

        FUNCTION_EXIT;
        return m_stisDestinationLevels;
    }


    std::string DisplayRequestHelper::getStisDestinationPids()
    {
        FUNCTION_ENTRY( "getStisDestinationPids" );

        reload();

        FUNCTION_EXIT;
        return m_stisDestinationPids;
    }


    std::string DisplayRequestHelper::getTtisDestinationTrainId()
    {
        FUNCTION_ENTRY( "getTtisDestinationTrainId" );

        reload();

        FUNCTION_EXIT;
        return m_ttisDestinationTrainId;
    }


    std::string DisplayRequestHelper::getTtisDestinationPids()
    {
        FUNCTION_ENTRY( "getTtisDestinationPids" );

        reload();

        FUNCTION_EXIT;
        return m_ttisDestinationPids;
    }


    TA_Base_Core::DisplayAttributes DisplayRequestHelper::getDisplayAttributes()
    {
        FUNCTION_ENTRY( "getDisplayAttributes" );

        reload();

      FUNCTION_EXIT;
      return m_displayAttributes;

    }


    TA_Base_Core::PlasmaAttributes DisplayRequestHelper::getPlasmaAttributes()
    {
        FUNCTION_ENTRY( "getPlasmaAttributes" );

        reload();

      FUNCTION_EXIT;
      return m_plasmaAttributes;
    }


    TA_Base_Core::LEDAttributes DisplayRequestHelper::getLEDAttributes()
    {
        FUNCTION_ENTRY( "getLEDAttributes" );

        reload();

      FUNCTION_EXIT;
      return m_ledAttributes;
    }


    time_t DisplayRequestHelper::getCreatedTime()
    {
        FUNCTION_ENTRY( "getCreatedTime" );

        reload();

        FUNCTION_EXIT;
        return m_createTime;
    }


    // 
    // invalidate
    // 
    void DisplayRequestHelper::invalidate()
    {
        FUNCTION_ENTRY( "invalidate" );

        // This method cannot be called until the VideoOutputGroupKey and
        // the VideoOutput position have been set.

        TA_ASSERT( !m_isNew, "Attempted to call invalidate() on a new DisplayRequest entry.");

        m_isValidData = false;

        FUNCTION_EXIT;
    }


    const std::vector< std::string >& DisplayRequestHelper::getColumnNames()
    {
        FUNCTION_ENTRY( "getColumnNames" );

        TA_THREADGUARD( s_columnNamesLock );

        if ( true == s_columnNames.empty() )
        {
            s_columnNames.push_back( TA_TISAgentDAI::DISPLAY_REQUEST_PKEY_COLUMN );
            s_columnNames.push_back( TA_TISAgentDAI::TIMESTAMP_COLUMN );
            s_columnNames.push_back( TA_TISAgentDAI::DESTINATION_SYSTEM_COLUMN );
            s_columnNames.push_back( TA_TISAgentDAI::SUBMITTED_BY_COLUMN );
            s_columnNames.push_back( TA_TISAgentDAI::MESSAGE_TYPE_COLUMN );
            s_columnNames.push_back( TA_TISAgentDAI::MESSAGE_TAG_COLUMN );
            s_columnNames.push_back( TA_TISAgentDAI::LIBRARY_SECTION_COLUMN );
            s_columnNames.push_back( TA_TISAgentDAI::LIBRARY_VERSION_COLUMN );
            s_columnNames.push_back( TA_TISAgentDAI::AD_HOC_MESSAGE_CONTENT_COLUMN );
            s_columnNames.push_back( TA_TISAgentDAI::START_TIME_COLUMN );
            s_columnNames.push_back( TA_TISAgentDAI::END_TIME_COLUMN );
            s_columnNames.push_back( TA_TISAgentDAI::CONTINUOUS_COLUMN );
            s_columnNames.push_back( TA_TISAgentDAI::PRIORITY_COLUMN );
            s_columnNames.push_back( TA_TISAgentDAI::RATIS_TAG_COLUMN );
            s_columnNames.push_back( TA_TISAgentDAI::RATIS_DESTINATION_COLUMN );
            s_columnNames.push_back( TA_TISAgentDAI::RATIS_OVERRIDABLE_COLUMN );
            s_columnNames.push_back( TA_TISAgentDAI::STIS_DESTINATION_STATION_COLUMN );
            s_columnNames.push_back( TA_TISAgentDAI::STIS_DESTINATION_LEVELS_COLUMN );
            s_columnNames.push_back( TA_TISAgentDAI::STIS_DESTINATION_PIDS_COLUMN );
            s_columnNames.push_back( TA_TISAgentDAI::TTIS_DESTINATION_TRAIN_ID_COLUMN );
            s_columnNames.push_back( TA_TISAgentDAI::TTIS_DESTINATION_PIDS_COLUMN );
            s_columnNames.push_back( TA_TISAgentDAI::DISPLAY_MODE_COLUMN );
            s_columnNames.push_back( TA_TISAgentDAI::SCROLL_SPEED_COLUMN );
            s_columnNames.push_back( TA_TISAgentDAI::REPEAT_INTERVAL_COLUMN );
            s_columnNames.push_back( TA_TISAgentDAI::DISPLAY_TIME_COLUMN );
            s_columnNames.push_back( TA_TISAgentDAI::JUSTIFICATION_COLUMN );
            s_columnNames.push_back( TA_TISAgentDAI::LED_FONT_SIZE_COLUMN );
            s_columnNames.push_back( TA_TISAgentDAI::LED_INTENSITY_COLUMN );
            s_columnNames.push_back( TA_TISAgentDAI::LED_FONT_COLOUR_COLUMN );
            s_columnNames.push_back( TA_TISAgentDAI::LED_BACKGROUND_COLOUR_COLUMN );
            s_columnNames.push_back( TA_TISAgentDAI::PLASMA_FONT_SIZE_COLUMN );
            s_columnNames.push_back( TA_TISAgentDAI::PLASMA_FONT_TYPE_COLUMN );
            s_columnNames.push_back( TA_TISAgentDAI::PLASMA_FOREGROUND_COLOUR_COLUMN );
            s_columnNames.push_back( TA_TISAgentDAI::PLASMA_BACKGROUND_COLOUR_COLUMN );
            s_columnNames.push_back( TA_TISAgentDAI::DISPLAY_REQUEST_DATA_CREATETIME);
            s_columnNames.push_back(TA_TISAgentDAI::DISPLAY_REQUEST_IS_SEND_SUCCESS_COLUMN);
        }

        FUNCTION_EXIT;
        return s_columnNames;
    }


    const std::string& DisplayRequestHelper::getColumnNamesSql()
    {
        FUNCTION_ENTRY( "getColumnNamesSql" );

        TA_THREADGUARD( s_columnNamesSqlLock );

        if ( true == s_columnNamesSql.empty() )
        {
            getColumnNames();

            std::stringstream sql;

            const unsigned long sz = s_columnNames.size();

            for ( unsigned long i = 0; i < sz; ++i )
            {
                if ( TA_TISAgentDAI::DISPLAY_REQUEST_DATA_CREATETIME == s_columnNames[ i ] )
                {
                    sql << "TO_CHAR( "<< s_columnNames[ i ] << ", 'YYYYMMDDHH24MISS' )";
                }
                else
                {
                    sql << s_columnNames[ i ];
                }

                if ( i + 1 != sz )
                {
                    sql << ", ";
                }
            }

            sql << " ";

            s_columnNamesSql = sql.str();
        }

        FUNCTION_EXIT;
        return s_columnNamesSql;
    }


    void DisplayRequestHelper::populateData( boost::shared_ptr< TA_Base_Core::IData > data, unsigned long index )
    {
        FUNCTION_ENTRY( "populateData" );

        TryGet< long > tryData( data, 0 );

        if ( data->getNumRows() < index ) // No entry found with the specified pkey
        {
            char reasonMessage[ 256 ] = {0};
            sprintf( reasonMessage, "No message library found for pkey = %s", m_key.c_str() );
            TA_THROW( TA_Base_Core::DataException(reasonMessage,TA_Base_Core::DataException::NO_VALUE,"" ) );
        }
        
        m_timestamp   = data->getStringData( index, TA_TISAgentDAI::TIMESTAMP_COLUMN );
        m_destinationSystem = data->getStringData( index, TA_TISAgentDAI::DESTINATION_SYSTEM_COLUMN );
        m_submittedBy = data->getStringData( index, TA_TISAgentDAI::SUBMITTED_BY_COLUMN );
        m_messageType = data->getIntegerData( index, TA_TISAgentDAI::MESSAGE_TYPE_COLUMN );

        m_createTime = data->getDateData( index,  TA_TISAgentDAI::DISPLAY_REQUEST_DATA_CREATETIME);

        m_messageTag = tryData.getIntegerData( index, TA_TISAgentDAI::MESSAGE_TAG_COLUMN );
        m_librarySection = tryData.getIntegerData( index, TA_TISAgentDAI::LIBRARY_SECTION_COLUMN );
        m_libraryVersion = static_cast< unsigned short >( tryData.getIntegerData( index, TA_TISAgentDAI::LIBRARY_VERSION_COLUMN ) );

        m_adhocMessageContent = data->getStringData( index, TA_TISAgentDAI::AD_HOC_MESSAGE_CONTENT_COLUMN );
        m_startTime = data->getStringData( index, TA_TISAgentDAI::START_TIME_COLUMN );
        m_endTime = data->getStringData( index, TA_TISAgentDAI::END_TIME_COLUMN );
        try
        {
            m_continuous = (0 != data->getIntegerData( index, TA_TISAgentDAI::CONTINUOUS_COLUMN ) );
        }
        catch ( ... )
        {
            m_continuous = 0;
        }
        m_priority = data->getIntegerData( index, TA_TISAgentDAI::PRIORITY_COLUMN );
        m_ratisTag = data->getStringData( index, TA_TISAgentDAI::RATIS_TAG_COLUMN );
        m_ratisDestination = data->getStringData( index, TA_TISAgentDAI::RATIS_DESTINATION_COLUMN );

        try
        {
            m_ratisOverridable = ( 0 != data->getIntegerData( index, TA_TISAgentDAI::RATIS_OVERRIDABLE_COLUMN ) );
        }
        catch( ... )
        {
            m_ratisOverridable = false;
        }

        m_stisDestinationStation = data->getStringData( index, TA_TISAgentDAI::STIS_DESTINATION_STATION_COLUMN );
        m_stisDestinationLevels = data->getStringData( index, TA_TISAgentDAI::STIS_DESTINATION_LEVELS_COLUMN );
        m_stisDestinationPids = data->getStringData( index, TA_TISAgentDAI::STIS_DESTINATION_PIDS_COLUMN );
        m_ttisDestinationTrainId = data->getStringData( index, TA_TISAgentDAI::TTIS_DESTINATION_TRAIN_ID_COLUMN );
        m_ttisDestinationPids = data->getStringData( index, TA_TISAgentDAI::TTIS_DESTINATION_PIDS_COLUMN );

        m_displayAttributes.displayMode = (TA_Base_Core::EDisplayMode) tryData.getIntegerData( index,  TA_TISAgentDAI::DISPLAY_MODE_COLUMN );
        m_displayAttributes.scrollSpeed = (TA_Base_Core::EScrollSpeed) tryData.getIntegerData( index,  TA_TISAgentDAI::SCROLL_SPEED_COLUMN );
        m_displayAttributes.repeatInterval = static_cast< short >( tryData.getIntegerData( index,  TA_TISAgentDAI::REPEAT_INTERVAL_COLUMN ) );
        m_displayAttributes.displayTime = static_cast< short >( tryData.getIntegerData( index,  TA_TISAgentDAI::DISPLAY_TIME_COLUMN ) );
        m_displayAttributes.justification = (TA_Base_Core::EJustification) tryData.getIntegerData( index,  TA_TISAgentDAI::JUSTIFICATION_COLUMN );
        m_plasmaAttributes.fontType = (TA_Base_Core::EFontType) tryData.getIntegerData( index,  TA_TISAgentDAI::PLASMA_FONT_TYPE_COLUMN );
        m_plasmaAttributes.fontSize = (TA_Base_Core::EPlasmaFontSize) tryData.getIntegerData( index,  TA_TISAgentDAI::PLASMA_FONT_SIZE_COLUMN );
        m_plasmaAttributes.fontColour = (TA_Base_Core::EPlasmaColour) tryData.getIntegerData( index,  TA_TISAgentDAI::PLASMA_FOREGROUND_COLOUR_COLUMN );
        m_plasmaAttributes.backgroundColour = (TA_Base_Core::EPlasmaColour) tryData.getIntegerData( index,  TA_TISAgentDAI::PLASMA_BACKGROUND_COLOUR_COLUMN );
        m_ledAttributes.fontSize = (TA_Base_Core::ELEDFontSize) tryData.getIntegerData( index,  TA_TISAgentDAI::LED_FONT_SIZE_COLUMN );
        m_ledAttributes.intensity = (TA_Base_Core::ELEDIntensity) tryData.getIntegerData( index,  TA_TISAgentDAI::LED_INTENSITY_COLUMN );
        m_ledAttributes.fontColour = (TA_Base_Core::ELEDColour) tryData.getIntegerData( index,  TA_TISAgentDAI::LED_FONT_COLOUR_COLUMN );
        m_ledAttributes.backgroundColour = (TA_Base_Core::ELEDColour) tryData.getIntegerData( index,  TA_TISAgentDAI::LED_BACKGROUND_COLOUR_COLUMN );

        try
        {
            std::string ifSuccess;
            ifSuccess = (std::string) data->getStringData( index,  TA_TISAgentDAI::DISPLAY_REQUEST_IS_SEND_SUCCESS_COLUMN );
            if (TA_TISAgentDAI::IF_SEND_SUCCESS_FAIL == ifSuccess)
            {
                m_ifSendSuccess = TA_Base_Core::Fail;
            }
            else if (TA_TISAgentDAI::IF_SEND_SUCCESS_PASS == ifSuccess)
            {
                m_ifSendSuccess = TA_Base_Core::Pass;
            }
            else
            {
                m_ifSendSuccess = TA_Base_Core::NA;
            }
        }
        catch ( ... )
        {
            m_ifSendSuccess = TA_Base_Core::NA;
        }

        m_isValidData = true;

        FUNCTION_EXIT;
    }


    // 
    // reload
    // 
    void DisplayRequestHelper::reload()
    {
        FUNCTION_ENTRY( "reload" );

        if ( m_isValidData || m_isNew)
        {
            FUNCTION_EXIT;
            return;
        }

        TA_ASSERT( "" != m_key,
            "This Display Request does not yet exist in the database. Therefore it cannot be reloaded.");

        // Get a connection to the database.
        TA_Base_Core::IDatabase* databaseConnection =
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::TisDisplayRequestLogs, TA_Base_Core::Read);

        std::stringstream selectSql;
        selectSql << "select " << TA_TISAgentDAI::TIMESTAMP_COLUMN << ", ";
        selectSql << TA_TISAgentDAI::DESTINATION_SYSTEM_COLUMN << ", ";
        selectSql << TA_TISAgentDAI::SUBMITTED_BY_COLUMN << ", ";
        selectSql << TA_TISAgentDAI::MESSAGE_TYPE_COLUMN << ", ";
        selectSql << TA_TISAgentDAI::MESSAGE_TAG_COLUMN << ", ";
        selectSql << TA_TISAgentDAI::LIBRARY_SECTION_COLUMN << ", ";
        selectSql << TA_TISAgentDAI::LIBRARY_VERSION_COLUMN << ", ";
        selectSql << TA_TISAgentDAI::AD_HOC_MESSAGE_CONTENT_COLUMN << ", ";
        selectSql << TA_TISAgentDAI::START_TIME_COLUMN << ", ";
        selectSql << TA_TISAgentDAI::END_TIME_COLUMN << ", ";
        selectSql << TA_TISAgentDAI::CONTINUOUS_COLUMN << ", ";
        selectSql << TA_TISAgentDAI::PRIORITY_COLUMN << ", ";
        selectSql << TA_TISAgentDAI::RATIS_TAG_COLUMN << ", ";
        selectSql << TA_TISAgentDAI::RATIS_DESTINATION_COLUMN << ", ";
        selectSql << TA_TISAgentDAI::RATIS_OVERRIDABLE_COLUMN << ", ";
        selectSql << TA_TISAgentDAI::STIS_DESTINATION_STATION_COLUMN << ", ";
        selectSql << TA_TISAgentDAI::STIS_DESTINATION_LEVELS_COLUMN << ", ";
        selectSql << TA_TISAgentDAI::STIS_DESTINATION_PIDS_COLUMN << ", ";
        selectSql << TA_TISAgentDAI::TTIS_DESTINATION_TRAIN_ID_COLUMN << ", ";
        selectSql << TA_TISAgentDAI::TTIS_DESTINATION_PIDS_COLUMN << ", ";

        selectSql << TA_TISAgentDAI::DISPLAY_MODE_COLUMN << ", ";
        selectSql << TA_TISAgentDAI::SCROLL_SPEED_COLUMN << ", ";
        selectSql << TA_TISAgentDAI::REPEAT_INTERVAL_COLUMN << ", ";
        selectSql << TA_TISAgentDAI::DISPLAY_TIME_COLUMN << ", ";
        selectSql << TA_TISAgentDAI::JUSTIFICATION_COLUMN << ", ";

        selectSql << TA_TISAgentDAI::LED_FONT_SIZE_COLUMN << ", ";
        selectSql << TA_TISAgentDAI::LED_INTENSITY_COLUMN << ", ";
        selectSql << TA_TISAgentDAI::LED_FONT_COLOUR_COLUMN << ", ";
        selectSql << TA_TISAgentDAI::LED_BACKGROUND_COLOUR_COLUMN << ", ";

        selectSql << TA_TISAgentDAI::PLASMA_FONT_SIZE_COLUMN << ", ";
        selectSql << TA_TISAgentDAI::PLASMA_FONT_TYPE_COLUMN << ", ";
        selectSql << TA_TISAgentDAI::PLASMA_FOREGROUND_COLOUR_COLUMN << ", ";
        selectSql << TA_TISAgentDAI::PLASMA_BACKGROUND_COLOUR_COLUMN << ", ";

        // TD 16724
        selectSql << "TO_CHAR("<< TA_TISAgentDAI::DISPLAY_REQUEST_DATA_CREATETIME <<",'YYYYMMDDHH24MISS') , ";
        // TD 16724

        // hongran ++ TD17502
        selectSql << TA_TISAgentDAI::DISPLAY_REQUEST_IS_SEND_SUCCESS_COLUMN ;
        // ++hongran TD17502

        selectSql << " from " << TA_TISAgentDAI::DISPLAY_REQUEST_TABLE_NAME;
        selectSql << " where " << TA_TISAgentDAI::DISPLAY_REQUEST_PKEY_COLUMN;
        selectSql << "= '" << m_key << "'";

         // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back( TA_TISAgentDAI::TIMESTAMP_COLUMN );
        columnNames.push_back( TA_TISAgentDAI::DESTINATION_SYSTEM_COLUMN );
        columnNames.push_back( TA_TISAgentDAI::SUBMITTED_BY_COLUMN );
        columnNames.push_back( TA_TISAgentDAI::MESSAGE_TYPE_COLUMN );
        columnNames.push_back( TA_TISAgentDAI::MESSAGE_TAG_COLUMN );
        columnNames.push_back( TA_TISAgentDAI::LIBRARY_SECTION_COLUMN );
        columnNames.push_back( TA_TISAgentDAI::LIBRARY_VERSION_COLUMN );
        columnNames.push_back( TA_TISAgentDAI::AD_HOC_MESSAGE_CONTENT_COLUMN );
        columnNames.push_back( TA_TISAgentDAI::START_TIME_COLUMN );
        columnNames.push_back( TA_TISAgentDAI::END_TIME_COLUMN );
        columnNames.push_back( TA_TISAgentDAI::CONTINUOUS_COLUMN );
        columnNames.push_back( TA_TISAgentDAI::PRIORITY_COLUMN );
        columnNames.push_back( TA_TISAgentDAI::RATIS_TAG_COLUMN );
        columnNames.push_back( TA_TISAgentDAI::RATIS_DESTINATION_COLUMN );
        columnNames.push_back( TA_TISAgentDAI::RATIS_OVERRIDABLE_COLUMN );
        columnNames.push_back( TA_TISAgentDAI::STIS_DESTINATION_STATION_COLUMN );
        columnNames.push_back( TA_TISAgentDAI::STIS_DESTINATION_LEVELS_COLUMN );
        columnNames.push_back( TA_TISAgentDAI::STIS_DESTINATION_PIDS_COLUMN );
        columnNames.push_back( TA_TISAgentDAI::TTIS_DESTINATION_TRAIN_ID_COLUMN );
        columnNames.push_back( TA_TISAgentDAI::TTIS_DESTINATION_PIDS_COLUMN );

        columnNames.push_back( TA_TISAgentDAI::DISPLAY_MODE_COLUMN );
        columnNames.push_back( TA_TISAgentDAI::SCROLL_SPEED_COLUMN );
        columnNames.push_back( TA_TISAgentDAI::REPEAT_INTERVAL_COLUMN );
        columnNames.push_back( TA_TISAgentDAI::DISPLAY_TIME_COLUMN );
        columnNames.push_back( TA_TISAgentDAI::JUSTIFICATION_COLUMN );

        columnNames.push_back( TA_TISAgentDAI::LED_FONT_SIZE_COLUMN );
        columnNames.push_back( TA_TISAgentDAI::LED_INTENSITY_COLUMN );
        columnNames.push_back( TA_TISAgentDAI::LED_FONT_COLOUR_COLUMN );
        columnNames.push_back( TA_TISAgentDAI::LED_BACKGROUND_COLOUR_COLUMN );

        columnNames.push_back( TA_TISAgentDAI::PLASMA_FONT_SIZE_COLUMN );
        columnNames.push_back( TA_TISAgentDAI::PLASMA_FONT_TYPE_COLUMN );
        columnNames.push_back( TA_TISAgentDAI::PLASMA_FOREGROUND_COLOUR_COLUMN );
        columnNames.push_back( TA_TISAgentDAI::PLASMA_BACKGROUND_COLOUR_COLUMN );

        columnNames.push_back( TA_TISAgentDAI::DISPLAY_REQUEST_DATA_CREATETIME);

        // hongran++ TD17502
        columnNames.push_back(TA_TISAgentDAI::DISPLAY_REQUEST_IS_SEND_SUCCESS_COLUMN);
        // ++hongran TD17502

        // Execute the query. The method can throw a TA_Base_Core::DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        std::auto_ptr< TA_Base_Core::IData > data;
        try
        {
            data = std::auto_ptr< TA_Base_Core::IData >( databaseConnection->executeQuery(selectSql.str(),columnNames) );
        }
        catch( ... )
        {

        }

        if (0 == data->getNumRows()) // No entry found with the specified pkey
        {
            char reasonMessage[256] = {0};
            sprintf(reasonMessage, "No message library found for pkey = %s", m_key.c_str() );
                TA_THROW(TA_Base_Core::DataException(reasonMessage,TA_Base_Core::DataException::NO_VALUE,""));
        }
        else if (1 < data->getNumRows()) // More than one entry found for the pkey
        {
            char reasonMessage[256] = {0};
            sprintf(reasonMessage, "More than one Display Request found with pkey = %s", m_key.c_str() );
            TA_THROW(TA_Base_Core::DataException(reasonMessage,TA_Base_Core::DataException::NOT_UNIQUE,""));
        }

        // TODO
        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.

        m_timestamp   = data->getStringData(0,TA_TISAgentDAI::TIMESTAMP_COLUMN );
        m_destinationSystem = data->getStringData(0,TA_TISAgentDAI::DESTINATION_SYSTEM_COLUMN );
        m_submittedBy = data->getStringData(0,TA_TISAgentDAI::SUBMITTED_BY_COLUMN );
        m_messageType = data->getIntegerData(0,TA_TISAgentDAI::MESSAGE_TYPE_COLUMN );

        m_createTime = data->getDateData(0, TA_TISAgentDAI::DISPLAY_REQUEST_DATA_CREATETIME);
        try
        {
            m_messageTag = data->getIntegerData(0,TA_TISAgentDAI::MESSAGE_TAG_COLUMN );
        }
        catch ( ... )
        {
            m_messageTag = 0;
        }
        try
        {
            m_librarySection = data->getIntegerData(0,TA_TISAgentDAI::LIBRARY_SECTION_COLUMN );
        }
        catch ( ... )
        {
            m_librarySection = 0;
        }
        try
        {
            m_libraryVersion = (unsigned short)data->getIntegerData(0,TA_TISAgentDAI::LIBRARY_VERSION_COLUMN );
        }
        catch ( ... )
        {
            m_libraryVersion = 0;
        }
        m_adhocMessageContent = data->getStringData(0,TA_TISAgentDAI::AD_HOC_MESSAGE_CONTENT_COLUMN );
        m_startTime = data->getStringData(0,TA_TISAgentDAI::START_TIME_COLUMN );
        m_endTime = data->getStringData(0,TA_TISAgentDAI::END_TIME_COLUMN );
        try
        {
            m_continuous = (0 != data->getIntegerData(0,TA_TISAgentDAI::CONTINUOUS_COLUMN ) );
        }
        catch ( ... )
        {
            m_continuous = 0;
        }
        m_priority = data->getIntegerData(0,TA_TISAgentDAI::PRIORITY_COLUMN );
        m_ratisTag = data->getStringData(0,TA_TISAgentDAI::RATIS_TAG_COLUMN );
        m_ratisDestination = data->getStringData(0,TA_TISAgentDAI::RATIS_DESTINATION_COLUMN );

        try
        {
            m_ratisOverridable = ( 0 != data->getIntegerData(0,TA_TISAgentDAI::RATIS_OVERRIDABLE_COLUMN ) );
        }
        catch( ... )
        {
            m_ratisOverridable = false;
        }

        m_stisDestinationStation = data->getStringData(0,TA_TISAgentDAI::STIS_DESTINATION_STATION_COLUMN );
        m_stisDestinationLevels = data->getStringData(0,TA_TISAgentDAI::STIS_DESTINATION_LEVELS_COLUMN );
        m_stisDestinationPids = data->getStringData(0,TA_TISAgentDAI::STIS_DESTINATION_PIDS_COLUMN );
        m_ttisDestinationTrainId = data->getStringData(0,TA_TISAgentDAI::TTIS_DESTINATION_TRAIN_ID_COLUMN );
        m_ttisDestinationPids = data->getStringData(0,TA_TISAgentDAI::TTIS_DESTINATION_PIDS_COLUMN );

        try
        {
            m_displayAttributes.displayMode = (TA_Base_Core::EDisplayMode) data->getIntegerData(0, TA_TISAgentDAI::DISPLAY_MODE_COLUMN );
        }
        catch ( ... )
        {
            m_displayAttributes.displayMode = (TA_Base_Core::EDisplayMode) 0;
        }
        try
        {
            m_displayAttributes.scrollSpeed = (TA_Base_Core::EScrollSpeed) data->getIntegerData(0, TA_TISAgentDAI::SCROLL_SPEED_COLUMN );
        }
        catch ( ... )
        {
            m_displayAttributes.scrollSpeed = (TA_Base_Core::EScrollSpeed) 0;
        }
        try
        {
            m_displayAttributes.repeatInterval = data->getIntegerData(0, TA_TISAgentDAI::REPEAT_INTERVAL_COLUMN );
        }
        catch ( ... )
        {
            m_displayAttributes.repeatInterval = 0;
        }
        try
        {
            m_displayAttributes.displayTime = data->getIntegerData(0, TA_TISAgentDAI::DISPLAY_TIME_COLUMN );
        }
        catch ( ... )
        {
            m_displayAttributes.displayTime = 0;
        }
        try
        {
            m_displayAttributes.justification = (TA_Base_Core::EJustification) data->getIntegerData(0, TA_TISAgentDAI::JUSTIFICATION_COLUMN );
        }
        catch ( ... )
        {
            m_displayAttributes.justification = (TA_Base_Core::EJustification) 0;
        }

        try
        {
            m_plasmaAttributes.fontType = (TA_Base_Core::EFontType) data->getIntegerData(0, TA_TISAgentDAI::PLASMA_FONT_TYPE_COLUMN );
        }
        catch ( ... )
        {
            m_plasmaAttributes.fontType = (TA_Base_Core::EFontType) 0;
        }
        try
        {
            m_plasmaAttributes.fontSize = (TA_Base_Core::EPlasmaFontSize) data->getIntegerData(0, TA_TISAgentDAI::PLASMA_FONT_SIZE_COLUMN );
        }
        catch ( ... )
        {
            m_plasmaAttributes.fontSize  = (TA_Base_Core::EPlasmaFontSize) 0;
        }
        try
        {
            m_plasmaAttributes.fontColour = (TA_Base_Core::EPlasmaColour) data->getIntegerData(0, TA_TISAgentDAI::PLASMA_FOREGROUND_COLOUR_COLUMN );
        }
        catch ( ... )
        {
            m_plasmaAttributes.fontColour = (TA_Base_Core::EPlasmaColour) 0;
        }
        try
        {
            m_plasmaAttributes.backgroundColour = (TA_Base_Core::EPlasmaColour) data->getIntegerData(0, TA_TISAgentDAI::PLASMA_BACKGROUND_COLOUR_COLUMN );
        }
        catch ( ... )
        {
            m_plasmaAttributes.backgroundColour = (TA_Base_Core::EPlasmaColour) 0;
        }

        try
        {
            m_ledAttributes.fontSize = (TA_Base_Core::ELEDFontSize) data->getIntegerData(0, TA_TISAgentDAI::LED_FONT_SIZE_COLUMN );
        }
        catch ( ... )
        {
            m_ledAttributes.fontSize = (TA_Base_Core::ELEDFontSize) 0;
        }
        try
        {
            m_ledAttributes.intensity = (TA_Base_Core::ELEDIntensity) data->getIntegerData(0, TA_TISAgentDAI::LED_INTENSITY_COLUMN );
        }
        catch ( ... )
        {
            m_ledAttributes.intensity = (TA_Base_Core::ELEDIntensity) 0;
        }
        try
        {
            m_ledAttributes.fontColour = (TA_Base_Core::ELEDColour) data->getIntegerData(0, TA_TISAgentDAI::LED_FONT_COLOUR_COLUMN );
        }
        catch ( ... )
        {
            m_ledAttributes.fontColour = (TA_Base_Core::ELEDColour) 0;
        }
        try
        {
            m_ledAttributes.backgroundColour = (TA_Base_Core::ELEDColour) data->getIntegerData(0, TA_TISAgentDAI::LED_BACKGROUND_COLOUR_COLUMN );
        }
        catch ( ... )
        {
            m_ledAttributes.backgroundColour = (TA_Base_Core::ELEDColour) 0;
        }

        // hongran++ TD17502
        try
        {
            std::string ifSuccess = (std::string) data->getStringData(0, TA_TISAgentDAI::DISPLAY_REQUEST_IS_SEND_SUCCESS_COLUMN );
            if (TA_TISAgentDAI::IF_SEND_SUCCESS_FAIL == ifSuccess)
            {
                m_ifSendSuccess = TA_Base_Core::Fail;
            }
            else if (TA_TISAgentDAI::IF_SEND_SUCCESS_PASS == ifSuccess)
            {
                m_ifSendSuccess = TA_Base_Core::Pass;
            }
            else
            {
                m_ifSendSuccess = TA_Base_Core::NA;
            }
        }
        // ++hongran TD17502
        catch ( ... )
        {
            m_ifSendSuccess = TA_Base_Core::NA;
        }


        m_isValidData = true;

        FUNCTION_EXIT;
    }


    void DisplayRequestHelper::addNewDisplayRequestData()
    {
        FUNCTION_ENTRY( "addNewDisplayRequestData" );

        TA_Base_Core::IDatabase* databaseConnection =
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::TisDisplayRequestLogs, TA_Base_Core::Write);

        // run a query to update m_key
//        std::stringstream selectSql;
//        selectSql << "select " << TA_TISAgentDAI::DISPLAY_REQUEST_PKEY_SEQUENCE;
//        selectSql << " from DUAL";
//
//        std::vector< std::string > columnNames;
//        columnNames.push_back( "NEXTVAL" );
//
//        std::auto_ptr< TA_Base_Core::IData > data = std::auto_ptr< TA_Base_Core::IData >(
//            databaseConnection->executeQuery( selectSql.str(), columnNames ) );
//
//        // Check that one entry was found
//        if ( 1 != data->getNumRows() )
//        {
//            // unable to get a new key, throw an exception
//            std::stringstream error;
//            error << "Cannot insert Display Request into ";
//            error << TA_TISAgentDAI::DISPLAY_REQUEST_TABLE_NAME << ".";
//            TA_THROW( TA_Base_Core::DataException( error.str().c_str(), TA_Base_Core::DataException::NO_VALUE, "" ));
//        }
//
//        // get the pkey to use
//        m_key = data->getUnsignedLongData( 0, "NEXTVAL" );

        m_key = TA_Base_Core::TA_Uuid::generateUUID();
        if ((m_ifSendSuccess != TA_Base_Core::Pass) && (m_ifSendSuccess != TA_Base_Core::Fail))
        {
            FUNCTION_EXIT;
            return;
        }

        LOG ( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
              "Got a unique pkey for new display request. Inserting with key %s", m_key.c_str());


        std::stringstream entrySql;
        entrySql.str("");


        entrySql << "BEGIN ";
        entrySql << "audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "','INSERT', '";
        entrySql << "insert into " << TA_TISAgentDAI::DISPLAY_REQUEST_TABLE_NAME;
        entrySql << " (";
        entrySql << TA_TISAgentDAI::DISPLAY_REQUEST_PKEY_COLUMN << ", ";
        entrySql << TA_TISAgentDAI::TIMESTAMP_COLUMN << ", ";
        entrySql << TA_TISAgentDAI::DESTINATION_SYSTEM_COLUMN << ", ";
        entrySql << TA_TISAgentDAI::SUBMITTED_BY_COLUMN << ", ";
        entrySql << TA_TISAgentDAI::MESSAGE_TYPE_COLUMN << ", ";
        entrySql << TA_TISAgentDAI::MESSAGE_TAG_COLUMN << ", ";
        entrySql << TA_TISAgentDAI::LIBRARY_SECTION_COLUMN << ", ";
        entrySql << TA_TISAgentDAI::LIBRARY_VERSION_COLUMN << ", ";
        entrySql << TA_TISAgentDAI::AD_HOC_MESSAGE_CONTENT_COLUMN << ", ";
        entrySql << TA_TISAgentDAI::START_TIME_COLUMN << ", ";
        entrySql << TA_TISAgentDAI::END_TIME_COLUMN << ", ";
        entrySql << TA_TISAgentDAI::CONTINUOUS_COLUMN << ", ";
        entrySql << TA_TISAgentDAI::PRIORITY_COLUMN << ", ";
        entrySql << TA_TISAgentDAI::RATIS_TAG_COLUMN << ", ";
        entrySql << TA_TISAgentDAI::RATIS_DESTINATION_COLUMN << ", ";
        entrySql << TA_TISAgentDAI::RATIS_OVERRIDABLE_COLUMN << ", ";
        entrySql << TA_TISAgentDAI::STIS_DESTINATION_STATION_COLUMN << ", ";
        entrySql << TA_TISAgentDAI::STIS_DESTINATION_LEVELS_COLUMN << ", ";
        entrySql << TA_TISAgentDAI::STIS_DESTINATION_PIDS_COLUMN << ", ";
        entrySql << TA_TISAgentDAI::TTIS_DESTINATION_TRAIN_ID_COLUMN << ", ";
        entrySql << TA_TISAgentDAI::TTIS_DESTINATION_PIDS_COLUMN << ", ";

        entrySql << TA_TISAgentDAI::DISPLAY_MODE_COLUMN << ", ";
        entrySql << TA_TISAgentDAI::SCROLL_SPEED_COLUMN << ", ";
        entrySql << TA_TISAgentDAI::REPEAT_INTERVAL_COLUMN << ", ";
        entrySql << TA_TISAgentDAI::DISPLAY_TIME_COLUMN << ", ";
        entrySql << TA_TISAgentDAI::JUSTIFICATION_COLUMN << ", ";
        entrySql << TA_TISAgentDAI::LED_FONT_SIZE_COLUMN << ", ";
        entrySql << TA_TISAgentDAI::LED_INTENSITY_COLUMN << ", ";
        entrySql << TA_TISAgentDAI::LED_FONT_COLOUR_COLUMN << ", ";
        entrySql << TA_TISAgentDAI::LED_BACKGROUND_COLOUR_COLUMN << ", ";
        entrySql << TA_TISAgentDAI::PLASMA_FONT_SIZE_COLUMN << ", ";
        entrySql << TA_TISAgentDAI::PLASMA_FONT_TYPE_COLUMN << ", ";
        entrySql << TA_TISAgentDAI::PLASMA_FOREGROUND_COLOUR_COLUMN << ", ";
        entrySql << TA_TISAgentDAI::PLASMA_BACKGROUND_COLOUR_COLUMN << ", ";

        // hongran++ TD17502
        entrySql << TA_TISAgentDAI::DISPLAY_REQUEST_IS_SEND_SUCCESS_COLUMN;
        // ++hongran TD17502


        entrySql << ")";
        entrySql << " values (''";
        entrySql << m_key << "'', ";
        entrySql << "''" << m_timestamp << "'', ";
        entrySql << "''" << m_destinationSystem << "'', ";
        entrySql << "''" << m_submittedBy << "'', ";
        entrySql << m_messageType << ", ";
        entrySql << m_messageTag << ", ";
        entrySql << m_librarySection << ", ";
        entrySql << m_libraryVersion << ", ";
        entrySql << "''" << databaseConnection->escapeAQSQLString(m_adhocMessageContent) << "'', ";  // TES844
        entrySql << "''" << m_startTime << "'', ";
        entrySql << "''" << m_endTime << "'', ";
        entrySql << m_continuous << ", ";
        entrySql << m_priority << ", ";
        entrySql << "''" << m_ratisTag << "'', ";
        entrySql << "''" << m_ratisDestination << "'', ";
        entrySql << m_ratisOverridable << ", ";
        entrySql << "''" << m_stisDestinationStation << "'', ";
        entrySql << "''" << m_stisDestinationLevels << "'', ";
        entrySql << "''" << m_stisDestinationPids << "'', ";
        entrySql << "''" << m_ttisDestinationTrainId << "'', ";
        entrySql << "''" << m_ttisDestinationPids << "'', ";
        entrySql << m_displayAttributes.displayMode << ", ";
        entrySql << m_displayAttributes.scrollSpeed << ", ";
        entrySql << m_displayAttributes.repeatInterval << ", ";
        entrySql << m_displayAttributes.displayTime << ", ";
        entrySql << m_displayAttributes.justification << ", ";
        entrySql << m_ledAttributes.fontSize << ", ";
        entrySql << m_ledAttributes.intensity << ", ";
        entrySql << m_ledAttributes.fontColour << ", ";
        entrySql << m_ledAttributes.backgroundColour << ", ";
        entrySql << m_plasmaAttributes.fontSize << ", ";
        entrySql << m_plasmaAttributes.fontType << ", ";
        entrySql << m_plasmaAttributes.fontColour << ", ";
        entrySql << m_plasmaAttributes.backgroundColour << ", ";

        entrySql << "''" << databaseConnection->escapeAQSQLString(convertSendSuccessToStr(m_ifSendSuccess)) << "'')";

        entrySql << "'); ";
        entrySql << "END;";

        // execute insert
        try
        {
            databaseConnection->executeModification(entrySql.str());
        }
        catch( ... )
        {
            LOG ( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
              "Could not insert new display request into database");
        }

        m_isNew = false;

        FUNCTION_EXIT;
    }


    void DisplayRequestHelper::writeDisplayRequestData()
    {
        FUNCTION_ENTRY( "writeDisplayRequestData" );

        addNewDisplayRequestData();

        // Now that the data has been written, what we hold is valid data.
        m_isValidData = true;
        m_isNew = false;
        m_hasChanged = false;

        FUNCTION_EXIT;
    }


    void DisplayRequestHelper::deleteThisDisplayRequest()
    {
        FUNCTION_ENTRY( "deleteThisDisplayRequest" );

        TA_Base_Core::IDatabase* databaseConnection =
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::TisDisplayRequestLogs, TA_Base_Core::Write);

        // Delete all the messages that 'belong' to this library
        std::ostringstream formatSQL;
        formatSQL.str("");

        formatSQL << "BEGIN ";
        formatSQL << "audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "','DELETE', '";
        formatSQL << "delete from TI_DISPLAY_REQUEST ";
        formatSQL << "where TIDREQ_ID = ''" << m_key <<"''";
        formatSQL << "'); ";
        formatSQL << "END;";

        try
        {
            databaseConnection->executeModification( formatSQL.str() );
        }
        catch( ... )
        {

        }

        // Now that the data has been deleted, it is new.
        m_isNew = true;
        m_hasChanged = true;

        FUNCTION_EXIT;
    }


    void DisplayRequestHelper::setTimestamp( const std::string& timestamp )
    {
        FUNCTION_ENTRY( "setTimestamp" );

        reload();
        m_timestamp = timestamp;

        FUNCTION_EXIT;
    }


    void DisplayRequestHelper::setDestinationSystem( const std::string& destinationSystem )
    {
        FUNCTION_ENTRY( "setDestinationSystem" );

        reload();
        m_destinationSystem = destinationSystem;

        FUNCTION_EXIT;
    }


    void DisplayRequestHelper::setSubmittedBy( const std::string& submittedBy )
    {
        FUNCTION_ENTRY( "setSubmittedBy" );

        reload();
        m_submittedBy = submittedBy;

        FUNCTION_EXIT;
    }


    void DisplayRequestHelper::setMessageType( int messageType )
    {
        FUNCTION_ENTRY( "setMessageType" );

        reload();
        m_messageType = messageType;

        FUNCTION_EXIT;
    }


    void DisplayRequestHelper::setMessageTag( int messageTag )
    {
        FUNCTION_ENTRY( "setMessageTag" );

        reload();
        m_messageTag = messageTag;

        FUNCTION_EXIT;
    }


    void DisplayRequestHelper::setLibrarySection( int librarySection )
    {
        FUNCTION_ENTRY( "setLibrarySection" );

        reload();
        m_librarySection = librarySection;

        FUNCTION_EXIT;
    }


    void DisplayRequestHelper::setLibraryVersion( unsigned short libraryVersion )
    {
        FUNCTION_ENTRY( "setLibraryVersion" );

        reload();
        m_libraryVersion = libraryVersion;

        FUNCTION_EXIT;
    }


    void DisplayRequestHelper::setAdhocMessageContent( const std::string& adhocMessageContent )
    {
        FUNCTION_ENTRY( "setAdhocMessageContent" );

        reload();
        m_adhocMessageContent = adhocMessageContent;

        FUNCTION_EXIT;
    }


    void DisplayRequestHelper::setStartTime( const std::string startTime )
    {
        FUNCTION_ENTRY( "setStartTime" );

        reload();
        m_startTime = startTime;

        FUNCTION_EXIT;
    }


    void DisplayRequestHelper::setEndTime( const std::string endTime )
    {
        FUNCTION_ENTRY( "setEndTime" );

        reload();
        m_endTime = endTime;

        FUNCTION_EXIT;
    }


    void DisplayRequestHelper::setContinuous( bool continuous )
    {
        FUNCTION_ENTRY( "setContinuous" );

        reload();
        m_continuous = continuous;

        FUNCTION_EXIT;
    }


    void DisplayRequestHelper::setPriority( int priority )
    {
        FUNCTION_ENTRY( "setPriority" );

        reload();
        m_priority = priority;

        FUNCTION_EXIT;
    }


    void DisplayRequestHelper::setRatisTag( const std::string& ratisTag )
    {
        FUNCTION_ENTRY( "setRatisTag" );

        reload();
        m_ratisTag = ratisTag;

        FUNCTION_EXIT;
    }


    void DisplayRequestHelper::setRatisDestination( const std::string& destination )
    {
        FUNCTION_ENTRY( "setRatisDestination" );

        reload();
        m_ratisDestination = destination;

        FUNCTION_EXIT;
    }


    void DisplayRequestHelper::setRatisOverridable( bool overridable )
    {
        FUNCTION_ENTRY( "setRatisOverridable" );

        reload();
        m_ratisOverridable = overridable;

        FUNCTION_EXIT;
    }


    void DisplayRequestHelper::setStisDestinationStation( const std::string& destinationStation )
    {
        FUNCTION_ENTRY( "setStisDestinationStation" );

        reload();
        m_stisDestinationStation = destinationStation;

        FUNCTION_EXIT;
    }


    void DisplayRequestHelper::setStisDestinationLevels( const std::string& destinationLevels )
    {
        FUNCTION_ENTRY( "setStisDestinationLevels" );

        reload();
        m_stisDestinationLevels = destinationLevels;

        FUNCTION_EXIT;
    }


    void DisplayRequestHelper::setStisDestinationPids( const std::string& destinationPids )
    {
        FUNCTION_ENTRY( "setStisDestinationPids" );

        reload();
        m_stisDestinationPids = destinationPids;

        FUNCTION_EXIT;
    }


    void DisplayRequestHelper::setTtisDestinationTrainId( const std::string& destinationTrainId )
    {
        FUNCTION_ENTRY( "setTtisDestinationTrainId" );

        reload();
        m_ttisDestinationTrainId = destinationTrainId;

        FUNCTION_EXIT;
    }


    void DisplayRequestHelper::setTtisDestinationPids( const std::string& destinationPids )
    {
        FUNCTION_ENTRY( "setTtisDestinationPids" );

        reload();
        m_ttisDestinationPids = destinationPids;

        FUNCTION_EXIT;
    }


    void DisplayRequestHelper::setDisplayAttributes( TA_Base_Core::DisplayAttributes attributes )
    {
        FUNCTION_ENTRY( "setDisplayAttributes" );

        reload();
        m_displayAttributes = attributes;

        FUNCTION_EXIT;
    }


    void DisplayRequestHelper::setPlasmaAttributes( TA_Base_Core::PlasmaAttributes attributes )
    {
        FUNCTION_ENTRY( "setPlasmaAttributes" );

        reload();
        m_plasmaAttributes = attributes;

        FUNCTION_EXIT;
    }


    void DisplayRequestHelper::setLEDAttributes( TA_Base_Core::LEDAttributes attributes )
    {
        FUNCTION_ENTRY( "setLEDAttributes" );

        reload();
        m_ledAttributes = attributes;

        FUNCTION_EXIT;
    }


    std::string DisplayRequestHelper::getLocalDatabaseName()
    {
        FUNCTION_ENTRY( "getLocalDatabaseName" );

        if (m_localDatabase.empty())
        {
            // have to find out what is the local db name
            // this should be the first in the connection list so grab it
            if (! TA_Base_Core::RunParams::getInstance().isSet(RPARAM_DBCONNECTIONFILE) )
            {
                TA_THROW(TA_Base_Core::DatabaseException("db-connection-file not set"));
            }
            try
            {
                TA_Base_Core::DbConnectionStrings connectionStrings(TA_Base_Core::RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE));
                TA_Base_Core::DataConnections connections = connectionStrings.getConnectionList(TA_Base_Core::TisDisplayRequestLogs, TA_Base_Core::Write);
                m_localDatabase = connections[0].first;
            }
            catch(...) // most likely a file not found error
            {
                TA_THROW( TA_Base_Core::DatabaseException("Unable to find database connection infomation"));
            }

        }

        FUNCTION_EXIT;
        return m_localDatabase;
    }


    // hongran++ TD17502
    TA_Base_Core::IfSendSuccess DisplayRequestHelper::getIfSendSuccess()
    {
        FUNCTION_ENTRY( "getIfSendSuccess" );
        FUNCTION_EXIT;
        return m_ifSendSuccess;
    }


    void DisplayRequestHelper::setIfSendSuccess(TA_Base_Core::IfSendSuccess successful)
    {
        FUNCTION_ENTRY( "setIfSendSuccess" );

        reload();
        m_ifSendSuccess = successful;

        FUNCTION_EXIT;
    }


    std::string DisplayRequestHelper::convertSendSuccessToStr(TA_Base_Core::IfSendSuccess success)
    {
        FUNCTION_ENTRY( "convertSendSuccessToStr" );

        std::string temp;
        switch(success)
        {
        case TA_Base_Core::Pass:
            temp = "Pass";
            break;
        case TA_Base_Core::Fail:
            temp = "Fail";
            break;
        default:
            temp = "N/A";
        }
        LOG ( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
          "##The value of success is %s", temp.c_str());

        FUNCTION_EXIT;
        return temp;
    }


} // closes TA_Base_Core
