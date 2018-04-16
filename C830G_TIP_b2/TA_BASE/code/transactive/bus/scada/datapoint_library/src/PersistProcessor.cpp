/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/4669_T01271350/TA_BASE/transactive/bus/scada/datapoint_library/src/PersistProcessor.cpp $
  * @author  Raymond Pau
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2008/11/28 16:26:01 $
  * Last modified by : $Author: builder $
  *
  *
  *	The PersistProcessor thread is responsible 
  * for persisting datapoint value and status.
  *
  */
#include <sstream>

#include "bus/scada/datapoint_library/src/PersistProcessor.h"

namespace TA_Base_Bus
{

	PersistProcessor::PersistProcessor()
    {
        FUNCTION_ENTRY("Constructor");

        start();

        FUNCTION_EXIT;
	}


	PersistProcessor::~PersistProcessor()
	{
        FUNCTION_ENTRY("Destructor");

        terminateAndWait();

        FUNCTION_EXIT;
	}

	void PersistProcessor::processEvent( PersistRequest * newPersistRequest )
	{
        FUNCTION_ENTRY("processEvent");

        TA_ASSERT( NULL != m_persistence.get(), "m_persistence had not been initialized" );

        if ( NULL == m_persistence.get() )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "m_persistence had not been initialized" );
            return;
        }

        EPersistedItem item = newPersistRequest->getItem();
        unsigned long entityKey = newPersistRequest->getEntityKey();
        std::string value = newPersistRequest->getValue();
        EQualityStatus quality = newPersistRequest->getQuality();

        std::ostringstream desc;
        desc << "Persisting Entity " << entityKey
             << " item: " << item
             << " value: " << value
             << " quality: " << quality;
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, desc.str().c_str() );

        if ( false == m_persistence->writePersistedValue( item, entityKey, value, quality ) )
        {
            // log error message
            std::ostringstream msg;
            msg << "updateControlStatus() failed to persist the status '" << newPersistRequest->getValue() << "' to database";
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, msg.str().c_str() );
        }

        FUNCTION_EXIT;
    }

}
