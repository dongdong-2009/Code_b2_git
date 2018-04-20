/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  adamr
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */


#include "app/trains/train_agent/src/IRadioGroupCoordinator.h"
#include "app/trains/train_agent/src/TrainRadioGroupCorbaServant.h"

#include "bus/trains/TrainAgentCorba/src/TrainAgentObjectNames.h"
#include "bus/trains/TrainCorbaProxy/src/ExceptionConversion.h"
#include "bus/trains/TrainCorbaProxy/src/RadioGroupTypesConversion.h"


namespace TA_IRS_App
{

    TrainRadioGroupCorbaServant::TrainRadioGroupCorbaServant( IRadioGroupCoordinator& radioGroupCoordinator )
        : m_radioGroupCoordinator( radioGroupCoordinator )
    {
        FUNCTION_ENTRY( "TrainRadioGroupCorbaServant" );

        // activate the servant, with the given object name
        activateServantWithName( TA_Base_Bus::TrainAgentObjectNames::TrainRadioGroupObjectName );

        FUNCTION_EXIT;
    }


    TrainRadioGroupCorbaServant::~TrainRadioGroupCorbaServant()
    {
        FUNCTION_ENTRY( "~TrainRadioGroupCorbaServant" );

        deactivateServant();

        FUNCTION_EXIT;
    }


    TA_Base_Bus::ITrainRadioGroupCorba::TrainRadioGroupList* TrainRadioGroupCorbaServant::getTrainRadioGroups()
    {
        FUNCTION_ENTRY( "getTrainRadioGroups" );

        // Delegate the call to m_radioGroupCoordinator:
        // call the cpp method, catching and wrapping any C++ exceptions in CORBA exceptions

        TA_IRS_Bus::RadioGroupTypes::TrainRadioGroupMap result;

        CPP_CATCH( result = m_radioGroupCoordinator.getRadioGroups() );

        FUNCTION_EXIT;
        return new TA_Base_Bus::ITrainRadioGroupCorba::TrainRadioGroupList( TA_IRS_Bus::RadioGroupTypesConversion::convertToCorba( result ) );
    }

} // TA_IRS_App
