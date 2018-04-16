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

#include "app/trains/train_agent/src/RadioSessionData.h"

#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_App
{

    RadioSessionData::RadioSessionData( TA_Base_Core::RadioSessionEntityDataPtr entityData )
            : m_consoleKey( 0 ), // not all entities have a console - protect this with exception handling
              m_locationKey( entityData->getLocation() ), // all entities have a location (exceptions can leak - configuration error)
              m_corbaName( entityData->getKey(),
                           entityData->getAgentName(),
                           entityData->getName(),
                           entityData->getLocation() ), // all radio sessions must have an agent name and object name
              m_agentObject( NULL )
    {
        FUNCTION_ENTRY( "RadioSessionData" );

        // from the entity data, populate the console key, location key, and the corbaname
        // catch any exceptions, it is possible the radio session does not have an associated console.

        try
        {
            m_consoleKey = entityData->getAssociatedConsoleKey();
        }
        catch ( TA_Base_Core::TransactiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", e.what() );
        }

        FUNCTION_EXIT;
    }


    RadioSessionData::~RadioSessionData()
    {
        FUNCTION_ENTRY( "~RadioSessionData" );

        // delete m_agentObject and set it to NULL
        delete m_agentObject;
        m_agentObject = NULL;

        FUNCTION_EXIT;
    }


    unsigned long RadioSessionData::getConsoleKey()
    {
        FUNCTION_ENTRY( "getConsoleKey" );
        FUNCTION_EXIT;
        return m_consoleKey;
    }


    unsigned long RadioSessionData::getLocationKey()
    {
        FUNCTION_ENTRY( "getLocationKey" );
        FUNCTION_EXIT;
        return m_locationKey;
    }


    RadioSessionData::RadioSessionObject& RadioSessionData::getRadioSessionObject()
    {
        FUNCTION_ENTRY( "getRadioSessionObject" );

        // if m_agentObject is NULL
        // create a new one using the corba name

        TA_THREADGUARD( m_agentObjectLock );

        if ( NULL == m_agentObject )
        {
            m_agentObject = new RadioSessionObject( m_corbaName );
        }


        FUNCTION_EXIT;

        return *m_agentObject;
    }

} // TA_IRS_App
