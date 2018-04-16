/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/pa_4669/src/PaDvaMessageAccessFactory.cpp $
  * @author Jade Lee
  * @version $Revision: #2 $
  * Last modification: $DateTime: 2011/01/24 14:26:15 $
  * Last modified by: $Author: builder $
  *
  * PaDvaMessageAccessFactory is a singleton that is used to retrieve PaDvaMessage objects either from the
  * database or newly created. All PaDvaMessage objects returned will adhere to the IPaDvaMessage interface.
  */
#pragma warning(disable:4786 4290)


#include "core/data_access_interface/pa_4669/src/PaDvaMessageAccessFactory.h"


#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/pa_4669/src/PaDvaMessage.h"
#include "core/data_access_interface/pa_4669/src/ConfigPaDvaMessage.h"
#include "core/data_access_interface/pa_4669/src/PaTisMessageMap.h"
#include "core/data_access_interface/pa_4669/src/ConfigPaTisMessageMap.h"

#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"


namespace TA_Base_Core
{
    PaDvaMessageAccessFactory* PaDvaMessageAccessFactory::m_instance = 0;

    static const std::string PA_TIS_MESSAGE_MAP_TABLE_NAME		= "PA_TIS_MESSAGE_MAP";
    static const std::string PA_STATION_DVA_MESSAGE_TABLE_NAME	= "PA_DVA_MESSAGE";

    static const std::string DVA_MSG_TABLE_KEY_COL				= "PA_DVA_MESSAGE.PADMES_ID";
    static const std::string DVA_MSG_TABLE_LOCATIONKEY_COL		= "PA_DVA_MESSAGE.LOCATIONKEY";
    static const std::string DVA_MSG_TABLE_ID_COL				= "PA_DVA_MESSAGE.ID";
    static const std::string DVA_MSG_TABLE_LABEL_COL			= "PA_DVA_MESSAGE.LABEL";
    static const std::string DVA_MSG_TABLE_TYPE_COL				= "PA_DVA_MESSAGE.TYPE";

    static const std::string MAP_TABLE_KEY_COL					= "PA_TIS_MESSAGE_MAP.PKEY";
    static const std::string MAP_TABLE_LOCATIONKEY_COL			= "PA_TIS_MESSAGE_MAP.LOCATIONKEY";
    static const std::string MAP_TABLE_ID_COL					= "PA_TIS_MESSAGE_MAP.ID";
    static const std::string MAP_TABLE_TISMESSAGETAG_COL		= "PA_TIS_MESSAGE_MAP.TIS_MESSAGE_TAG";
    static const std::string MAP_TABLE_TISLIBRARYSECTION_COL	= "PA_TIS_MESSAGE_MAP.TIS_LIBRARY_SECTION";
    static const std::string MAP_TABLE_TISLIBRARYVERSION_COL	= "PA_TIS_MESSAGE_MAP.TIS_LIBRARY_VERSION";


    static const std::string PA_DVA_MESSAGE_SEQUENCE_COUNTER	= "PADMES_SEQ";

    static const std::string DVA_MSG_TABLE_DATEMODIFIED_COL		= "PA_DVA_MESSAGE.DATE_MODIFIED";
    static const std::string DVA_MSG_TABLE_DATECREATED_COL		= "PA_DVA_MESSAGE.DATE_CREATED";
    // TD 16524
    static const std::string MAP_TABLE_DATEMODIFIED_COL		= "PA_TIS_MESSAGE_MAP.DATE_MODIFIED";
    static const std::string MAP_TABLE_DATECREATED_COL		= "PA_TIS_MESSAGE_MAP.DATE_CREATED";
    // TD 16524
    static const std::string TIS_TABLE_NAME = "TI_PREDEFINED_MESSAGE";
    static const std::string TIS_KEY_COL_NAME = "TIPMES_ID";
    static const std::string TIS_MESSAGE_TAG_COL = "MESSAGE_TAG";
    static const std::string TIS_LIBRARY_SECTION_COL = "LIBRARY_SECTION";
    static const std::string TIS_DESCRIPTION_COL = "DESCRIPTION";

    static const std::string TIS_LIBRARY_VER_KEY_COL = "TIPMLI_ID";
    static const std::string TIS_LIBRARY_VER_TABLE = "TI_PREDEFINED_MESSAGE_LIBRARY";
    static const std::string TIS_LIBRARY_VERSION_COL = "VERSION";

