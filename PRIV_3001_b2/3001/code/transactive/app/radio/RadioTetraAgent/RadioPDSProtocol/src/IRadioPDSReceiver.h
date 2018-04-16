
#ifndef IRADIO_PDS_RECEIVER_H
#define IRADIO_PDS_RECEIVER_H

#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/IPDSEvent.h"

namespace TA_IRS_App
{

	class IRadioPDSReceiver
	{

		public:

			/**
			 * This is implemented by concrete classes to process incoming events.
			 * 
			 * @param event
			 */
			virtual void processEvent(TA_IRS_App::IPDSEvent * event ) = 0;
		
	};
}

#endif //END of IRADIO_PDS_EVENT_PROCESSOR_H