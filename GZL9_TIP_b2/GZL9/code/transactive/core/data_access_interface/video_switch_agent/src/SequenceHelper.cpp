/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/GZL6_TIP/3002/transactive/core/data_access_interface/video_switch_agent/src/SequenceHelper.cpp $
  * @author Katherine Thomson
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by: $Author: builder $
  * 
  * SequenceHelper is an object that is held by Sequence and ConfigSequence objects. 
  * It contains all data used by TypePluginMatrices, and manipulation
  * methods for the data. It helps avoid re-writing code for both Sequence and ConfigSequence.
  *
  * Note: The dwell time for Sequences can be retrieved/set using the 
  * Sequence object accessed through the EntityAccessFactory.
  */

#ifdef _MSC_VER
#pragma warning(disable:4786)
#pragma warning(disable:4503)  // warning C4503: decorated type length exceeded, type was truncated
#endif // _MSC_VER

#include <vector>
#include <memory>
#include <sstream>

#include "core/data_access_interface/video_switch_agent/src/SequenceHelper.h"
#include "core/data_access_interface/video_switch_agent/src/SequenceAccessFactory.h"
#include "core/data_access_interface/video_switch_agent/src/SequenceTable.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/exceptions/src/DataConfigurationException.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_Core
{
    //
    // SequenceHelper (For configuration)
    //
/*    SequenceHelper::SequenceHelper()
        : m_isValidData( false ),
          m_key( 0 )
    {
        m_videoInputs.clear();

        // Set up the vector of column names.

        std::string typeColumn = "TYPE";
        std::string pluginNameColumn = "PLUGIN_NAME";
        std::vector<std::string> columnNames;
        columnNames.push_back(typeColumn);
        columnNames.push_back(pluginNameColumn);
        columnNames.push_back(pluginNameColumn);
    }
*/

	SequenceHelper::SequenceHelper()
	{
	}

    //
    // SequenceHelper
    //
    SequenceHelper::SequenceHelper( unsigned long sequenceKey )
        : m_isValidData( false ),
          m_key( sequenceKey ),
          m_isNew( true )
    {
        m_videoInputs.clear();
    }


    //
    // SequenceHelper (Copy Constructor)
    // 
	SequenceHelper::SequenceHelper( const SequenceHelper& theSequenceHelper )
		: m_isValidData( theSequenceHelper.m_isValidData ),
          m_key( theSequenceHelper.m_key ),
          m_videoInputs( theSequenceHelper.m_videoInputs ),
          m_isNew( theSequenceHelper.m_isNew )
	{
        // Do nothing.
	}


    //
    // getKey
    //
    unsigned long SequenceHelper::getKey()
    {
        // This method cannot be called until the key has been set.

        TA_ASSERT( 0 != m_key, "The Sequence key has not been initialised." );
        return m_key;
    }


    //
    // getVideoInputs
    //
    std::map< unsigned short, unsigned long > SequenceHelper::getVideoInputs()
    {
        if ( !m_isValidData )
        {
            reload();
        }
        return m_videoInputs;
    }


    //
    // setVideoInputs
    //
    void SequenceHelper::setVideoInputs( const std::map< unsigned short, unsigned long >& videoInputs )
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "SequenceHelper::setType" );

        if ( !m_isValidData )
        {
            reload();
        }
        m_videoInputs = videoInputs;
        
        LOG_FUNCTION_EXIT( SourceInfo, "SequenceHelper::setType" );
    }


    //
    // invalidate
    //
    void SequenceHelper::invalidate()
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "SequenceHelper::invalidate" );

        // This method cannot be called until the SequenceKey and 
        // the VideoInput position have been set.

        TA_ASSERT( !m_isNew, "Attempted to call invalidate() on a new Sequence entry.");
        
        m_isValidData = false;

        LOG_FUNCTION_EXIT( SourceInfo, "SequenceHelper::invalidate" );
    }


    //
    // reload
    //
    void SequenceHelper::reload()
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "SequenceHelper::reload" );
        
        // Ensure the Sequence entry has already been written to the database. 
        // It doesn't make any sense to reload a Sequence entry that doesn't exist in the database.
        
        TA_ASSERT( 0 != m_key, 
            "This Sequence does not yet exist in the database. Therefore it cannot be reloaded.");
        
        m_videoInputs.clear();

        // Get a connection to the database.
        
        TA_Base_Core::IDatabase* databaseConnection = 
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Video_Cd, TA_Base_Core::Read);

        // Create the SQL string to retrieve the data for the Sequence
        // based upon the Sequence key.

      /*  std::stringstream selectSql;
        selectSql << "select " << TA_VideoSwitchAgentDAI::POSITION_COLUMN << ", ";
        selectSql << TA_VideoSwitchAgentDAI::VIDEO_INPUT_KEY_COLUMN;
        selectSql << " from " << TA_VideoSwitchAgentDAI::SEQUENCE_TABLE_NAME;
        selectSql << " where " << TA_VideoSwitchAgentDAI::SEQUENCE_KEY_COLUMN;
        selectSql << "=" << m_key;
        selectSql << " order by " << TA_VideoSwitchAgentDAI::POSITION_COLUMN;*/
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, VI_SEQUENCE_SELECT_61503, m_key);

        // Execute the query. 
        //
        // The executeQuery() method can throw a DatabaseException, as documented 
        // in the comment of this method.
        //
        // We are responsible for deleting the returned TA_Base_Core::IData object when we're done with it,
        // so make it an std::auto_ptr, then it will automatically be cleaned up.

        std::vector< std::string > columnNames;
        columnNames.push_back( TA_VideoSwitchAgentDAI::POSITION_COLUMN );
        columnNames.push_back( TA_VideoSwitchAgentDAI::VIDEO_INPUT_KEY_COLUMN );
        std::auto_ptr< TA_Base_Core::IData > data = std::auto_ptr< TA_Base_Core::IData >( 
            databaseConnection->executeQuery( strSql, columnNames ) );

        // Check that at least one entry was found for this key.  Note that for this
        // class it is generally expected that more than one entry will be found -
        // in fact we are expecting one entry per position in the Sequence, except
        // in the case where some positions are empty.
        
        int numRows = data->getNumRows();
        if ( 0 == numRows ) // No entry found with the specified key.
        {
            std::stringstream error;
            error << "The Sequence with the key " << m_key;
            error << " was not found in the database table ";
            error << TA_VideoSwitchAgentDAI::SEQUENCE_TABLE_NAME << ".";
            TA_THROW( TA_Base_Core::DataException( error.str().c_str(), TA_Base_Core::DataException::NO_VALUE, "" ));
        }

        // Assign the data as appropriate to the member variables.
        // These calls can throw TA_Base_Core::DataExceptions, as documented in the comment of this method.

        unsigned short position = 0;
        unsigned long videoInputKey = 0;
        for( int i = 0; i < numRows; i++ )
        {
            position = data->getIntegerData( i, TA_VideoSwitchAgentDAI::POSITION_COLUMN );
            videoInputKey = data->getUnsignedLongData( i, TA_VideoSwitchAgentDAI::VIDEO_INPUT_KEY_COLUMN );
            
            // Throw an exception if there are multiple entries for the same position.

            if ( m_videoInputs.end() != m_videoInputs.find( position ) )
            {
                std::stringstream error;
                error << "There are multiple VideoInputs assigned to position " << position;
                error << " for the Sequence with the key " << m_key;
                error << " in the database table ";
                error << TA_VideoSwitchAgentDAI::SEQUENCE_TABLE_NAME << ".";
                TA_THROW( TA_Base_Core::DataException( error.str().c_str(), TA_Base_Core::DataException::NOT_UNIQUE, 
                    TA_VideoSwitchAgentDAI::POSITION_COLUMN ));
            }

            // If there is only one entry for this position so far, add it to the map.
                                           
            m_videoInputs[ position ] = videoInputKey;
        }

        // The data is now valid.

        m_isValidData = true;

        LOG_FUNCTION_EXIT( SourceInfo, "SequenceHelper::reload" );
    }

                                                                 
    //
    // writeSequenceData
    //
    void SequenceHelper::writeSequenceData()
    {       
        LOG_FUNCTION_ENTRY( SourceInfo, "SequenceHelper::writeSequenceData" );     
        
        // If there are no VideoInputs specified for this Sequence, 
        // then we don't need to write anything.

        if ( m_videoInputs.empty() )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                "There are no VideoInputs assigned to Sequence %lu. No data will be written to the database.",
                m_key );
            return;
        }

        if ( m_isNew )
        {
            insertNewSequence();
        }
        else
        {
            updateExistingSequence();
        }

        // Now that the data has been written, what we hold is valid data.

        m_isValidData = true;
        
        LOG_FUNCTION_EXIT( SourceInfo, "SequenceHelper::writeSequenceData" );
    }


    //
    // deleteSequence
    //
    void SequenceHelper::deleteSequence()
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "SequenceHelper::deleteSequence" );
        TA_ASSERT( 0 != m_key, "This Sequence does not yet exist in the database, and therefore cannot be deleted." );
        
        // Get a connection to the database.

        TA_Base_Core::IDatabase* databaseConnection = 
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Video_Cd, TA_Base_Core::Write);
        
       /* std::stringstream deleteSql;
        deleteSql << "delete from " << TA_VideoSwitchAgentDAI::SEQUENCE_TABLE_NAME;
        deleteSql << " where " << TA_VideoSwitchAgentDAI::SEQUENCE_KEY_COLUMN;
        deleteSql << "=" << m_key;*/
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, VI_SEQUENCE_DELETE_61901, m_key);
        databaseConnection->executeModification( strSql );

        m_key = 0;
        m_videoInputs.clear();

        LOG_FUNCTION_EXIT( SourceInfo, "SequenceHelper::deleteSequence" );
    }


    //
    // insertNewSequence
    //
    void SequenceHelper::insertNewSequence()
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "SequenceHelper::insertNewSequence" );

        // If there are no VideoInputs specified for this Sequence, 
        // then we don't need to write anything.

        if ( m_videoInputs.empty() )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                "There are no VideoInputs assigned to Sequence %lu. No data will be written to the database.",
                m_key );
            return;
        }

        // Get a connection to the database

        TA_Base_Core::IDatabase* databaseConnection = 
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Video_Cd, TA_Base_Core::Write);

        // Write the Sequence to the database.
  
        // First check if there is already a Sequence with this key in the database.
        // Use a std::auto_ptr for the TA_Base_Core::IData so it is automatically cleaned up.
        
       /* std::stringstream selectSql;
        selectSql << "select " << TA_VideoSwitchAgentDAI::POSITION_COLUMN;
		selectSql << ", " << TA_VideoSwitchAgentDAI::VIDEO_INPUT_KEY_COLUMN;
        selectSql << " from " << TA_VideoSwitchAgentDAI::SEQUENCE_TABLE_NAME;
        selectSql << " where " << TA_VideoSwitchAgentDAI::SEQUENCE_KEY_COLUMN;
        selectSql << "=" << m_key;*/
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, VI_SEQUENCE_SELECT_61504, m_key);

        std::vector< std::string > columnNames;
        columnNames.push_back( TA_VideoSwitchAgentDAI::POSITION_COLUMN );
        columnNames.push_back( TA_VideoSwitchAgentDAI::SEQUENCE_KEY_COLUMN );
        std::auto_ptr< TA_Base_Core::IData > data = std::auto_ptr< TA_Base_Core::IData >( 
            databaseConnection->executeQuery( strSql, columnNames ) );

		// The position and VideoInput key will be added as required.

		/*std::stringstream insertSql;
        insertSql << "insert into ";
        insertSql << TA_VideoSwitchAgentDAI::SEQUENCE_TABLE_NAME << " (";
        insertSql << TA_VideoSwitchAgentDAI::SEQUENCE_KEY_COLUMN << ", ";
        insertSql << TA_VideoSwitchAgentDAI::POSITION_COLUMN << ", ";
        insertSql << TA_VideoSwitchAgentDAI::VIDEO_INPUT_KEY_COLUMN << ") values(";
        insertSql << m_key << ", ";*/

        // If there is already an entry for this Sequence, throw an exception.

        unsigned short numRows = static_cast< unsigned short >( data->getNumRows() );
        if ( 0 != numRows )     // This is an existing Sequence.
        {
            std::stringstream error;
            error << "A Sequence already exists with database key = " << m_key;
            error << " in the database table ";
            error << TA_VideoSwitchAgentDAI::SEQUENCE_TABLE_NAME;
            TA_THROW( TA_Base_Core::DataException( error.str().c_str(), TA_Base_Core::DataException::NOT_UNIQUE, 
                TA_VideoSwitchAgentDAI::SEQUENCE_KEY_COLUMN ) );
        }

        // If there are no entries for this Sequence key then we need to insert them.

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
            "The Sequence with the key %lu is a new Sequence. It will be added to the database.",
            m_key );
        
        //std::stringstream fullInsertSql;
        std::map< unsigned short, unsigned long >::iterator it = m_videoInputs.begin();
        for( ; it != m_videoInputs.end(); ++it )
        {
           /* fullInsertSql.str( "" );
            fullInsertSql << insertSql.str() << it->first << ", " << it->second << ")";*/
		databaseConnection->prepareSQLStatement(strSql, VI_SEQUENCE_INSERT_61801, m_key, it->first, it->second);
            databaseConnection->executeModification( strSql );
        }

        m_isNew = false;

        LOG_FUNCTION_EXIT( SourceInfo, "SequenceHelper::insertNewSequence" );
    }


    //
    // updateExistingSequence
    //
    void SequenceHelper::updateExistingSequence()
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
            "The Sequence with the key %lu already exists. It's data will be updated.",
            m_key );

       // If there are no VideoInputs specified for this Sequence, 
        // then we don't need to write anything.

        if ( m_videoInputs.empty() )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                "There are no VideoInputs assigned to Sequence %lu. No data will be written to the database.",
                m_key );
            return;
        }

        // Get a connection to the database

        TA_Base_Core::IDatabase* databaseConnection = 
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Video_Cd, TA_Base_Core::Write);

        // Write the Sequence to the database.
  
        // First check if there is already a Sequence with this key in the database.
        // Use a std::auto_ptr for the TA_Base_Core::IData so it is automatically cleaned up.
        
       /* std::stringstream selectSql;
        selectSql << "select " << TA_VideoSwitchAgentDAI::POSITION_COLUMN;
		selectSql << ", " << TA_VideoSwitchAgentDAI::VIDEO_INPUT_KEY_COLUMN;
        selectSql << " from " << TA_VideoSwitchAgentDAI::SEQUENCE_TABLE_NAME;
        selectSql << " where " << TA_VideoSwitchAgentDAI::SEQUENCE_KEY_COLUMN;
        selectSql << "=" << m_key;*/
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, VI_SEQUENCE_SELECT_61504, m_key);

        std::vector< std::string > columnNames;
        columnNames.push_back( TA_VideoSwitchAgentDAI::POSITION_COLUMN );
        columnNames.push_back( TA_VideoSwitchAgentDAI::SEQUENCE_KEY_COLUMN );
        std::auto_ptr< TA_Base_Core::IData > data = std::auto_ptr< TA_Base_Core::IData >( 
            databaseConnection->executeQuery( strSql, columnNames ) );

        // If there is not already an entry for this Sequence.

        unsigned short numRows = static_cast< unsigned short >( data->getNumRows() );
        if ( 0 == numRows )     // This is a new Sequence.
        {
            std::stringstream error;
            error << "The Sequence with database key = " << m_key;
            error << " does not already exist in the database table ";
            error << TA_VideoSwitchAgentDAI::SEQUENCE_TABLE_NAME;
            TA_THROW( TA_Base_Core::DataException( error.str().c_str(), TA_Base_Core::DataException::NO_VALUE, 
                TA_VideoSwitchAgentDAI::SEQUENCE_KEY_COLUMN ) );
        }
        else    // This is an existing Sequence.
        {
		    // If we are updating an existing Sequence we may have to delete items that are
		    // no longer in the group, update items that have changed and insert new items.  The
		    // delete, update and insert statements are defined below.  The position and the 
            // VideoInput key will be added to these statements as required.

            // The positions to be deleted from the old Sequence.  They will all be done at once.

		    std::stringstream deleteSql; 
		   /* deleteSql << "delete from " << TA_VideoSwitchAgentDAI::SEQUENCE_TABLE_NAME;
		    deleteSql << " where " << TA_VideoSwitchAgentDAI::SEQUENCE_KEY_COLUMN;
		    deleteSql << "=" << m_key << " and " << TA_VideoSwitchAgentDAI::POSITION_COLUMN;
		    deleteSql << "in (";     */   // positions - To be added as part of loop.

            // The update statements shall be of the form:   
            //      updateSqlPart1 << videoInputKey << updateSqlPart2 << position

		   /* std::stringstream updateSqlPart1;
		    updateSqlPart1 << "update " << TA_VideoSwitchAgentDAI::SEQUENCE_TABLE_NAME;
            updateSqlPart1 << " set " << TA_VideoSwitchAgentDAI::VIDEO_INPUT_KEY_COLUMN;
            updateSqlPart1 << " = "; */   // videoInputKey - To be added as part of loop.
		    
           /* std::stringstream updateSqlPart2;
		    updateSqlPart2 << " where " << TA_VideoSwitchAgentDAI::SEQUENCE_KEY_COLUMN;
		    updateSqlPart2 << "=" << m_key << " and " << TA_VideoSwitchAgentDAI::POSITION_COLUMN;
		    updateSqlPart2 << "=";  */    // position - To be added as part of loop.

		    //std::stringstream fullSql;
            unsigned short position = 0;
            unsigned long videoInputKey = 0;
            unsigned short deleteCount = 0;     // The number of positions to be deleted from the old Sequence.
            
            // Use a copy of the m_videoInputs map, so we can delete items as we go along,
            // then we'll know what is left to insert at the end.
            
            std::map< unsigned short, unsigned long > copyOfVideoInputs = m_videoInputs;
            std::map< unsigned short, unsigned long >::iterator it;
            for( unsigned long i = 0; i < numRows; i++ )
            {
                //fullSql.str( "" );
                position = data->getIntegerData( i, TA_VideoSwitchAgentDAI::POSITION_COLUMN );
                videoInputKey = data->getUnsignedLongData( i, TA_VideoSwitchAgentDAI::VIDEO_INPUT_KEY_COLUMN );

			    // Will this position be used in the updated Sequence?

                it = m_videoInputs.find( position );
                if ( m_videoInputs.end() == it )
                {
                    // Yes. Is the VideoInput key the same?

                    if ( videoInputKey == it->second )
                    {
                        // Yes. Do nothing.

                        continue;
                    }
                    else
                    {
                        // No. Perform an update.

                        //fullSql << updateSqlPart1.str() << videoInputKey << updateSqlPart2.str() << position;
						TA_Base_Core::SQLStatement strSql2;
						databaseConnection->prepareSQLStatement(strSql2, VI_SEQUENCE_UPDATE_61651, videoInputKey, m_key, position);
                        databaseConnection->executeModification( strSql2 );
                    }
                }
                else    // The position is not required for the updated Sequence.
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

                copyOfVideoInputs.erase( position );
            }
            
            // Now insert all the positions that haven't been inserted yet.

		   /* std::stringstream insertSql;
            insertSql << "insert into ";
            insertSql << TA_VideoSwitchAgentDAI::SEQUENCE_TABLE_NAME << " (";
            insertSql << TA_VideoSwitchAgentDAI::SEQUENCE_KEY_COLUMN << ", ";
            insertSql << TA_VideoSwitchAgentDAI::POSITION_COLUMN << ", ";
            insertSql << TA_VideoSwitchAgentDAI::VIDEO_INPUT_KEY_COLUMN << ") values(";
            insertSql << m_key << ", ";*/

            for ( it = copyOfVideoInputs.begin(); it != copyOfVideoInputs.end(); ++it )
            {
               /* fullSql.str( "" );
                fullSql << insertSql.str() << it->first << ", " << it->second << ")";*/
				TA_Base_Core::SQLStatement strSql3;
				databaseConnection->prepareSQLStatement(strSql3, VI_SEQUENCE_INSERT_61801, m_key, it->first, it->second);
                databaseConnection->executeModification( strSql3 );
            }

            // Now delete all the positions we don't need anymore.

            //deleteSql << ")";
			TA_Base_Core::SQLStatement strSql4;
			databaseConnection->prepareSQLStatement(strSql4, VI_SEQUENCE_DELETE_61902, m_key, deleteSql.str());
			databaseConnection->executeModification(strSql4);
        }
        m_isNew = false;
    }

    void SequenceHelper::getSequenceRelationShipInSequences(const std::vector<unsigned long>& sequenceKeys, sequence_to_input_map& sequences)
    {
        if(sequenceKeys.empty())
        {
            return;
        }
        std::stringstream ss;
        ss << "(";
        for(std::vector<unsigned long>::const_iterator it = sequenceKeys.begin(); it != sequenceKeys.end(); ++it)
        {
            ss << (*it);
            ss << ",";
        }
        std::string inString = ss.str();
        inString.replace(inString.find_last_of(','), 1, 1, ')');
        /*std::stringstream selectSql;
        selectSql << "select " << TA_VideoSwitchAgentDAI::PARENT_PKEY_COLUMN;
        selectSql << "," << TA_VideoSwitchAgentDAI::PKEY_COLUMN;
		selectSql << "," << TA_VideoSwitchAgentDAI::DWELLTIME_COLUMN;
		selectSql << "," << TA_VideoSwitchAgentDAI::DESCRIPTION_COLUMN;
        selectSql << " from " << TA_VideoSwitchAgentDAI::RELATION_TABLE_NAME;
        selectSql << " where " << TA_VideoSwitchAgentDAI::PARENT_PKEY_COLUMN;
        selectSql << " in " << inString;
		selectSql << " order by " << TA_VideoSwitchAgentDAI::PARENT_PKEY_COLUMN;
        selectSql << "," << TA_VideoSwitchAgentDAI::ORDERINSEQ_COLUMN;*/
        
		std::vector< std::string > columnNames;
        columnNames.push_back( TA_VideoSwitchAgentDAI::PARENT_PKEY_COLUMN );
        columnNames.push_back( TA_VideoSwitchAgentDAI::PKEY_COLUMN );
		columnNames.push_back( TA_VideoSwitchAgentDAI::DWELLTIME_COLUMN );
		columnNames.push_back( TA_VideoSwitchAgentDAI::DESCRIPTION_COLUMN );
        TA_Base_Core::IDatabase* databaseConnection = 
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Video_Cd, TA_Base_Core::Read);
		TA_Base_Core::SQLStatement selectSql;
		databaseConnection->prepareSQLStatement(selectSql, VI_ENTITY_RELATIONSHIP_SELECT_108001, inString.c_str());
        TA_Base_Core::IData* data = databaseConnection->executeQuery( selectSql, columnNames );
        unsigned long parentkey, inputKey;
        unsigned short dwellTime;
        std::string description;
        do 
        {
            {
                std::auto_ptr<TA_Base_Core::IData> dataWrap(data);
                for(unsigned long i = 0, size = dataWrap->getNumRows(); i < size; ++i)
                {
                    parentkey = dataWrap->getUnsignedLongData(i, TA_VideoSwitchAgentDAI::PARENT_PKEY_COLUMN);
                    inputKey = dataWrap->getUnsignedLongData(i, TA_VideoSwitchAgentDAI::PKEY_COLUMN);
                    dwellTime = dataWrap->getUnsignedLongData(i, TA_VideoSwitchAgentDAI::DWELLTIME_COLUMN);
                    description = dataWrap->getStringData(i, TA_VideoSwitchAgentDAI::DESCRIPTION_COLUMN);
                    sequence_input& input = sequences[parentkey];
                    input.m_sequenceKey = parentkey;
                    input.m_inputKeys.push_back(inputKey);
                    input.m_dwellTime = dwellTime;
                    input.m_descriptions = description;
                }
            }
            data = NULL;
        } while (databaseConnection->moreData(data));
    }

	void SequenceHelper::getSequenceRelationShip(unsigned long sequenceKey,unsigned short& dwellTime, std::string& description,std::vector<unsigned long>& videoInputKeys)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
			"Get the input videos int the Sequence with the key %lu .",
            sequenceKey );
		        // Get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Video_Cd, TA_Base_Core::Read);

		// Read the Sequence from the database.
  
        // First check if there is already a Sequence with this key in the database.
        // Use a std::auto_ptr for the TA_Base_Core::IData so it is automatically cleaned up.
        
        /*std::stringstream selectSql;
        selectSql << "select " << TA_VideoSwitchAgentDAI::PKEY_COLUMN;
		selectSql << "," << TA_VideoSwitchAgentDAI::DWELLTIME_COLUMN;
		selectSql << "," << TA_VideoSwitchAgentDAI::DESCRIPTION_COLUMN;
        selectSql << " from " << TA_VideoSwitchAgentDAI::RELATION_TABLE_NAME;
        selectSql << " where " << TA_VideoSwitchAgentDAI::PARENT_PKEY_COLUMN;
        selectSql << "=" << sequenceKey;
		selectSql << "order by " << TA_VideoSwitchAgentDAI::ORDERINSEQ_COLUMN;*/
		TA_Base_Core::SQLStatement selectSql;
		databaseConnection->prepareSQLStatement(selectSql, VI_ENTITY_RELATIONSHIP_SELECT_108002, sequenceKey);
		std::vector< std::string > columnNames;
        columnNames.push_back( TA_VideoSwitchAgentDAI::PKEY_COLUMN );
		columnNames.push_back( TA_VideoSwitchAgentDAI::DWELLTIME_COLUMN );
		columnNames.push_back( TA_VideoSwitchAgentDAI::DESCRIPTION_COLUMN );
	 //	columnNames.push_back( TA_VideoSwitchAgentDAI::PARENT_PKEY_COLUMN );
        std::auto_ptr< TA_Base_Core::IData > data = std::auto_ptr< TA_Base_Core::IData >( 
            databaseConnection->executeQuery( selectSql, columnNames ) );
		unsigned short numRows = static_cast< unsigned short >( data->getNumRows() );
        if ( 0 == numRows )     // This is a new Sequence.
        {
            std::stringstream error;
            error << "The Sequence with key = " << sequenceKey;
            error << " does not exist in the database table ";
            error << TA_VideoSwitchAgentDAI::RELATION_TABLE_NAME;
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, error.str().c_str());
        //    TA_THROW( TA_Base_Core::DataException( error.str().c_str(), TA_Base_Core::DataException::NO_VALUE, 
        //        TA_VideoSwitchAgentDAI::PARENT_PKEY_COLUMN ) );
        }
        else    // This is an existing Sequence.
        {
		    // If we are updating an existing Sequence we may have to delete items that are
		    // no longer in the group, update items that have changed and insert new items.  The
		    // delete, update and insert statements are defined below.  The position and the 
            // VideoInput key will be added to these statements as required.

            // The positions to be deleted from the old Sequence.  They will all be done at once.
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugTrace,"Found camera under sequence %ld",sequenceKey );
			videoInputKeys.erase(videoInputKeys.begin(),videoInputKeys.end());
			videoInputKeys.resize(numRows);
			unsigned long videoInputKey;
			int nOrderInSeq;			
			int i = 0;
			dwellTime = data->getIntegerData(0,TA_VideoSwitchAgentDAI::DWELLTIME_COLUMN );
			description=data->getStringData(0,TA_VideoSwitchAgentDAI::DESCRIPTION_COLUMN);
			for( ; i < numRows; i++ )
			{
				videoInputKey = data->getUnsignedLongData( i, TA_VideoSwitchAgentDAI::PKEY_COLUMN );
			//	nOrderInSeq	  =	data->getIntegerData( i, TA_VideoSwitchAgentDAI::ORDERINSEQ_COLUMN );
			//	if(nOrderInSeq != 0)
			//	{						
			//		videoInputKeys[nOrderInSeq-1]=videoInputKey;
			//	}
			//	else
				{
					videoInputKeys[i] = videoInputKey;
				}
			}
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugTrace,"Total %d camera are gotten",i );
		}
	}

	void SequenceHelper::setSequenceRelationShip(unsigned long sequenceKey,unsigned short dwellTime,const std::string& description, std::vector<unsigned long> videoInputKeys)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
			"The Sequence with the key %lu will be updated.",
            sequenceKey );

        // Get a connection to the database

        TA_Base_Core::IDatabase* databaseConnection = 
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Video_Cd, TA_Base_Core::Write);

        // Write the Sequence to the database.
  
        // First check if there is already a Sequence with this key in the database.
        // Use a std::auto_ptr for the TA_Base_Core::IData so it is automatically cleaned up.
        
        /*std::stringstream selectSql;
        selectSql << "select " << TA_VideoSwitchAgentDAI::PKEY_COLUMN;
        selectSql << " from " << TA_VideoSwitchAgentDAI::RELATION_TABLE_NAME;
        selectSql << " where " << TA_VideoSwitchAgentDAI::PARENT_PKEY_COLUMN;
        selectSql << "=" << sequenceKey;*/

		TA_Base_Core::SQLStatement selectSql;
		databaseConnection->prepareSQLStatement(selectSql, VI_ENTITY_RELATIONSHIP_SELECT_108003, sequenceKey);
        std::vector< std::string > columnNames;
        columnNames.push_back( TA_VideoSwitchAgentDAI::PKEY_COLUMN );
        std::auto_ptr< TA_Base_Core::IData > data = std::auto_ptr< TA_Base_Core::IData >( 
            databaseConnection->executeQuery( selectSql, columnNames ) );

        // If there is not already an entry for this Sequence.

        unsigned short numRows = static_cast< unsigned short >( data->getNumRows() );
        if ( 0 == numRows )     // This is a new Sequence.
        {
            std::stringstream error;
            error << "The Sequence with key = " << sequenceKey;
            error << " does not exist in the database table ";
            error << TA_VideoSwitchAgentDAI::RELATION_TABLE_NAME;
            //TA_THROW( TA_Base_Core::DataException( error.str().c_str(), TA_Base_Core::DataException::NO_VALUE, 
            //    TA_VideoSwitchAgentDAI::PARENT_PKEY_COLUMN ) );
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, error.str().c_str());
        }
        else    // This is an existing Sequence.
        {
		    // If we are updating an existing Sequence we may have to delete items that are
		    // no longer in the group, update items that have changed and insert new items.  The
		    // delete, update and insert statements are defined below.  The position and the 
            // VideoInput key will be added to these statements as required.

            // The positions to be deleted from the old Sequence.  They will all be done at once.

		    /*std::stringstream deleteSql; 
		    deleteSql << "delete from " << TA_VideoSwitchAgentDAI::RELATION_TABLE_NAME;
		    deleteSql << " where " << TA_VideoSwitchAgentDAI::PARENT_PKEY_COLUMN;
		    deleteSql << "=" << sequenceKey ;*/
			TA_Base_Core::SQLStatement deleteSql;
			databaseConnection->prepareSQLStatement(deleteSql, VI_ENTITY_RELATIONSHIP_DELETE_108004, sequenceKey);
			databaseConnection->executeModification( deleteSql );
            // Now insert all the positions that haven't been inserted yet.
		}
		TA_Base_Core::SQLStatement fullSql;
	    /*std::stringstream insertSql;
        insertSql << "insert into ";
        insertSql << TA_VideoSwitchAgentDAI::RELATION_TABLE_NAME << " (";
        insertSql << TA_VideoSwitchAgentDAI::PARENT_PKEY_COLUMN << ", ";        
        
        insertSql << TA_VideoSwitchAgentDAI::PKEY_COLUMN << ",";
		insertSql << TA_VideoSwitchAgentDAI::DWELLTIME_COLUMN <<",";
		insertSql << TA_VideoSwitchAgentDAI::DESCRIPTION_COLUMN <<",";
		insertSql << TA_VideoSwitchAgentDAI::ORDERINSEQ_COLUMN <<") values(";
        insertSql << sequenceKey << ", ";
		std::stringstream fullSql;*/
		for(int i=0;i<videoInputKeys.size();i++ )
        {
            /*fullSql.str( "" );
            fullSql << insertSql.str() << videoInputKeys[i] << ","<<dwellTime<<",'"<<description<<"',"<<i+1<<")";*/

			databaseConnection->prepareSQLStatement(fullSql, VI_ENTITY_RELATIONSHIP_INSERT_108005, sequenceKey, videoInputKeys[i], dwellTime,
				description, i+1);

            databaseConnection->executeModification( fullSql );
        }
        
	}

    void SequenceHelper::getRelationShipFromParentKey(unsigned long parentKey, unsigned long& pkey, std::string& description, int* ptype)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
			"Get the input videos int the Sequence with the key %lu .",
			parentKey );
		// Get a connection to the database
		TA_Base_Core::IDatabase* databaseConnection = 
			TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Video_Cd, TA_Base_Core::Read);

		// Read the Sequence from the database.

		// First check if there is already a Sequence with this key in the database.
		// Use a std::auto_ptr for the TA_Base_Core::IData so it is automatically cleaned up.

		/*std::stringstream selectSql;*/
		pkey = 0l;
		/*selectSql << "select " << TA_VideoSwitchAgentDAI::PKEY_COLUMN;
		selectSql << "," << TA_VideoSwitchAgentDAI::ENTITY_TYPE_COLUMN;
		selectSql << " from " << TA_VideoSwitchAgentDAI::RELATION_TABLE_NAME;
		selectSql << " where " << TA_VideoSwitchAgentDAI::PARENT_PKEY_COLUMN;
		selectSql << "=" << parentKey;*/
		TA_Base_Core::SQLStatement selectSql;
		databaseConnection->prepareSQLStatement(selectSql, VI_ENTITY_RELATIONSHIP_SELECT_108006, parentKey);
		
		std::vector< std::string > columnNames;
		columnNames.push_back( TA_VideoSwitchAgentDAI::PKEY_COLUMN );
		columnNames.push_back( TA_VideoSwitchAgentDAI::DESCRIPTION_COLUMN);
		columnNames.push_back( TA_VideoSwitchAgentDAI::ENTITY_TYPE_COLUMN);
		std::auto_ptr< TA_Base_Core::IData > data = std::auto_ptr< TA_Base_Core::IData >( 
			databaseConnection->executeQuery( selectSql, columnNames ) );
		unsigned short numRows = static_cast< unsigned short >( data->getNumRows() );
		if ( 0 == numRows )     // This is a new Sequence.
		{
			std::stringstream error;
			error << "The entity relationship with parentkey = " << parentKey;
			error << " does not exist in the database table ";
			error << TA_VideoSwitchAgentDAI::RELATION_TABLE_NAME;
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, error.str().c_str());
		}
		else    // This is an existing Sequence.
		{
			// If we are updating an existing Sequence we may have to delete items that are
			// no longer in the group, update items that have changed and insert new items.  The
			// delete, update and insert statements are defined below.  The position and the 
			// VideoInput key will be added to these statements as required.

			// The positions to be deleted from the old Sequence.  They will all be done at once.
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugTrace,"Found entity relationship with parentkey %ld",parentKey );
			
			unsigned long videoInputKey;
			int nOrderInSeq;			
			int i = 0;
			pkey = data->getUnsignedLongData( 0, TA_VideoSwitchAgentDAI::PKEY_COLUMN );
            description = data->getStringData(0, TA_VideoSwitchAgentDAI::DESCRIPTION_COLUMN);
			if(ptype!=NULL)
				*ptype = data->getIntegerData(0,TA_VideoSwitchAgentDAI::ENTITY_TYPE_COLUMN );
			
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugTrace,"entity relationship with pkey %ld, description %s and typekey %d are gotten",pkey, description.c_str(), ptype==NULL?0:*ptype );
		}
	}

    void SequenceHelper::setRelationShipOfParentKey(unsigned long parentKey, unsigned long pkey, const std::string& description, int typekey)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
			"The entity relationship with the parentkey %lu will be updated.",
			parentKey );

		// Get a connection to the database

		TA_Base_Core::IDatabase* databaseConnection = 
			TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Video_Cd, TA_Base_Core::Write);

		// Write the Sequence to the database.

		// First check if there is already a Sequence with this key in the database.
		// Use a std::auto_ptr for the TA_Base_Core::IData so it is automatically cleaned up.

		/*std::stringstream selectSql;
		selectSql << "select " << TA_VideoSwitchAgentDAI::PKEY_COLUMN;
		selectSql << " from " << TA_VideoSwitchAgentDAI::RELATION_TABLE_NAME;
		selectSql << " where " << TA_VideoSwitchAgentDAI::PARENT_PKEY_COLUMN;
		selectSql << "=" << parentKey;*/

		TA_Base_Core::SQLStatement selectSql;
		databaseConnection->prepareSQLStatement(selectSql, VI_ENTITY_RELATIONSHIP_SELECT_108007, parentKey);
		std::vector< std::string > columnNames;
		columnNames.push_back( TA_VideoSwitchAgentDAI::PKEY_COLUMN );
		std::auto_ptr< TA_Base_Core::IData > data = std::auto_ptr< TA_Base_Core::IData >( 
			databaseConnection->executeQuery( selectSql, columnNames ) );

		// If there is not already an entry for this Sequence.

		unsigned short numRows = static_cast< unsigned short >( data->getNumRows() );

		if ( 0 < numRows )    //already exist a entity relationship,remove it
		{
			delRelationShipOfParentKey(parentKey);
		}	
		std::stringstream error;
		error << "The Sequence with key = " << parentKey;
		error << " does not exist in the database table ";
		error << TA_VideoSwitchAgentDAI::RELATION_TABLE_NAME;
		//TA_THROW( TA_Base_Core::DataException( error.str().c_str(), TA_Base_Core::DataException::NO_VALUE, 
		//    TA_VideoSwitchAgentDAI::PARENT_PKEY_COLUMN ) );
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, error.str().c_str());
		/*std::stringstream insertSql;
		insertSql << "insert into ";
		insertSql << TA_VideoSwitchAgentDAI::RELATION_TABLE_NAME << " (";
		insertSql << TA_VideoSwitchAgentDAI::PARENT_PKEY_COLUMN << ", ";        

		insertSql << TA_VideoSwitchAgentDAI::PKEY_COLUMN << ",";
		insertSql << TA_VideoSwitchAgentDAI::ENTITY_TYPE_COLUMN << ") values(";
		insertSql << parentKey << ", ";
		insertSql << pkey << ", ";
		insertSql << typekey << ") ";*/

		TA_Base_Core::SQLStatement insertSql;
		databaseConnection->prepareSQLStatement(insertSql, VI_ENTITY_RELATIONSHIP_INSERT_108008, parentKey, pkey, description, typekey);
		databaseConnection->executeModification( insertSql );

	}

	void SequenceHelper::delRelationShipOfParentKey(unsigned long parentKey)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
			"The entity relationship with the parentkey %lu will be deleted.",
			parentKey );

		// Get a connection to the database

		TA_Base_Core::IDatabase* databaseConnection = 
			TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Video_Cd, TA_Base_Core::Write);

		// Write the Sequence to the database.

		// First check if there is already a Sequence with this key in the database.
		// Use a std::auto_ptr for the TA_Base_Core::IData so it is automatically cleaned up.

		/*std::stringstream deleteSql;
		deleteSql << "delete from " << TA_VideoSwitchAgentDAI::RELATION_TABLE_NAME;
		deleteSql << " where " << TA_VideoSwitchAgentDAI::PARENT_PKEY_COLUMN;
		deleteSql << " = " << parentKey ;*/
		
		TA_Base_Core::SQLStatement deleteSql;
		databaseConnection->prepareSQLStatement(deleteSql, VI_ENTITY_RELATIONSHIP_DELETE_108009, parentKey);

		
		databaseConnection->executeModification( deleteSql );
	}
} // closes TA_IRS_Core
