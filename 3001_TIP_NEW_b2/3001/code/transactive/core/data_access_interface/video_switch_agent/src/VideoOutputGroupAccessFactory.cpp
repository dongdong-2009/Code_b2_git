/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/core/data_access_interface/video_switch_agent/src/VideoOutputGroupAccessFactory.cpp $
  * @author Katherine Thomson
  * @version $Revision: #4 $
  * Last modification: $DateTime: 2012/12/27 17:54:14 $
  * Last modified by: $Author: weikun.lin $
  * 
  * VideoOutputGroupAccessFactory is a singleton that is used to retrieve objects either from the
  * database or newly created. All objects returned will adhere to the IVideoOutputGroup interface.
  */

#pragma warning(disable:4786)

#include <sstream>

#include "core/data_access_interface/video_switch_agent/src/VideoOutputGroupAccessFactory.h"
#include "core/data_access_interface/video_switch_agent/src/DAIVideoOutputGroup.h"
#include "core/data_access_interface/video_switch_agent/src/ConfigVideoOutputGroup.h"
#include "core/data_access_interface/video_switch_agent/src/VideoOutputGroupTable.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_Core
{
	// Initialise statics
		
    TA_Base_Core::ReEntrantThreadLockable VideoOutputGroupAccessFactory::m_singletonLock;
	VideoOutputGroupAccessFactory* VideoOutputGroupAccessFactory::m_theClassInstance = NULL;


    //
    // getInstance
    //
	VideoOutputGroupAccessFactory& VideoOutputGroupAccessFactory::getInstance()
	{
	    TA_Base_Core::ThreadGuard guard( m_singletonLock );

		// If guard acquired then make sure the singleton is still NULL.

		if ( m_theClassInstance == NULL )
		{
			// Create the one & only object
			
            m_theClassInstance = new VideoOutputGroupAccessFactory();
		}
		
		return *m_theClassInstance;
	}

    
    //
    // getVideoOutputGroup
    //
    IVideoOutputGroup* VideoOutputGroupAccessFactory::getVideoOutputGroup( 
        unsigned long videoOutputGroupKey, bool isReadWrite )
    {
        // Get a connection to the database
        
        TA_Base_Core::IDatabase* databaseConnection = 
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Video_Cd, TA_Base_Core::Read);

        // Check that this VideoOutputGroup exists in the table.

      /*  std::stringstream selectSql;
        selectSql << "select " << TA_VideoSwitchAgentDAI::VIDEO_OUTPUT_GROUP_KEY_COLUMN;
        selectSql << " from " << TA_VideoSwitchAgentDAI::VIDEO_OUTPUT_GROUP_TABLE_NAME;
        selectSql << " where " << TA_VideoSwitchAgentDAI::VIDEO_OUTPUT_GROUP_KEY_COLUMN;
        selectSql << "=" << videoOutputGroupKey;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), VI_VIDEO_OUTPUT_GROUP_STD_SELECT_62501, videoOutputGroupKey);
//		std::string strSql  = databaseConnection->prepareSQLStatement(VI_VIDEO_OUTPUT_GROUP_SELECT_62501, videoOutputGroupKey);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, VI_VIDEO_OUTPUT_GROUP_SELECT_62501, videoOutputGroupKey);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned TA_Base_Core::IData object when we're done with it,
        // so make it a std::auto_ptr so it is automatically cleaned up.

        std::vector< std::string > columnNames;
        columnNames.push_back( TA_VideoSwitchAgentDAI::VIDEO_OUTPUT_GROUP_KEY_COLUMN );
        std::auto_ptr< TA_Base_Core::IData > data = std::auto_ptr< TA_Base_Core::IData >( 
            databaseConnection->executeQuery( strSql, columnNames ) );
        
        if ( 0 == data->getNumRows() ) // No entry found with the specified pkey
        {
            std::stringstream error;
            error << "No data found for VideoOutputGroup with the key " << videoOutputGroupKey;			
            TA_THROW( TA_Base_Core::DataException( error.str().c_str(), TA_Base_Core::DataException::NO_VALUE, 
                TA_VideoSwitchAgentDAI::VIDEO_OUTPUT_GROUP_KEY_COLUMN ));
        }

        // Create the VideoOutputGroup object to represent this object. Initially only populate it
        // with VideoOutputGroup key. We will populate the rest of the data on demand.

        IVideoOutputGroup* theVideoOutputGroup;
        
        // Decide which type of VideoOutputGroup object to return.

        if ( isReadWrite ) // Need a config object.
        {
            theVideoOutputGroup = new ConfigVideoOutputGroup( videoOutputGroupKey );
        }
        else // Need a standard object.
        {
            theVideoOutputGroup = new TA_VideoSwitchAgentDAI::DAIVideoOutputGroup( videoOutputGroupKey );
        }

        // Return the appropriate object. 
        // The class that recieves this pointer is responsible for deleting it.

        return theVideoOutputGroup;
    }


    //
    // getAllVideoOutputGroups
    //
    std::vector< IVideoOutputGroup* > VideoOutputGroupAccessFactory::getAllVideoOutputGroups( bool isReadWrite )
    {
        // Get a connection to the database.

        TA_Base_Core::IDatabase* databaseConnection = 
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Video_Cd, TA_Base_Core::Read);

        // Get all the VideoOutputGroup keys that exist in the table.

      /*  std::stringstream selectSql;
        selectSql << "select " << TA_VideoSwitchAgentDAI::VIDEO_OUTPUT_GROUP_KEY_COLUMN;
        selectSql << " from " << TA_VideoSwitchAgentDAI::VIDEO_OUTPUT_GROUP_TABLE_NAME;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), VI_VIDEO_OUTPUT_GROUP_STD_SELECT_62502);
//		std::string strSql  = databaseConnection->prepareSQLStatement(VI_VIDEO_OUTPUT_GROUP_SELECT_62502);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, VI_VIDEO_OUTPUT_GROUP_SELECT_62502);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned TA_Base_Core::IData object when we're done with it,
        // so make it a std::auto_ptr so it is automatically cleaned up.

        std::vector< std::string > columnNames;
        columnNames.push_back( TA_VideoSwitchAgentDAI::VIDEO_OUTPUT_GROUP_KEY_COLUMN );
        std::auto_ptr< TA_Base_Core::IData > data = std::auto_ptr< TA_Base_Core::IData >(
            databaseConnection->executeQuery( strSql, columnNames ) );
        
        if ( 0 == data->getNumRows() ) // No entry found with the specified pkey
        {
            std::stringstream error;
            error << "There are no VideoOutputGroups configured in the database table ";
            error << TA_VideoSwitchAgentDAI::VIDEO_OUTPUT_GROUP_TABLE_NAME;
            TA_THROW( TA_Base_Core::DataException( error.str().c_str(), TA_Base_Core::DataException::NO_VALUE, "" ));
        }

        // Create the vector of pointers to IVideoOutputGroups.

        std::vector< IVideoOutputGroup* > videoOutputGroups;
        if ( isReadWrite )  // Need a config objects.
        {
            for ( unsigned long i = 0; i < data->getNumRows(); ++i )
            {                
                videoOutputGroups.push_back( new ConfigVideoOutputGroup( 
                    data->getUnsignedLongData( i, TA_VideoSwitchAgentDAI::VIDEO_OUTPUT_GROUP_KEY_COLUMN ) ) );
            }
        }
        else // Need standard objects.
        {
            for ( unsigned long i = 0; i < data->getNumRows(); ++i )
            {                
                videoOutputGroups.push_back( new TA_VideoSwitchAgentDAI::DAIVideoOutputGroup( 
                    data->getUnsignedLongData( i, TA_IRS_Core::TA_VideoSwitchAgentDAI::VIDEO_OUTPUT_GROUP_KEY_COLUMN ) ) );
            }
        }

        return videoOutputGroups;
    }

    
    //
    // createVideoOutputGroup
    //
    IConfigVideoOutputGroup* VideoOutputGroupAccessFactory::createVideoOutputGroup( unsigned long videoOutputGroupKey )
    {            
        return new ConfigVideoOutputGroup( videoOutputGroupKey );    
    }

} // closes TA_IRS_Core