    PaDvaMessageAccessFactory::TisMessage::TisMessage()
        :
        pKey( TA_Base_Core::DatabaseKey::getInvalidKey() ),
        tisMessageTag( TA_Base_Core::DatabaseKey::getInvalidKey() ),
        tisLibrarySection( TA_Base_Core::DatabaseKey::getInvalidKey() ),
        tisLibraryVersion( TA_Base_Core::DatabaseKey::getInvalidKey() )
    {
    }

    PaDvaMessageAccessFactory& PaDvaMessageAccessFactory::getInstance()
    {
        if( m_instance == 0 )
        {
            m_instance = new PaDvaMessageAccessFactory();
        }
        return *m_instance;
    }


    void PaDvaMessageAccessFactory::removeInstance( )
    {
        if ( m_instance != NULL )
        {
            delete m_instance;
            m_instance = NULL;
        }
    }


    IPaDvaMessage* PaDvaMessageAccessFactory::getPaDvaMessage( const unsigned long key, const bool readWrite )
    {
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection =
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase( Pa_Cd, Read );

        // create the SQL string to retrieve the name of the PaDvaMessage
        // this is a check to ensure an PaDvaMessage with the specified
        // PAZONE_ID actually exists.
        std::ostringstream sql;
        sql << "select "
            << DVA_MSG_TABLE_KEY_COL << ", "
            << DVA_MSG_TABLE_LOCATIONKEY_COL << ", "
            << DVA_MSG_TABLE_ID_COL << ", "
            << DVA_MSG_TABLE_LABEL_COL << ", "
            << DVA_MSG_TABLE_TYPE_COL << ", "
            << MAP_TABLE_KEY_COL << ", "
            << MAP_TABLE_LOCATIONKEY_COL << ", "
            << MAP_TABLE_ID_COL << ", "
            << MAP_TABLE_TISMESSAGETAG_COL << ", "
            << MAP_TABLE_TISLIBRARYSECTION_COL << ", "
            << MAP_TABLE_TISLIBRARYVERSION_COL << ", TO_CHAR("
            << DVA_MSG_TABLE_DATECREATED_COL << ", 'YYYYMMDDHH24MISS'), TO_CHAR("
            << DVA_MSG_TABLE_DATEMODIFIED_COL << ", 'YYYYMMDDHH24MISS')"
            << " from "
            << PA_TIS_MESSAGE_MAP_TABLE_NAME
            << " join "
            << PA_STATION_DVA_MESSAGE_TABLE_NAME
            << " on "
            << DVA_MSG_TABLE_LOCATIONKEY_COL << " = " << MAP_TABLE_LOCATIONKEY_COL
            << " and "
            << DVA_MSG_TABLE_ID_COL << " = " << MAP_TABLE_ID_COL
            << " where "
            << DVA_MSG_TABLE_KEY_COL << " = " << key << std::endl;

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back( DVA_MSG_TABLE_KEY_COL );

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery( sql.str(), columnNames );

        // Bring the DataException into the namespace
        using TA_Base_Core::DataException;

        if ( 0 == data->getNumRows() ) // No entry found with the specified pkey
        {
            // clean up the pointer
            delete data;
            data = NULL;

            char reasonMessage[256] = {0};
            sprintf( reasonMessage, "No data found for pkey = %lu", key );
            TA_THROW( DataException( reasonMessage, DataException::NO_VALUE, "" ) );
        }
        else if ( 1 < data->getNumRows() ) // More than one entry found for the pkey
        {
            // clean up the pointer
            delete data;
            data = NULL;

            char reasonMessage[256] = {0};
            sprintf( reasonMessage, "More than one entry found for pkey = %lu", key );
            TA_THROW( DataException( reasonMessage, DataException::NOT_UNIQUE, "" ) );
        }

        // Don't need to do anything with the data - the check was only that a single
        // item was returned.
        // Now that we're finished with the IData object, we need to delete it.
        delete data;
        data = NULL;

        // Create the PaDvaMessage object to represent this object. Initially only populate it
        // with id. We will populate the rest of the data on demand.
        IPaDvaMessage* thePaDvaMessage;

        // decide which PaDvaMessage object to return
        if ( readWrite ) // need a config PaDvaMessage
        {
            thePaDvaMessage = new ConfigPaDvaMessage( key );
        }
        else // need a standard PaDvaMessage
        {
            thePaDvaMessage = new PaDvaMessage( key );
        }

        // Return the PaDvaMessage pointer. The class that recieves this pointer is responsible
        // for deleting it.
        return thePaDvaMessage;
    }


