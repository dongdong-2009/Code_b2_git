/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/PRIV_3001/3001/transactive/core/data_access_interface/video_switch_agent/src/SequenceAccessFactory.cpp $
  * @author Katherine Thomson
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by: $Author: lim.cy $
  * 
  * SequenceAccessFactory is a singleton that is used to retrieve objects either from the
  * database or newly created. All objects returned will adhere to the ISequence interface.
  */

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif // _MSC_VER

#include <sstream>

#include "core/data_access_interface/video_switch_agent/src/SequenceAccessFactory.h"
#include "core/data_access_interface/video_switch_agent/src/Sequence.h"
#include "core/data_access_interface/video_switch_agent/src/ConfigSequence.h"
#include "core/data_access_interface/video_switch_agent/src/SequenceTable.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_Core
{
	// Initialise statics
		
    TA_Base_Core::ReEntrantThreadLockable SequenceAccessFactory::m_singletonLock;
	SequenceAccessFactory* SequenceAccessFactory::m_theClassInstance = NULL;


    //
    // getInstance
    //
	SequenceAccessFactory& SequenceAccessFactory::getInstance()
	{
	    TA_Base_Core::ThreadGuard guard( m_singletonLock );

		// If guard acquired then make sure the singleton is still NULL.

		if ( m_theClassInstance == NULL )
		{
			// Create the one & only object
			
            m_theClassInstance = new SequenceAccessFactory();
		}
		
		return *m_theClassInstance;
	}

    
    //
    // getSequence
    //
    ISequence* SequenceAccessFactory::getSequence( 
        unsigned long sequenceKey, bool isReadWrite )
    {
        // Get a connection to the database
        
        TA_Base_Core::IDatabase* databaseConnection = 
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Video_Cd, TA_Base_Core::Read);

        // Check that this Sequence exists in the table.

      /*  std::stringstream selectSql;
        selectSql << "select " << TA_VideoSwitchAgentDAI::SEQUENCE_KEY_COLUMN;
        selectSql << " from " << TA_VideoSwitchAgentDAI::SEQUENCE_TABLE_NAME;
        selectSql << " where " << TA_VideoSwitchAgentDAI::SEQUENCE_KEY_COLUMN;
        selectSql << "=" << sequenceKey;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), VI_SEQUENCE_STD_SELECT_61501, sequenceKey);
//		std::string strSql  = databaseConnection->prepareSQLStatement(VI_SEQUENCE_SELECT_61501, sequenceKey);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, VI_SEQUENCE_SELECT_61501, sequenceKey);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned TA_Base_Core::IData object when we're done with it,
        // so make it a std::auto_ptr so it is automatically cleaned up.

        std::vector< std::string > columnNames;
        columnNames.push_back( TA_VideoSwitchAgentDAI::SEQUENCE_KEY_COLUMN );
        std::auto_ptr< TA_Base_Core::IData > data = std::auto_ptr< TA_Base_Core::IData >( 
            databaseConnection->executeQuery( strSql, columnNames ) );
        
        if ( 0 == data->getNumRows() ) // No entry found with the specified pkey
        {
            std::stringstream error;
            error << "No data found for Sequence with the key " << sequenceKey;			
            TA_THROW( TA_Base_Core::DataException( error.str().c_str(), TA_Base_Core::DataException::NO_VALUE, 
                TA_VideoSwitchAgentDAI::SEQUENCE_KEY_COLUMN ));
        }

        // Create the Sequence object to represent this object. Initially only populate it
        // with Sequence key. We will populate the rest of the data on demand.

        ISequence* theSequence;
        
        // Decide which type of Sequence object to return.

        if ( isReadWrite ) // Need a config object.
        {
            theSequence = new ConfigSequence( sequenceKey );
        }
        else // Need a standard object.
        {
            theSequence = new TA_VideoSwitchAgentDAI::Sequence( sequenceKey );
        }

        // Return the appropriate object. 
        // The class that recieves this pointer is responsible for deleting it.

        return theSequence;
    }


    //
    // getAllSequences
    //
    std::vector< ISequence* > SequenceAccessFactory::getAllSequences( bool isReadWrite )
    {
        // Get a connection to the database.

        TA_Base_Core::IDatabase* databaseConnection = 
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Video_Cd, TA_Base_Core::Read);

        // Get all the Sequence keys that exist in the table.

       /* std::stringstream selectSql;
        selectSql << "select " << TA_VideoSwitchAgentDAI::SEQUENCE_KEY_COLUMN;
        selectSql << " from " << TA_VideoSwitchAgentDAI::SEQUENCE_TABLE_NAME;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), VI_SEQUENCE_STD_SELECT_61502);
//		std::string strSql  = databaseConnection->prepareSQLStatement(VI_SEQUENCE_SELECT_61502);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, VI_SEQUENCE_SELECT_61502);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned TA_Base_Core::IData object when we're done with it,
        // so make it a std::auto_ptr so it is automatically cleaned up.

        std::vector< std::string > columnNames;
        columnNames.push_back( TA_VideoSwitchAgentDAI::SEQUENCE_KEY_COLUMN );
        std::auto_ptr< TA_Base_Core::IData > data = std::auto_ptr< TA_Base_Core::IData >(
            databaseConnection->executeQuery( strSql, columnNames ) );
        
        if ( 0 == data->getNumRows() ) // No entry found with the specified pkey
        {
            std::stringstream error;
            error << "There are no Sequences configured in the database table ";
            error << TA_VideoSwitchAgentDAI::SEQUENCE_TABLE_NAME;
            TA_THROW( TA_Base_Core::DataException( error.str().c_str(), TA_Base_Core::DataException::NO_VALUE, "" ));
        }

        // Create the vector of pointers to ISequences.

        std::vector< ISequence* > sequences;
        if ( isReadWrite )  // Need a config objects.
        {
            for ( unsigned long i = 0; i < data->getNumRows(); ++i )
            {                
                sequences.push_back( new ConfigSequence( 
                    data->getUnsignedLongData( i, TA_VideoSwitchAgentDAI::SEQUENCE_KEY_COLUMN ) ) );
            }
        }
        else // Need standard objects.
        {
            for ( unsigned long i = 0; i < data->getNumRows(); ++i )
            {                
                sequences.push_back( new TA_VideoSwitchAgentDAI::Sequence( 
                    data->getUnsignedLongData( i, TA_IRS_Core::TA_VideoSwitchAgentDAI::SEQUENCE_KEY_COLUMN ) ) );
            }
        }

        return sequences;
    }

    
    //
    // createSequence
    //
    IConfigSequence* SequenceAccessFactory::createSequence( unsigned long sequenceKey )
    {            
        return new ConfigSequence( sequenceKey );    
    }

} // closes TA_IRS_Core


