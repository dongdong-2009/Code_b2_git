/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/CODE_TA_BASE_DDB/transactive/core/data_access_interface/entity_access/src/TrainBorneManagerEntityData.cpp $
  * @author zhang hongzhi 
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/09/29 15:27:00 $
  * Last modified by: $Author: trainteam $
  * 
  * TrainBorneManagerEntityData provides access to TrainBorneManagerEntityData entity data
  * and a specific framework for TrainBorneManagerEntityData entity parameters.
  */

#pragma warning( disable: 4786 )

#include "core/data_access_interface/entity_access/src/TrainBorneManagerEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityHelper.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DbConnection.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"

namespace
{
    // 
    // The entity parameters for this entity type.
    //
  
    const std::string POLL_PERIOD_IN_M_SECS        = "PollPeriodInMSecs";
}


namespace TA_Base_Core
{
    //
    // The entity type is fixed for all instances of this class.
    // If this is changed, please update app/cctv/video_switch_agent/src/EntityTypeConstants.h.
    //

    const std::string TrainBorneManagerEntityData::ENTITY_TYPE = "TrainBorneManager";
   
    //
    // TrainBorneManagerEntityData
    //
    TrainBorneManagerEntityData::TrainBorneManagerEntityData()
        : EntityData()
    {
        FUNCTION_ENTRY( "TrainBorneManagerEntityData constructor" );
    }


    //
    // TrainBorneManagerEntityData
    //
    TrainBorneManagerEntityData::TrainBorneManagerEntityData( unsigned long key ) 
        : EntityData( key, ENTITY_TYPE )
    {
        FUNCTION_ENTRY( "TrainBorneManagerEntityData constructor" );
    }


    //
    // getAutoTrainBorneDirectory
    //
    int TrainBorneManagerEntityData::getBulkingPollPeriod()
    {
		int nPollPeriod = 1;
		std::string value = getHelper()->getParameter(POLL_PERIOD_IN_M_SECS);
		nPollPeriod = getHelper()->getIntegerData(value);
		return nPollPeriod;
    }


    ///////////////////////////////////////////////////////////////////////
    //
    // IEntityData Methods
    //
    ///////////////////////////////////////////////////////////////////////


    //
    // getType
    //
    std::string TrainBorneManagerEntityData::getType()
    {
        return(getStaticType());
    }


    //
    // getStaticType
    //
    std::string TrainBorneManagerEntityData::getStaticType()
    {
        return ENTITY_TYPE;
    }

   
    //
    // clone
    //
    IEntityData* TrainBorneManagerEntityData::clone( unsigned long key )
    {
        return new TrainBorneManagerEntityData( key );
    }
    
    
    //
    // invalidate
    //
    void TrainBorneManagerEntityData::invalidate()
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "invalidate" );
        TA_ASSERT( 0 != getHelper(), "The EntityHelper pointer is NULL" );
        getHelper()->invalidate();
        LOG_FUNCTION_EXIT( SourceInfo, "invalidate" );
    }

        
    ///////////////////////////////////////////////////////////////////////
    //
    // Protected Methods
    //
    ///////////////////////////////////////////////////////////////////////



    //
    // validateData
    //
    void TrainBorneManagerEntityData::validateData()
    {
    }


    //
    // setParameter (for string parameters)
    //
    void TrainBorneManagerEntityData::setParameter( const std::string& name, 
        const std::string& value)
    {
        // Set the value in the database.

        TA_ASSERT( 0 != getHelper(), "The EntityHelper pointer is NULL" );
        std::map< std::string, std::string > paramMap;
        paramMap[ name ] = value;
        getHelper()->writeParameters( paramMap );

        // Set the member variable.
    }

} // TA_Base_Core