    IPaDvaMessages PaDvaMessageAccessFactory::getAllPaDvaMessages( const bool readWrite )
    {
        return getPaDvaMessagesReturnedFromStatement( "", readWrite );
    }


    IPaDvaMessages PaDvaMessageAccessFactory::getPaDvaMessagesByLocationKey( unsigned long locationKey, const bool readWrite )
    {
        std::ostringstream sql;
        sql << DVA_MSG_TABLE_LOCATIONKEY_COL << " = " << locationKey;

        return getPaDvaMessagesReturnedFromStatement( sql.str(), readWrite );
    }

    IPaTisMessageMaps PaDvaMessageAccessFactory::getAllPaDvaMessagesForConfigEditor( const bool readWrite /* = false */ )
    {
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase( Pa_Cd, Read );

        // create the SQL string to retrieve the data from the pa_station_dva_message and the
        // pa_tis_message_map tables based upon the primary key of a PaDvaMessage record
        std::ostringstream sql;
        sql << "select "
            << DVA_MSG_TABLE_KEY_COL << ", "
            << DVA_MSG_TABLE_LOCATIONKEY_COL << ", "
            << DVA_MSG_TABLE_ID_COL << ", "
            << DVA_MSG_TABLE_LABEL_COL << ", "
            << DVA_MSG_TABLE_TYPE_COL << ", "
            << MAP_TABLE_KEY_COL << ", "
            << MAP_TABLE_LOCATIONKEY_COL << ", "
            << MAP_TABLE_ID_COL << ", "
            << MAP_TABLE_TISMESSAGETAG_COL << ", "
            << MAP_TABLE_TISLIBRARYSECTION_COL << ", "
            << MAP_TABLE_TISLIBRARYVERSION_COL << ", TO_CHAR("
            << MAP_TABLE_DATECREATED_COL << ", 'YYYYMMDDHH24MISS'), TO_CHAR(" 	// TD 16524
            << MAP_TABLE_DATEMODIFIED_COL << ", 'YYYYMMDDHH24MISS')" 	// TD 16524
            << " from "
            << PA_TIS_MESSAGE_MAP_TABLE_NAME
            << " join "
            << PA_STATION_DVA_MESSAGE_TABLE_NAME
            << " on "
            << DVA_MSG_TABLE_LOCATIONKEY_COL << " = " << MAP_TABLE_LOCATIONKEY_COL
            << " and "
            << DVA_MSG_TABLE_ID_COL << " = " << MAP_TABLE_ID_COL
            << std::endl;
        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back( DVA_MSG_TABLE_KEY_COL );
        columnNames.push_back( DVA_MSG_TABLE_LOCATIONKEY_COL );
        columnNames.push_back( DVA_MSG_TABLE_ID_COL );
        columnNames.push_back( DVA_MSG_TABLE_LABEL_COL );
        columnNames.push_back( DVA_MSG_TABLE_TYPE_COL );
        columnNames.push_back( MAP_TABLE_KEY_COL );
        columnNames.push_back( MAP_TABLE_LOCATIONKEY_COL );
        columnNames.push_back( MAP_TABLE_ID_COL );
        columnNames.push_back( MAP_TABLE_TISMESSAGETAG_COL );
        columnNames.push_back( MAP_TABLE_TISLIBRARYSECTION_COL );
        columnNames.push_back( MAP_TABLE_TISLIBRARYVERSION_COL );
        // TD 16524
        columnNames.push_back( MAP_TABLE_DATECREATED_COL );
        columnNames.push_back( MAP_TABLE_DATEMODIFIED_COL );
        // TD 16524

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it (handled by auto pointer)
        TA_Base_Core::IData* data = databaseConnection->executeQuery( sql.str(), columnNames );

        // create the vector of pointers to IPaTisMessageMap
        IPaTisMessageMaps returnValue;

        do
        {
            unsigned long rowCount = data->getNumRows();

            for ( unsigned long i = 0; i < rowCount; ++i )
            {
                //                unsigned long key = data->getUnsignedLongData(i, PKEY_COL);

                if ( readWrite == true )
                {
                    returnValue.push_back( new ConfigPaTisMessageMap( i, *data ) );
                }
                else
                {
                    returnValue.push_back( new PaTisMessageMap( i, *data ) );
                }
            }

            delete data;
            data = NULL;
        }
        while( databaseConnection->moreData( data ) == true );


        return returnValue;
    }

