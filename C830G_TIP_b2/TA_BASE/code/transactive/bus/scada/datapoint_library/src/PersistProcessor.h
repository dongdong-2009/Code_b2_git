/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/4669_T01271350/TA_BASE/transactive/bus/scada/datapoint_library/src/PersistProcessor.h $
  * @author  Raymond Pau
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2008/11/28 16:26:01 $
  * Last modified by : $Author: builder $
  *
  *
  * The PersistProcessor thread is responsible
  * for writing persisted datapoint value
  *
  */

#ifndef PERSISTENCEPROCESSOR_H
#define PERSISTENCEPROCESSOR_H

#include "bus/scada/common_library/src/EventProcessor.h"
#include "bus/scada/common_library/src/ScadaPersistence.h"
#include "bus/scada/datapoint_library/src/PersistRequest.h"

namespace TA_Base_Bus
{
    class PersistProcessor : public EventProcessor< PersistRequest >
	{

	public:

		PersistProcessor();
		~PersistProcessor();

        void setScadaPersistencePtr( ScadaPersistencePtr persistence )
        {
            m_persistence = persistence;
        };

	private:

		/**
		 * PersistProcessor::processEvent()
		 *
		 * Generates an Event based on the request, and logs it
		 *
		 * @param newEvent	the request for the new event
		 *
		 */
		virtual void processEvent( PersistRequest * newPersistRequest );

    private:
        ScadaPersistencePtr m_persistence;
	};

}

#endif
