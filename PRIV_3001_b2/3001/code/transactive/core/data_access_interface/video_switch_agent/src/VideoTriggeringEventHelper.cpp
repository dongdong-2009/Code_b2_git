/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/core/data_access_interface/video_switch_agent/src/VideoTriggeringEventHelper.cpp $
  * @author:  Nick Jardine
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * VideoTriggeringEventHelper is an object that is held by VideoTriggeringEvent and ConfigVideoTriggeringEvent objects. 
  * It contains all data used by VideoTriggeringEvents, and manipulation
  * methods for the data. It helps avoid re-writing code for both VideoTriggeringEvent and ConfigVideoTriggeringEvent.
  */


#pragma warning(disable:4786) // identifier was truncated to 'number' characters in the debug information
#pragma warning(disable:4503) // decorated name length exceeded, name was truncated
#pragma warning(disable:4290) // C++ Exception Specification ignored

#include <vector>
#include <algorithm>
#include <ctime>

#include "core/data_access_interface/video_switch_agent/src/VideoTriggeringEventHelper.h"
#include "core/data_access_interface/video_switch_agent/src/VideoTriggeringEventAccessFactory.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"
#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"


using TA_Base_Core::DebugUtil;

namespace TA_IRS_Core
{

    // triggering events table
    static const std::string TRIGGERING_EVENTS_TABLE = "VI_TRIGGERING_EVENTS";
    static const std::string PKEY_COL = "VITREV_ID";
    static const std::string PKEY_SEQ = "VITREV_SEQ.NEXTVAL";
    static const std::string STATION_ID_PKEY_COL = "VISTID_ID";
    static const std::string EVENT_ID_COL = "EVENT_ID";
	static const std::string LOCATION_COL = "LOCATION";
    static const std::string TRIGGERING_EVENT_COL = "CCTV_TRIGGERING";
    static const std::string MANUAL_CLOSING_COL = "MANUAL_CLOSING";
	// TD 15200
	static const std::string ENTITYKEY_COL = "ENTITYKEY";
	// TD 15200
    static const std::string DATE_CREATED_COL = "DATE_CREATED";
    static const std::string DATE_MODIFIED_COL = "DATE_MODIFIED";

    // station id table
    static const std::string STATION_ID_TABLE = "VI_STATION_ID";
    static const std::string STATION_ID_COL = "STATION_ID";
    static const std::string TA_LOCATION_COL = "TA_LOCATION";

	// TD 15200 entity table
	static const std::string ENTITY_TABLE = "ENTITY";
    static const std::string E_PKEY_COL = "PKEY";

    VideoTriggeringEventHelper::VideoTriggeringEventHelper( const VideoTriggeringEventHelper& theVideoTriggeringEventHelper)
		: m_pkey(-1),
          m_stationId(theVideoTriggeringEventHelper.m_stationId),
		  m_eventId(theVideoTriggeringEventHelper.m_eventId),
          m_location(theVideoTriggeringEventHelper.m_location),
          m_triggeringEvent(theVideoTriggeringEventHelper.m_triggeringEvent),
          m_manualClosing(theVideoTriggeringEventHelper.m_manualClosing),
		  m_entitykey(theVideoTriggeringEventHelper.m_entitykey), // TD 15200
          m_dateCreated(0),
          m_dateModified(0),
          m_isValidData(false),
          m_isNew(true)
    {	  
	};


    VideoTriggeringEventHelper::VideoTriggeringEventHelper(const unsigned long key)
        : m_pkey(key),
          m_stationId(-1),
		  m_eventId(-1),
          m_location(""),
          m_triggeringEvent(""),
          m_manualClosing(false),
		  m_entitykey(-1), // TD 15200
          m_dateCreated(0),
          m_dateModified(0),
          m_isValidData(false),
          m_isNew(false)
    {
    }


    VideoTriggeringEventHelper::VideoTriggeringEventHelper()
        : m_pkey(-1),
          m_stationId(-1),
		  m_eventId(-1),
          m_location(""),
          m_triggeringEvent(""),
          m_manualClosing(false),
		  m_entitykey(-1), // TD 15200
          m_dateCreated(0),
          m_dateModified(0),
          m_isValidData(false),
          m_isNew(true)
    {
    }