    IPaDvaMessages PaDvaMessageAccessFactory::getPaDvaMessagesReturnedFromStatement( const std::string& whereClause,  const bool readWrite )
    {
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase( Pa_Cd, Read );

        // create the SQL string to retrieve the data from the pa_station_dva_message and the
        // pa_tis_message_map tables based upon the primary key of a PaDvaMessage record
        std::ostringstream sql;
        sql << "select "
            << DVA_MSG_TABLE_KEY_COL << ", "
            << DVA_MSG_TABLE_LOCATIONKEY_COL << ", "
            << DVA_MSG_TABLE_ID_COL << ", "
            << DVA_MSG_TABLE_LABEL_COL << ", "
            << DVA_MSG_TABLE_TYPE_COL << ", "
            << MAP_TABLE_KEY_COL << ", "
            << MAP_TABLE_LOCATIONKEY_COL << ", "
            << MAP_TABLE_ID_COL << ", "
            << MAP_TABLE_TISMESSAGETAG_COL << ", "
            << MAP_TABLE_TISLIBRARYSECTION_COL << ", "
            << MAP_TABLE_TISLIBRARYVERSION_COL << ", TO_CHAR("
            << DVA_MSG_TABLE_DATECREATED_COL << ", 'YYYYMMDDHH24MISS'), TO_CHAR("
            << DVA_MSG_TABLE_DATEMODIFIED_COL << ", 'YYYYMMDDHH24MISS')"
            << " from "
            << PA_TIS_MESSAGE_MAP_TABLE_NAME
            << " join "
            << PA_STATION_DVA_MESSAGE_TABLE_NAME
            << " on "
            << DVA_MSG_TABLE_LOCATIONKEY_COL << " = " << MAP_TABLE_LOCATIONKEY_COL
            << " and "
            << DVA_MSG_TABLE_ID_COL << " = " << MAP_TABLE_ID_COL;
        if( "" != whereClause )
        {
            sql << " where " << whereClause;
        }
        sql << std::endl;
        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back( DVA_MSG_TABLE_KEY_COL );
        columnNames.push_back( DVA_MSG_TABLE_LOCATIONKEY_COL );
        columnNames.push_back( DVA_MSG_TABLE_ID_COL );
        columnNames.push_back( DVA_MSG_TABLE_LABEL_COL );
        columnNames.push_back( DVA_MSG_TABLE_TYPE_COL );
        columnNames.push_back( MAP_TABLE_KEY_COL );
        columnNames.push_back( MAP_TABLE_LOCATIONKEY_COL );
        columnNames.push_back( MAP_TABLE_ID_COL );
        columnNames.push_back( MAP_TABLE_TISMESSAGETAG_COL );
        columnNames.push_back( MAP_TABLE_TISLIBRARYSECTION_COL );
        columnNames.push_back( MAP_TABLE_TISLIBRARYVERSION_COL );
        columnNames.push_back( DVA_MSG_TABLE_DATECREATED_COL );
        columnNames.push_back( DVA_MSG_TABLE_DATEMODIFIED_COL );


        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it (handled by auto pointer)
        TA_Base_Core::IData* data = databaseConnection->executeQuery( sql.str(), columnNames );

        // create the vector of pointers to ITrackElement
        IPaDvaMessages returnValue;

        do
        {
            unsigned long rowCount = data->getNumRows();

            for ( unsigned long i = 0; i < rowCount; ++i )
            {
//                unsigned long key = data->getUnsignedLongData(i, PKEY_COL);

                if ( readWrite == true )
                {
                    returnValue.push_back( new ConfigPaDvaMessage( i, *data ) );
                }
                else
                {
                    returnValue.push_back( new PaDvaMessage( i, *data ) );
                }
            }

            delete data;
            data = NULL;
        }
        while( databaseConnection->moreData( data ) == true );


        return returnValue;



//        return DatabaseQueryHelper::getDatabaseRecordsReturnedFromStatement
//                    <IPaDvaMessages, ConfigPaDvaMessage, PaDvaMessage>
//                    (PaDvaMessageHelper::getBasicQueryData(), whereClause, readWrite);
    }


