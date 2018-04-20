/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  Robert van Hugten
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */


#include "bus/trains/TrainCorbaProxy/src/TrainOperationModeCorbaProxy.h"
#include "bus/trains/TrainCorbaProxy/src/ExceptionConversion.h"
#include "bus/trains/TrainAgentCorba/src/TrainAgentObjectNames.h"

#include "bus/trains/TrainAgentCorba/idl/src/ITrainOperationModeCorba.h"

#include "core/naming/src/NamedObject.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_Bus
{

    /**
     * The standard named object for access to the CORBA servant.
     * @author adamr
     * @version 1.0
     * @created 22-Oct-2007 4:46:57 PM
     */
    typedef TA_Base_Core::NamedObject < TA_Base_Bus::ITrainOperationModeCorba,
                                        TA_Base_Bus::ITrainOperationModeCorba_ptr,
                                        TA_Base_Bus::ITrainOperationModeCorba_var > ITrainOperationModeCorbaNamedObject;


    struct TrainOperationModeCorbaProxy::OperationModeCorbaObject
    {
        ITrainOperationModeCorbaNamedObject namedObject;
    };


    TrainOperationModeCorbaProxy::TrainOperationModeCorbaProxy( const std::string& agentName )
            : m_operationModeCorbaObject( new OperationModeCorbaObject() ) 
    {
        FUNCTION_ENTRY( "TrainPaCorbaProxy" );

        m_operationModeCorbaObject->namedObject.setCorbaName( agentName, TA_Base_Bus::TrainAgentObjectNames::TrainOperationModeObjectName );

        FUNCTION_EXIT;
    }


    TrainOperationModeCorbaProxy::~TrainOperationModeCorbaProxy()
    {
        FUNCTION_ENTRY( "~TrainOperationModeCorbaProxy" );

        delete m_operationModeCorbaObject;
        m_operationModeCorbaObject = NULL;

        FUNCTION_EXIT;
    }


    bool TrainOperationModeCorbaProxy::radioInFallback()
    {
        FUNCTION_ENTRY( "radioInFallback" );

        CORBA::Boolean fallBack = false;
        
        CORBA_CATCH( CORBA_CALL_RETURN( fallBack, m_operationModeCorbaObject->namedObject, radioInFallback, () ) );//limin

        FUNCTION_EXIT;
        return fallBack;
    }


    void TrainOperationModeCorbaProxy::inFallback( bool fallback )
    {
        FUNCTION_ENTRY( "inFallback" );

        CORBA_CATCH( CORBA_CALL( m_operationModeCorbaObject->namedObject, inFallback, ( fallback ) ) );//limin

        FUNCTION_EXIT;
    }


    void TrainOperationModeCorbaProxy::inDoubleAtsFailure( bool doubleAtsFailure )
    {
        FUNCTION_ENTRY( "inDoubleAtsFailure" );

        CORBA_CATCH( CORBA_CALL( m_operationModeCorbaObject->namedObject, inDoubleAtsFailure, ( doubleAtsFailure ) ) );//limin

        FUNCTION_EXIT;
    }


    void TrainOperationModeCorbaProxy::manuallyTakeTrainControl( CommonTypes::TrainIdType trainId,
                                                                 const std::string& sessionId )
    {
        FUNCTION_ENTRY( "manuallyTakeTrainControl" );

        CORBA_CATCH( CORBA_CALL( m_operationModeCorbaObject->namedObject, manuallyTakeTrainControl, ( trainId, sessionId.c_str() ) ) );

        FUNCTION_EXIT;
    }


}