    VideoTriggeringEventHelper::VideoTriggeringEventHelper(unsigned long row, TA_Base_Core::IData& data)
        : m_pkey(-1),
          m_stationId(-1),
		  m_eventId(-1),
          m_location(""),
          m_triggeringEvent(""),
          m_manualClosing(false),
		  m_entitykey(-1), // TD 15200
          m_dateCreated(0),
          m_dateModified(0),
          m_isValidData(false),
          m_isNew(false)
    {
        m_pkey = data.getUnsignedLongData(row, PKEY_COL);

        reloadUsing(row, data);
    }


    VideoTriggeringEventHelper::~VideoTriggeringEventHelper()
    {
    }

    unsigned long VideoTriggeringEventHelper::getKey()
    {
        FUNCTION_ENTRY("getKey");
        TA_ASSERT(!m_isNew,"The data must be written to the database before the pkey can be retrieved");

        FUNCTION_EXIT;
        return m_pkey;
    }

    std::string VideoTriggeringEventHelper::getName()
    {
        // return the triggering event description
        // its as close to a name as this things got.
        FUNCTION_ENTRY("getName");
        FUNCTION_EXIT;
        return getTriggeringEvent();
    }
    
    unsigned long VideoTriggeringEventHelper::getStationId()
    {
        FUNCTION_ENTRY("getStationId");
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
		
        FUNCTION_EXIT;
        return m_stationId;
    }


