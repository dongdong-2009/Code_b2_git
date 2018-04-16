/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/video_switch_agent/src/VideoOutputGroupHelper.cpp $
  * @author Katherine Thomson
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * VideoOutputGroupHelper is an object that is held by VideoOutputGroup and ConfigVideoOutputGroup objects. 
  * It contains all data used by TypePluginMatrices, and manipulation
  * methods for the data. It helps avoid re-writing code for both VideoOutputGroup and ConfigVideoOutputGroup.
  */

#pragma warning(disable:4786)
#pragma warning(disable:4503)  // warning C4503: decorated type length exceeded, type was truncated

#include <vector>
#include <memory>

#include "core/data_access_interface/video_switch_agent/src/VideoOutputGroupHelper.h"
#include "core/data_access_interface/video_switch_agent/src/VideoOutputGroupAccessFactory.h"
#include "core/data_access_interface/video_switch_agent/src/VideoOutputGroupTable.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/exceptions/src/DataConfigurationException.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_Core
{
    //
    // VideoOutputGroupHelper
    //
    VideoOutputGroupHelper::VideoOutputGroupHelper( unsigned long videoOutputGroupKey )
        : m_isValidData( false ),
          m_key( videoOutputGroupKey ),
          m_isNew( true )
    {
		m_videoOutputs.clear();
    }


    //
    // VideoOutputGroupHelper (Copy Constructor)
    // 
	VideoOutputGroupHelper::VideoOutputGroupHelper( const VideoOutputGroupHelper& theVideoOutputGroupHelper )
		: m_isValidData( theVideoOutputGroupHelper.m_isValidData ),
          m_key( theVideoOutputGroupHelper.m_key ),
          m_videoOutputs( theVideoOutputGroupHelper.m_videoOutputs ),
          m_isNew( theVideoOutputGroupHelper.m_isNew )
	{
        // Do nothing.
	}


    //
    // getKey
    //
    unsigned long VideoOutputGroupHelper::getKey()
    {
        // This method cannot be called until the key has been set.

        TA_ASSERT( 0 != m_key, "The VideoOutputGroup key has not been initialised." );
        return m_key;
    }


    //
    // getVideoOutputs
    //
    std::map< unsigned short, unsigned long > VideoOutputGroupHelper::getVideoOutputs()
    {
        if ( !m_isValidData )
        {
            reload();
        }
        return m_videoOutputs;
    }


    //
    // setVideoOutputs
    //
    void VideoOutputGroupHelper::setVideoOutputs( const std::map< unsigned short, unsigned long >& videoOutputs )
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "VideoOutputGroupHelper::setType" );

        if ( !m_isValidData )
        {
            reload();
        }
        m_videoOutputs = videoOutputs;
        
        LOG_FUNCTION_EXIT( SourceInfo, "VideoOutputGroupHelper::setType" );
    }


    //
    // invalidate
    //
    void VideoOutputGroupHelper::invalidate()
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "VideoOutputGroupHelper::invalidate" );

        // This method cannot be called until the VideoOutputGroupKey and 
        // the VideoOutput position have been set.

        TA_ASSERT( !m_isNew, "Attempted to call invalidate() on a new VideoOutputGroup entry.");
        
        m_isValidData = false;

        LOG_FUNCTION_EXIT( SourceInfo, "VideoOutputGroupHelper::invalidate" );
    }


    //
    // reload
    //
    void VideoOutputGroupHelper::reload()
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "VideoOutputGroupHelper::reload" );
        
        // Ensure the VideoOutputGroup entry has already been written to the database. 
        // It doesn't make any sense to reload a VideoOutputGroup entry that doesn't exist in the database.
        
        TA_ASSERT( 0 != m_key, 
            "This VideoOutputGroup does not yet exist in the database. Therefore it cannot be reloaded.");
        
        m_videoOutputs.clear();

        // Get a connection to the database.
        
        TA_Base_Core::IDatabase* databaseConnection = 
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Video_Cd, TA_Base_Core::Read);

        // Create the SQL string to retrieve the data for the VideoOutputGroup
        // based upon the VideoOutputGroup key.

        std::stringstream selectSql;
        selectSql << "select " << TA_VideoSwitchAgentDAI::POSITION_COLUMN << ", ";
        selectSql << TA_VideoSwitchAgentDAI::VIDEO_OUTPUT_KEY_COLUMN;
        selectSql << " from " << TA_VideoSwitchAgentDAI::VIDEO_OUTPUT_GROUP_TABLE_NAME;
        selectSql << " where " << TA_VideoSwitchAgentDAI::VIDEO_OUTPUT_GROUP_KEY_COLUMN;
        selectSql << "=" << m_key;
        selectSql << " order by " << TA_VideoSwitchAgentDAI::POSITION_COLUMN;

        // Execute the query. 
        //
        // The executeQuery() method can throw a DatabaseException, as documented 
        // in the comment of this method.
        //
        // We are responsible for deleting the returned TA_Base_Core::IData object when we're done with it,
        // so make it an std::auto_ptr, then it will automatically be cleaned up.

        std::vector< std::string > columnNames;
        columnNames.push_back( TA_VideoSwitchAgentDAI::POSITION_COLUMN );
        columnNames.push_back( TA_VideoSwitchAgentDAI::VIDEO_OUTPUT_KEY_COLUMN );
        std::auto_ptr< TA_Base_Core::IData > data = std::auto_ptr< TA_Base_Core::IData >( 
            databaseConnection->executeQuery( selectSql.str(), columnNames ) );

        // Check that at least one entry was found for this key.  Note that for this
        // class it is generally expected that more than one entry will be found -
        // in fact we are expecting one entry per position in the VideoOutputGroup, except
        // in the case where some positions are empty.
        
        int numRows = data->getNumRows();
        if ( 0 == numRows ) // No entry found with the specified key.
        {
            std::stringstream error;
            error << "The VideoOutputGroup with the key " << m_key;
            error << " was not found in the database table ";
            error << TA_VideoSwitchAgentDAI::VIDEO_OUTPUT_GROUP_TABLE_NAME << ".";
            TA_THROW( TA_Base_Core::DataException( error.str().c_str(), TA_Base_Core::DataException::NO_VALUE, "" ));
        }

        // Assign the data as appropriate to the member variables.
        // These calls can throw TA_Base_Core::DataExceptions, as documented in the comment of this method.

        unsigned short position = 0;
        unsigned long videoOutputKey = 0;
        for( int i = 0; i < numRows; i++ )
        {
            position = data->getIntegerData( i, TA_VideoSwitchAgentDAI::POSITION_COLUMN );
            videoOutputKey = data->getUnsignedLongData( i, TA_VideoSwitchAgentDAI::VIDEO_OUTPUT_KEY_COLUMN );
            
            // Throw an exception if there are multiple entries for the same position.

            if ( m_videoOutputs.end() != m_videoOutputs.find( position ) )
            {
                std::stringstream error;
                error << "There are multiple VideoOutputs assigned to position " << position;
                error << " for the VideoOutputGroup with the key " << m_key;
                error << " in the database table ";
                error << TA_VideoSwitchAgentDAI::VIDEO_OUTPUT_GROUP_TABLE_NAME << ".";
                TA_THROW( TA_Base_Core::DataException( error.str().c_str(), TA_Base_Core::DataException::NOT_UNIQUE, 
                    TA_VideoSwitchAgentDAI::POSITION_COLUMN ));
            }

            // If there is only one entry for this position so far, add it to the map.

            m_videoOutputs[ position ] = videoOutputKey;
        }

        // The data is now valid.

        m_isValidData = true;

        LOG_FUNCTION_EXIT( SourceInfo, "VideoOutputGroupHelper::reload" );
    }


    //
    // writeVideoOutputGroupData
    //
    void VideoOutputGroupHelper::writeVideoOutputGroupData()
    {       
        LOG_FUNCTION_ENTRY( SourceInfo, "VideoOutputGroupHelper::writeVideoOutputGroupData" );     
        
        // If there are no VideoOutputs specified for this VideoOutputGroup, 
        // then we don't need to write anything.

        if ( m_videoOutputs.empty() )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                "There are no VideoOutputs assigned to VideoOutputGroup %lu. No data will be written to the database.",
                m_key );
            return;
        }

        if ( m_isNew )
        {
            insertNewVideoOutputGroup();
        }
        else
        {
            updateExistingVideoOutputGroup();
        }

        // Now that the data has been written, what we hold is valid data.

        m_isValidData = true;
        
        LOG_FUNCTION_EXIT( SourceInfo, "VideoOutputGroupHelper::writeVideoOutputGroupData" );
    }


    //
    // deleteVideoOutputGroup
    //
    void VideoOutputGroupHelper::deleteVideoOutputGroup()
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "VideoOutputGroupHelper::deleteVideoOutputGroup" );
        TA_ASSERT( 0 != m_key, "This VideoOutputGroup does not yet exist in the database, and therefore cannot be deleted." );
        
        // Get a connection to the database.

        TA_Base_Core::IDatabase* databaseConnection = 
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Video_Cd, TA_Base_Core::Write);
        
        std::stringstream deleteSql;
        deleteSql << "delete from " << TA_VideoSwitchAgentDAI::VIDEO_OUTPUT_GROUP_TABLE_NAME;
        deleteSql << " where " << TA_VideoSwitchAgentDAI::VIDEO_OUTPUT_GROUP_KEY_COLUMN;
        deleteSql << "=" << m_key;
        databaseConnection->executeModification( deleteSql.str() );

        m_key = 0;
        m_videoOutputs.clear();

        LOG_FUNCTION_EXIT( SourceInfo, "VideoOutputGroupHelper::deleteVideoOutputGroup" );
    }


    //
    // insertNewVideoOutputGroup
    //
    void VideoOutputGroupHelper::insertNewVideoOutputGroup()
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "VideoOutputGroupHelper::insertNewVideoOutputGroup" );

        // If there are no VideoOutputs specified for this VideoOutputGroup, 
        // then we don't need to write anything.

        if ( m_videoOutputs.empty() )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                "There are no VideoOutputs assigned to VideoOutputGroup %lu. No data will be written to the database.",
                m_key );
            return;
        }

        // Get a connection to the database

        TA_Base_Core::IDatabase* databaseConnection = 
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Video_Cd, TA_Base_Core::Write);

        // Write the VideoOutputGroup to the database.
  
        // First check if there is already a VideoOutputGroup with this key in the database.
        // Use a std::auto_ptr for the TA_Base_Core::IData so it is automatically cleaned up.
        
        std::stringstream selectSql;
        selectSql << "select " << TA_VideoSwitchAgentDAI::POSITION_COLUMN;
		selectSql << ", " << TA_VideoSwitchAgentDAI::VIDEO_OUTPUT_KEY_COLUMN;
        selectSql << " from " << TA_VideoSwitchAgentDAI::VIDEO_OUTPUT_GROUP_TABLE_NAME;
        selectSql << " where " << TA_VideoSwitchAgentDAI::VIDEO_OUTPUT_GROUP_KEY_COLUMN;
        selectSql << "=" << m_key;

        std::vector< std::string > columnNames;
        columnNames.push_back( TA_VideoSwitchAgentDAI::POSITION_COLUMN );
        columnNames.push_back( TA_VideoSwitchAgentDAI::VIDEO_OUTPUT_GROUP_KEY_COLUMN );
        std::auto_ptr< TA_Base_Core::IData > data = std::auto_ptr< TA_Base_Core::IData >( 
            databaseConnection->executeQuery( selectSql.str(), columnNames ) );

		// The position and VideoOutput key will be added as required.

		std::stringstream insertSql;
        insertSql << "insert into ";
        insertSql << TA_VideoSwitchAgentDAI::VIDEO_OUTPUT_GROUP_TABLE_NAME << " (";
        insertSql << TA_VideoSwitchAgentDAI::VIDEO_OUTPUT_GROUP_KEY_COLUMN << ", ";
        insertSql << TA_VideoSwitchAgentDAI::POSITION_COLUMN << ", ";
        insertSql << TA_VideoSwitchAgentDAI::VIDEO_OUTPUT_KEY_COLUMN << ") values(";
        insertSql << m_key << ", ";

        // If there is already an entry for this VideoOutputGroup, throw an exception.

        unsigned short numRows = static_cast< unsigned short >( data->getNumRows() );
        if ( 0 != numRows )     // This is an existing VideoOutputGroup.
        {
            std::stringstream error;
            error << "A VideoOutputGroup already exists with database key = " << m_key;
            error << " in the database table ";
            error << TA_VideoSwitchAgentDAI::VIDEO_OUTPUT_GROUP_TABLE_NAME;
            TA_THROW( TA_Base_Core::DataException( error.str().c_str(), TA_Base_Core::DataException::NOT_UNIQUE, 
                TA_VideoSwitchAgentDAI::VIDEO_OUTPUT_GROUP_KEY_COLUMN ) );
        }

        // If there are no entries for this VideoOutputGroup key then we need to insert them.

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
            "The VideoOutputGroup with the key %lu is a new VideoOutputGroup. It will be added to the database.",
            m_key );
        
        std::stringstream fullInsertSql;
        std::map< unsigned short, unsigned long >::iterator it = m_videoOutputs.begin();
        for( ; it != m_videoOutputs.end(); ++it )
        {
            fullInsertSql.str( "" );
            fullInsertSql << insertSql.str() << it->first << ", " << it->second << ")";
            databaseConnection->executeModification( fullInsertSql.str() );
        }

        m_isNew = false;

        LOG_FUNCTION_EXIT( SourceInfo, "VideoOutputGroupHelper::insertNewVideoOutputGroup" );
    }


    //
    // updateExistingVideoOutputGroup
    //
    void VideoOutputGroupHelper::updateExistingVideoOutputGroup()
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
            "The VideoOutputGroup with the key %lu already exists. It's data will be updated.",
            m_key );

       // If there are no VideoOutputs specified for this VideoOutputGroup, 
        // then we don't need to write anything.

        if ( m_videoOutputs.empty() )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                "There are no VideoOutputs assigned to VideoOutputGroup %lu. No data will be written to the database.",
                m_key );
            return;
        }

        // Get a connection to the database

        TA_Base_Core::IDatabase* databaseConnection = 
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Video_Cd, TA_Base_Core::Write);

        // Write the VideoOutputGroup to the database.
  
        // First check if there is already a VideoOutputGroup with this key in the database.
        // Use a std::auto_ptr for the TA_Base_Core::IData so it is automatically cleaned up.
        
        std::stringstream selectSql;
        selectSql << "select " << TA_VideoSwitchAgentDAI::POSITION_COLUMN;
		selectSql << ", " << TA_VideoSwitchAgentDAI::VIDEO_OUTPUT_KEY_COLUMN;
        selectSql << " from " << TA_VideoSwitchAgentDAI::VIDEO_OUTPUT_GROUP_TABLE_NAME;
        selectSql << " where " << TA_VideoSwitchAgentDAI::VIDEO_OUTPUT_GROUP_KEY_COLUMN;
        selectSql << "=" << m_key;

        std::vector< std::string > columnNames;
        columnNames.push_back( TA_VideoSwitchAgentDAI::POSITION_COLUMN );
        columnNames.push_back( TA_VideoSwitchAgentDAI::VIDEO_OUTPUT_GROUP_KEY_COLUMN );
        std::auto_ptr< TA_Base_Core::IData > data = std::auto_ptr< TA_Base_Core::IData >( 
            databaseConnection->executeQuery( selectSql.str(), columnNames ) );

        // If there is not already an entry for this VideoOutputGroup.

        unsigned short numRows = static_cast< unsigned short >( data->getNumRows() );
        if ( 0 == numRows )     // This is a new VideoOutputGroup.
        {
            std::stringstream error;
            error << "The VideoOutputGroup with database key = " << m_key;
            error << " does not already exist in the database table ";
            error << TA_VideoSwitchAgentDAI::VIDEO_OUTPUT_GROUP_TABLE_NAME;
            TA_THROW( TA_Base_Core::DataException( error.str().c_str(), TA_Base_Core::DataException::NO_VALUE, 
                TA_VideoSwitchAgentDAI::VIDEO_OUTPUT_GROUP_KEY_COLUMN ) );
        }
        else    // This is an existing VideoOutputGroup.
        {
		    // If we are updating an existing VideoOutputGroup we may have to delete items that are
		    // no longer in the group, update items that have changed and insert new items.  The
		    // delete, update and insert statements are defined below.  The position and the 
            // VideoOutput key will be added to these statements as required.

            // The positions to be deleted from the old VideoOutputGroup.  They will all be done at once.

		    std::stringstream deleteSql; 
		    deleteSql << "delete from " << TA_VideoSwitchAgentDAI::VIDEO_OUTPUT_GROUP_TABLE_NAME;
		    deleteSql << " where " << TA_VideoSwitchAgentDAI::VIDEO_OUTPUT_GROUP_KEY_COLUMN;
		    deleteSql << "=" << m_key << " and " << TA_VideoSwitchAgentDAI::POSITION_COLUMN;
		    deleteSql << "in (";        // positions - To be added as part of loop.

            // The update statements shall be of the form:   
            //      updateSqlPart1 << videoOutputKey << updateSqlPart2 << position

		    std::stringstream updateSqlPart1;
		    updateSqlPart1 << "update " << TA_VideoSwitchAgentDAI::VIDEO_OUTPUT_GROUP_TABLE_NAME;
            updateSqlPart1 << " set " << TA_VideoSwitchAgentDAI::VIDEO_OUTPUT_KEY_COLUMN;
            updateSqlPart1 << " = ";    // videoOutputKey - To be added as part of loop.
		    
            std::stringstream updateSqlPart2;
		    updateSqlPart2 << " where " << TA_VideoSwitchAgentDAI::VIDEO_OUTPUT_GROUP_KEY_COLUMN;
		    updateSqlPart2 << "=" << m_key << " and " << TA_VideoSwitchAgentDAI::POSITION_COLUMN;
		    updateSqlPart2 << "=";      // position - To be added as part of loop.

		    std::stringstream fullSql;
            unsigned short position = 0;
            unsigned long videoOutputKey = 0;
            unsigned short deleteCount = 0;     // The number of positions to be deleted from the old VideoOutputGroup.
            
            // Use a copy of the m_videoOutputs map, so we can delete items as we go along,
            // then we'll know what is left to insert at the end.
            
            std::map< unsigned short, unsigned long > copyOfVideoOutputs = m_videoOutputs;
            std::map< unsigned short, unsigned long >::iterator it;
            for( unsigned long i = 0; i < numRows; i++ )
            {
                fullSql.str( "" );
                position = data->getIntegerData( i, TA_VideoSwitchAgentDAI::POSITION_COLUMN );
                videoOutputKey = data->getUnsignedLongData( i, TA_VideoSwitchAgentDAI::VIDEO_OUTPUT_KEY_COLUMN );

			    // Will this position be used in the updated VideoOutputGroup?

                it = m_videoOutputs.find( position );
                if ( m_videoOutputs.end() == it )
                {
                    // Yes. Is the VideoOutput key the same?

                    if ( videoOutputKey == it->second )
                    {
                        // Yes. Do nothing.

                        continue;
                    }
                    else
                    {
                        // No. Perform an update.

                        fullSql << updateSqlPart1.str() << videoOutputKey << updateSqlPart2.str() << position;
                        databaseConnection->executeModification( fullSql.str() );
                    }
                }
                else    // The position is not required for the updated VideoOutputGroup.
                {
                    // No. 
    
                    deleteCount++;
                    if ( 1 != deleteCount )
                    {
                        deleteSql << ", ";  // If it isn't the first position added, we need a ", " for the statement.
                    }
                    deleteSql << position;
                }                
                
                // Now that we've dealt with this one, delete it from the (copy of the) map.

                copyOfVideoOutputs.erase( position );
            }
            
            // Now insert all the positions that haven't been inserted yet.

		    std::stringstream insertSql;
            insertSql << "insert into ";
            insertSql << TA_VideoSwitchAgentDAI::VIDEO_OUTPUT_GROUP_TABLE_NAME << " (";
            insertSql << TA_VideoSwitchAgentDAI::VIDEO_OUTPUT_GROUP_KEY_COLUMN << ", ";
            insertSql << TA_VideoSwitchAgentDAI::POSITION_COLUMN << ", ";
            insertSql << TA_VideoSwitchAgentDAI::VIDEO_OUTPUT_KEY_COLUMN << ") values(";
            insertSql << m_key << ", ";

            for ( it = copyOfVideoOutputs.begin(); it != copyOfVideoOutputs.end(); ++it )
            {
                fullSql.str( "" );
                fullSql << insertSql.str() << it->first << ", " << it->second << ")";
                databaseConnection->executeModification( fullSql.str() );
            }

            // Now delete all the positions we don't need anymore.

            deleteSql << ")";
            databaseConnection->executeModification( deleteSql.str() );
        }
        m_isNew = false;
    }

} // closes TA_IRS_Core