    IConfigPaDvaMessage* PaDvaMessageAccessFactory::createPaDvaMessage()
    {
        TA_ASSERT( false, "this function should be call when use distribute DB" );
        return new ConfigPaDvaMessage();
    }


    IConfigPaDvaMessage* PaDvaMessageAccessFactory::copyPaDvaMessage( const IConfigPaDvaMessage* paDvaMessageToCopy )
    {
        TA_ASSERT( false, "this function should be call when use distribute DB" );

//        FUNCTION_ENTRY("copyPaDvaMessage");
//
//        TA_ASSERT(paDvaMessageToCopy !=  NULL, "The PaDvaMessage to copy was NULL");
//
//        const ConfigPaDvaMessage* cast = dynamic_cast<const ConfigPaDvaMessage*>(paDvaMessageToCopy);
//
//        TA_ASSERT(cast != NULL, "PaDvaMessage passed could not be converted into a ConfigPaDvaMessage");
//
//        FUNCTION_EXIT;
//        return new ConfigPaDvaMessage(*cast);
        return new ConfigPaDvaMessage;

    }


    std::vector<PaDvaMessageAccessFactory::TisMessage> PaDvaMessageAccessFactory::enumerateTisMessages()
    {
        std::vector<std::string> columnNames;

        std::string msgTable( "a." );
        std::string verTable( "b." );

        columnNames.push_back( msgTable + TIS_KEY_COL_NAME );
        columnNames.push_back( msgTable + TIS_MESSAGE_TAG_COL );
        columnNames.push_back( msgTable + TIS_LIBRARY_SECTION_COL );
        columnNames.push_back( msgTable + TIS_DESCRIPTION_COL );
        columnNames.push_back( verTable + TIS_LIBRARY_VERSION_COL );

        unsigned int i;
        std::ostringstream sql;
        sql << "select ";
        for ( i = 0; i < columnNames.size(); i ++ )
        {
            if ( i > 0 )
            {
                sql << ", ";
            }
            sql << columnNames[i];
        }

        // Join with the TIS message version table, to get version information
        sql << " from " << TIS_TABLE_NAME << " a, " << TIS_LIBRARY_VER_TABLE << " b"
            << " where a." << TIS_LIBRARY_VER_KEY_COL << " = " << "b." << TIS_LIBRARY_VER_KEY_COL;

        TA_Base_Core::IDatabase* databaseConnection =
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase( Pa_Cd, Read );

        TA_Base_Core::IData *data = NULL;
        data = databaseConnection->executeQuery( sql.str(), columnNames );
        std::vector<TisMessage> result;

        if ( data == NULL )
        {
            // Return empty set in the event of no rows
            return result;
        }

        do
        {
            unsigned long rowCount = 0;
            rowCount = data->getNumRows();
            try
            {
                for ( unsigned long i = 0 ; i < rowCount ; ++i )
                {
                    TisMessage msg;

                    msg.pKey = data->getUnsignedLongData( i, msgTable + TIS_KEY_COL_NAME );

                    if ( !data->isNull( i, msgTable + TIS_MESSAGE_TAG_COL ) )
                    {
                        msg.tisMessageTag = data->getUnsignedLongData( i, msgTable + TIS_MESSAGE_TAG_COL );
                    }
                    if ( !data->isNull( i, msgTable + TIS_LIBRARY_SECTION_COL ) )
                    {
                        msg.tisLibrarySection = data->getUnsignedLongData( i, msgTable + TIS_LIBRARY_SECTION_COL );
                    }
                    if ( !data->isNull( i, msgTable + TIS_DESCRIPTION_COL ) )
                    {
                        msg.description = data->getStringData( i, msgTable + TIS_DESCRIPTION_COL );
                    }

                    // One of the few fields that isn't possibly null
                    msg.tisLibraryVersion = data->getUnsignedLongData( i, verTable + TIS_LIBRARY_VERSION_COL );

                    result.push_back( msg );
                }
            }
            catch( ... )
            {
                delete data;
                data = NULL;
                throw;
            }
            delete data;
            data = NULL;
        }
        while ( databaseConnection->moreData( data ) == true );

        return result;
    }

} // closes TA_Base_Core