    unsigned long VideoTriggeringEventHelper::getEventId()
    {
        FUNCTION_ENTRY("getEventId" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        FUNCTION_EXIT;
        return m_eventId;
    }


    std::string VideoTriggeringEventHelper::getLocation()
    {
        FUNCTION_ENTRY("getLocation" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        FUNCTION_EXIT;
        return m_location;
    }


	std::string VideoTriggeringEventHelper::getTriggeringEvent()
    {
        FUNCTION_ENTRY("getTriggeringEvent" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        FUNCTION_EXIT;
        return m_triggeringEvent;
    }

    bool VideoTriggeringEventHelper::getManualClosing()
    {
        FUNCTION_ENTRY("getManualClosing" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        FUNCTION_EXIT;
        return m_manualClosing;
    }

	// TD 15200
	unsigned long VideoTriggeringEventHelper::getEntityKey()
    {
        FUNCTION_ENTRY("getEntityKey" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        FUNCTION_EXIT;
        return m_entitykey;
    }
	// TD 15200
    
    void VideoTriggeringEventHelper::setStationId(unsigned long stationId)
    {
        FUNCTION_ENTRY("setStationId");
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        
		m_stationId = stationId;
        FUNCTION_EXIT;
    }

    void VideoTriggeringEventHelper::setEventId(unsigned long eventId)
    {
        FUNCTION_ENTRY("setEventId");
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        
		m_eventId = eventId;
        FUNCTION_EXIT;
    }


    void VideoTriggeringEventHelper::setLocation(const std::string& location)
    {
        FUNCTION_ENTRY("setLocation" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        m_location = location;
        FUNCTION_EXIT;
    }


    void VideoTriggeringEventHelper::setTriggeringEvent(const std::string& triggeringEvent)
    {
        FUNCTION_ENTRY("setTriggeringEvent" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        m_triggeringEvent = triggeringEvent;

        FUNCTION_EXIT;
    }


    void VideoTriggeringEventHelper::setManualClosing(bool manualClosing)
    {
        FUNCTION_ENTRY("setManualClosing" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        m_manualClosing = manualClosing;

        FUNCTION_EXIT;
    }

	// TD 15200
	void VideoTriggeringEventHelper::setEntityKey(unsigned long entityKey)
    {
        FUNCTION_ENTRY("setEntityKey");
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        
		m_entitykey = entityKey;
        FUNCTION_EXIT;
    }
	// TD 15200

    time_t VideoTriggeringEventHelper::getDateCreated()
    {
        FUNCTION_ENTRY("getDateCreated" );
        TA_ASSERT(!m_isNew,"The data must be written to the database before the date modified can be retrieved");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        FUNCTION_EXIT;
        return m_dateCreated;
    }


    time_t VideoTriggeringEventHelper::getDateModified()
    {
        FUNCTION_ENTRY("getDateModified" );
        TA_ASSERT(!m_isNew,"The data must be written to the database before the date modified can be retrieved");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        FUNCTION_EXIT;
        return m_dateModified;
    }


	void VideoTriggeringEventHelper::invalidate()
    {
        FUNCTION_ENTRY("invalidate" );

        // This method cannot be called until the key has been set
        TA_ASSERT(!m_isNew,"Attempted to call invalidate() on a new VideoTriggeringEvent");

        m_isValidData = false;

        FUNCTION_EXIT;
    }


    void VideoTriggeringEventHelper::reload()
    {
        FUNCTION_ENTRY("reload" );
        // Ensure the VideoTriggeringEvent has already been written to the database. It doesn't make
        // any sense to reload an VideoTriggeringEvent that doesn't exist in the database.
        TA_ASSERT(!m_isNew, "This VideoTriggeringEvent does not yet exist in the database. Therefore it cannot be reloaded.");
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Video_Cd, TA_Base_Core::Read);


        //std::ostringstream sql;
        //sql << "select "
        //    << "e." << PKEY_COL << ", "
        //    << "s." << STATION_ID_COL << ", "
        //    << "e." << EVENT_ID_COL << ", "
        //    << "e." << LOCATION_COL << ", "
        //    << "e." << TRIGGERING_EVENT_COL << ", "
        //    << "e." << MANUAL_CLOSING_COL << ", "
        //    << "en." << E_PKEY_COL << ", "  // TD 15200
        //    << "TO_CHAR(e." << DATE_CREATED_COL << ",'YYYYMMDDHH24MISS'), "
        //    << "TO_CHAR(nvl(e." << DATE_MODIFIED_COL << ",TO_DATE('12:00:00 01/01/1990','hh:mi:ss dd/mm/yyyy')),'YYYYMMDDHH24MISS')"
        //    << " from " << TRIGGERING_EVENTS_TABLE << " e, " << STATION_ID_TABLE << " s, " << ENTITY_TABLE << " en" // TD 15200
        //    << " where e." << STATION_ID_PKEY_COL <<" = s." << STATION_ID_PKEY_COL
        //    << " and en." << E_PKEY_COL <<" = e." << ENTITYKEY_COL // TD 15200
        //    << " and e." << PKEY_COL << " = " << m_pkey;
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), VI_STATION_ID_Oracle_SELECT_62057, m_pkey);
//		std::string strSql  = databaseConnection->prepareSQLStatement(VI_STATION_ID_SELECT_62057, m_pkey);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, VI_STATION_ID_SELECT_62057, m_pkey);

        // Set up the columnNames vector to be passed to executeQuery()

        std::vector<std::string> columnNames;
        columnNames.push_back(PKEY_COL);
		columnNames.push_back(STATION_ID_COL);
        columnNames.push_back(EVENT_ID_COL);
		columnNames.push_back(LOCATION_COL);
        columnNames.push_back(TRIGGERING_EVENT_COL);
		columnNames.push_back(MANUAL_CLOSING_COL);
		columnNames.push_back(ENTITYKEY_COL);  // TD 15200
        columnNames.push_back(DATE_CREATED_COL);
		columnNames.push_back(DATE_MODIFIED_COL);
        
        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned TA_Base_Core::IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql, columnNames);

        if (0 == data->getNumRows()) 
        {
            // VideoTriggeringEvent record not found   
            delete data;
            data = NULL;

            TA_THROW( TA_Base_Core::DataException("No triggering events found",
                                    TA_Base_Core::DataException::NO_VALUE,
                                    TRIGGERING_EVENTS_TABLE ) );
        }
        
        // assign the data to the member variables
        try
        {
            reloadUsing(0, *data);
        }
        catch (...)
        {
            // prevent memory leaks
            delete data;
            data = NULL;
            throw;
        }
        
        // Now that we're done with the TA_Base_Core::IData object, it's our responsibility to delete it
        delete data;
        data = NULL;
        
        FUNCTION_EXIT;
    }


    void VideoTriggeringEventHelper::reloadUsing(unsigned long row, TA_Base_Core::IData& data)
    {   
        FUNCTION_ENTRY("reloadUsing" );

        // Assign the data as appropriate to the member variables.
        // These calls can throw TA_Base_Core::DataExceptions, as documented in the comment of this method.
        m_pkey = data.getUnsignedLongData(row, PKEY_COL);
		m_stationId = data.getUnsignedLongData(row, STATION_ID_COL);
        m_eventId = data.getUnsignedLongData(row, EVENT_ID_COL);
		m_location = data.getStringData(row, LOCATION_COL);
        m_triggeringEvent = data.getStringData(row, TRIGGERING_EVENT_COL);
		m_manualClosing = data.getBooleanData(row, MANUAL_CLOSING_COL);
		// TD 15200
		m_entitykey = data.getUnsignedLongData(row, ENTITYKEY_COL);
		// TD 15200

        m_dateCreated = data.getDateData( row, DATE_CREATED_COL );
        
        try
        {
            m_dateModified = data.getDateData( row, DATE_MODIFIED_COL );
            struct tm newtm;

            // Set up the date created structure to equal the value we set it to when we get NULL
            // from the database
		    newtm.tm_sec = 0;		//seconds
		    newtm.tm_min = 0;		// minutes 
		    newtm.tm_hour = 12;		// hours 
		    newtm.tm_mday = 1;		// day of the month 
		    newtm.tm_mon = 0;	// month of Year - goes from 0 to 11
		    newtm.tm_year = 90;	// years since 1900 - this structure only deals with years 
									    // between 1901 and 2038
		    newtm.tm_isdst = 0;			// disable daylight savings - this could have implications

		    time_t value;
		    value = mktime(&newtm);

            if (value == m_dateModified) // The date was actually NULL
            {
                m_dateModified = 0;
            }
        }
        catch (TA_Base_Core::DataException& ex)
        {
            //If we catch this exception, need to check if its a "NO_VALUE" type
            if (TA_Base_Core::DataException::NO_VALUE != ex.getFailType())
            {   // Need to on-throw this exception
                throw;
            }
            else // The date-modified column was null - this is not fatal
            {
                m_dateModified = 0;
            }
        }

        // Need to record that we now have valid data
        m_isValidData = true;
        FUNCTION_EXIT;
    }


    void VideoTriggeringEventHelper::writeVideoTriggeringEventData() throw(TA_Base_Core::TransactiveException)
    {       
        FUNCTION_ENTRY( "writeVideoTriggeringEventData" );     

        // Check all values exist. If not add them to the dataConfiguratonException
        std::vector<std::string> fieldNames;
        if (-1 == m_stationId)
        {
            fieldNames.push_back(STATION_ID_COL);
        }
        if (-1 == m_eventId)
        {
            fieldNames.push_back(EVENT_ID_COL);
        }
        if ("" == m_location)
        {
            fieldNames.push_back(TA_LOCATION_COL);
        }
        if ("" == m_triggeringEvent)
        {
            fieldNames.push_back(TRIGGERING_EVENT_COL);
        }
		// TD 15200
		if (-1 == m_entitykey)
        {
            fieldNames.push_back(ENTITYKEY_COL);
        }
		// TD 15200
        if ( !fieldNames.empty() ) // Need to throw an exception
        {
            TA_THROW(TA_Base_Core::DataConfigurationException("VideoTriggeringEvent data not fully specified. VideoTriggeringEvent cannot be written to database",
                                                         fieldNames));
        }

        // Write the VideoTriggeringEvent to the database

        // Need to check if this is a new VideoTriggeringEvent or an existing VideoTriggeringEvent - 
        // as a new VideoTriggeringEvent will be inserted, while an existing VideoTriggeringEvent will be updated
        if (m_isNew) // This is a new VideoTriggeringEvent
        {
            addNewVideoTriggeringEvent();
        }
        else // This is an existing VideoTriggeringEvent
        {
            modifyExistingVideoTriggeringEvent();
        }

        // Now that the data has been written, what we hold is valid data.
        // except of course the created and modified dates
        // so force a reload to refresh the dates
        m_isValidData = false;
        m_isNew = false;

        FUNCTION_EXIT;
    }


    void VideoTriggeringEventHelper::deleteVideoTriggeringEvent(bool cascade)
    {
        FUNCTION_ENTRY("deleteVideoTriggeringEvent");

        TA_ASSERT(!m_isNew, "This VideoTriggeringEvent does not yet exist in the database, and therefore cannot be deleted");
        
        TA_Base_Core::IDatabase* databaseConnection = 
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Video_Cd, TA_Base_Core::Write);
        //std::ostringstream sql;
        
        if (!cascade)
        {
			//nothing references this table
        }

        // Good to go...
      /*  sql.str("");
        sql << "delete from " << TRIGGERING_EVENTS_TABLE
            << " where " << PKEY_COL << " = " << m_pkey;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), VI_STATION_ID_STD_DELETE_62401, m_pkey);
//		std::string strSql  = databaseConnection->prepareSQLStatement(VI_STATION_ID_DELETE_62401, m_pkey);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, VI_STATION_ID_DELETE_62401, m_pkey);

        databaseConnection->executeModification(strSql);

        FUNCTION_EXIT;
    }


    void VideoTriggeringEventHelper::modifyExistingVideoTriggeringEvent() throw(TA_Base_Core::TransactiveException)
    {
        FUNCTION_ENTRY("modifyExistingVideoTriggeringEvent");

        LOG ( SourceInfo, DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
            "VideoTriggeringEvent id=%ul already exists. It's data will be updated.",m_pkey);

        TA_Base_Core::IDatabase* databaseConnection = 
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Video_Cd, TA_Base_Core::Write);

       /* std::ostringstream nestedSql;
        nestedSql << "select " << STATION_ID_PKEY_COL
                  << " from " << STATION_ID_TABLE
                  << " where " << STATION_ID_COL << " = " << m_stationId;*/
		// TD 15200
/*		std::ostringstream entitykeySql;
        nestedSql << "select " << E_PKEY_COL
                  << " from " << ENTITY_TABLE
                  << " where " << E_PKEY_COL << " = " << m_entitykey;
*/		
		// TD 15200

        //std::ostringstream sql;
        //sql << "update "<< TRIGGERING_EVENTS_TABLE << " set "
        //    << STATION_ID_PKEY_COL << " = (" << nestedSql.str() << "), "
        //    << EVENT_ID_COL << " = " << m_eventId << ", "
        //    << LOCATION_COL << " = '" << databaseConnection->escapeInsertString(m_location) << "', "
        //    << TRIGGERING_EVENT_COL << " = '" << databaseConnection->escapeInsertString(m_triggeringEvent) << "', "
        //    << MANUAL_CLOSING_COL << " = " << (m_manualClosing ? 1 : 0) << ", "
        //    << ENTITYKEY_COL<< " = " << m_entitykey  // TD 15200
        //    << " where " << PKEY_COL << " = " << m_pkey;
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), VI_STATION_ID_STD_UPDATE_62152, m_stationId,m_eventId,
//		std::string strSql  = databaseConnection->prepareSQLStatement(VI_STATION_ID_UPDATE_62152, m_stationId,m_eventId,
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, VI_STATION_ID_UPDATE_62152, m_stationId,m_eventId,
			databaseConnection->escapeInsertString(m_location), databaseConnection->escapeInsertString(m_triggeringEvent),
			(m_manualClosing ? 1 : 0), m_entitykey, m_pkey);


        // get a connection to the database
        databaseConnection->executeModification(strSql);

        // nothing more to be done here
        FUNCTION_EXIT;
    }


    void VideoTriggeringEventHelper::addNewVideoTriggeringEvent() throw(TA_Base_Core::TransactiveException)
    {
        FUNCTION_ENTRY("addNewVideoTriggeringEvent");

        LOG ( SourceInfo, DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
            "VideoTriggeringEvent %ul is a new VideoTriggeringEvent. It will be added to the database.",m_stationId);

        ////////////////////////////////////////////////////////////////////////////
        //
        // Good to go...
        //
        ////////////////////////////////////////////////////////////////////////////

        TA_Base_Core::IDatabase* databaseConnection = 
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Video_Cd, TA_Base_Core::Write);
		//int nDbServerType = databaseConnection->GetDbServerType();

        // get a new pkey
       /* std::ostringstream sql;
        sql << "select " << PKEY_SEQ << " from DUAL";*/
		//std::string strSql = defPrepareSQLStatement(nDbServerType, VI_STATION_ID_Oracle_SELECT_62058);
//		std::string strSql = databaseConnection->prepareSQLStatement(VI_STATION_ID_SELECT_62058);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, VI_STATION_ID_SELECT_62058);

        std::vector<std::string> columnNames;
        columnNames.push_back("NEXTVAL");
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql, columnNames);

