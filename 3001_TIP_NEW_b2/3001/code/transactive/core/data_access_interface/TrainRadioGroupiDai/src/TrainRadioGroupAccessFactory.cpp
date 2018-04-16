/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  Adam Radics
  * @version: $Revision$
  *
  * Last modification: $14:14 2007-11-5$
  * Last modified by:  $Lan Yuan$
  *
  */

#include "core/data_access_interface/TrainRadioGroupiDai/src/TrainRadioGroupAccessFactory.h"
#include "core/data_access_interface/TrainRadioGroupiDai/src/TrainRadioGroup.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_Core
{

    std::vector< ITrainRadioGroupPtr > TrainRadioGroupAccessFactory::loadAllTrainRadioGroups()
    {
        FUNCTION_ENTRY( "loadAllTrainRadioGroups" );

        // simply delegate the call to TrainRadioGroup

        FUNCTION_EXIT;
        return TrainRadioGroup::loadAllTrainRadioGroups();
    }

  
    ITrainRadioGroupPtr TrainRadioGroupAccessFactory::loadTrainRadioGroup( unsigned long pkey )
    {
        FUNCTION_ENTRY( "loadTrainRadioGroup" );

        //simply delegate the call to TrainRadioGroup
        FUNCTION_EXIT;
        return TrainRadioGroup::loadTrainRadioGroup(pkey);
    }

 
    ITrainRadioGroupPtr TrainRadioGroupAccessFactory::loadTrainRadioGroup( const std::string& groupTsi )
    {
        FUNCTION_ENTRY( "loadTrainRadioGroup" );

        //simply delegate the call to TrainRadioGroup
        FUNCTION_EXIT;
        return TrainRadioGroup::loadTrainRadioGroup(groupTsi);
    }


    ITrainRadioGroupPtr TrainRadioGroupAccessFactory::createNewTrainRadioGroup()
    {
        FUNCTION_ENTRY( "createNewTrainRadioGroup" );

        ITrainRadioGroupPtr newPointer( new TrainRadioGroup() );
        
        FUNCTION_EXIT;
        return newPointer;
    }


    ITrainRadioGroupPtr TrainRadioGroupAccessFactory::copyTrainRadioGroup( ITrainRadioGroupPtr trainRadioGroup )
    {
        FUNCTION_ENTRY( "copyTrainRadioGroup" );

        // need to cast down the pointer to the correct type
        ITrainRadioGroup* rawPointer = trainRadioGroup.get();

        TA_ASSERT( NULL != rawPointer, "Cant copy a NULL pointer" );

        TrainRadioGroup* specificType = dynamic_cast<TrainRadioGroup*>( rawPointer );

        // if the case failed, this is an exception
        if ( NULL == specificType )
        {
            TA_THROW( TA_Base_Core::DataException( "Incorrect type to copy",
                                                   TA_Base_Core::DataException::WRONG_TYPE,
                                                   "" ) );
        }

        ITrainRadioGroupPtr copyOfItem( new TrainRadioGroup( *specificType ) );

        FUNCTION_EXIT;
        return copyOfItem;
    }

}
