
#ifndef IDATA_CALL_H
#define IDATA_CALL_H

#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/CommonTypes.h"

#include <string>

namespace TA_IRS_App
{

	/**
     * Used to handle makeDataCall, EndDataCall and cancelDataCall,
     */

	class IDataCall
	{

		public:

			/**
			 * This sets up a data call to transfer from given data.
			 * @param data    The raw bytes to send
			 */
			virtual void makeDataCall( const long & lTrainID, const TA_IRS_App::CommonTypes::RadioPDSMessage& data ) = 0;
			
			/**
			 * This ends a data call in progress to the given ITSI.
			 */
			virtual void cancelDataCall(const long & lTrainID) = 0;

			/**
			 * This will end a call that is in progress. It will also remove the call from the ongoing calls list in the status manager.
			 */
			virtual void endDataCall(const long & lTrainID) = 0;
                             
                                     
                                   

	};
}

#endif // END of IDATA_CALL_H