        if (data->getNumRows() != 1)
        {
            TA_THROW(TA_Base_Core::DataException("Could not get primary key for new record",
                TA_Base_Core::DataException::MISSING_MANDATORY, PKEY_COL));
        }

        // get the pkey
        m_pkey = data->getUnsignedLongData(0, "NEXTVAL");

     /*   std::ostringstream nestedSql;
        nestedSql << "select " << STATION_ID_PKEY_COL
                  << " from " << STATION_ID_TABLE
                  << " where " << STATION_ID_COL << " = " << m_stationId;*/

		// TD 15200
		/*std::ostringstream entitykeySql;
        entitykeySql << "select " << E_PKEY_COL
                     << " from " << ENTITY_TABLE
                     << " where " << E_PKEY_COL << " = " << m_entitykey;*/
		// TD 15200
   
        //sql.str("");
        //sql << "insert into " << TRIGGERING_EVENTS_TABLE << " ("
        //    << PKEY_COL << ", "
        //    << STATION_ID_PKEY_COL << ", "
        //    << EVENT_ID_COL << ", "
        //    << LOCATION_COL << ", "
        //    << TRIGGERING_EVENT_COL << ", "
        //    << MANUAL_CLOSING_COL << ", "
        //    << ENTITYKEY_COL << ") " // TD 15200
        //    << "values ("
        //    << m_pkey << ", (" 
        //    << nestedSql.str() << "), " 
        //    << m_eventId << ", '" 
        //    << databaseConnection->escapeInsertString(m_location) << "', '" 
        //    << databaseConnection->escapeInsertString(m_triggeringEvent) << "', "
        //    << (m_manualClosing ? 1 : 0) << ", "
        //    << m_entitykey << ")"; // TD 15200
		/*strSql = defPrepareSQLStatement(nDbServerType, VI_STATION_ID_STD_INSERT_62302,m_pkey, m_stationId, m_eventId, 
			databaseConnection->escapeInsertString(m_location), databaseConnection->escapeInsertString(m_triggeringEvent),
			(m_manualClosing ? 1 : 0), m_entitykey);*/
//		strSql = databaseConnection->prepareSQLStatement(VI_STATION_ID_INSERT_62302, m_pkey, m_stationId, m_eventId, 

		databaseConnection->prepareSQLStatement(strSql, VI_STATION_ID_INSERT_62302, m_pkey, m_stationId, m_eventId,
			databaseConnection->escapeInsertString(m_location), databaseConnection->escapeInsertString(m_triggeringEvent),
			(m_manualClosing ? 1 : 0), m_entitykey);


        databaseConnection->executeModification(strSql);
        
        FUNCTION_EXIT;
    }

    
} // closes TA_IRS_Core

