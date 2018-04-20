/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/src/IApplicationObserver.h $
 * @author:  Anita Lee
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2008/11/28 16:26:01 $
 * Last modified by:  $Author: builder $
 * 
 * IApplicationObserver is an interface that will allows any DataAccessInterface class to listen for notifications from 
 * the application that may affect any database accessing that may be currently in progress.
 */


#if !defined(IAPPLICATION_OBSERVER_H)
#define IAPPLICATION_OBSERVER_H

namespace TA_Base_Core
{

   
	class IApplicationObserver
	{

	public:

		/**
         * applicationTerminating
         *
         * Called when the application is shutting down. This method will interrupt a current load that may be
         * taking a long time, so that termination can be done immediately.
         */
        virtual void applicationTerminating() =0;

        virtual void applicationPausing() =0;

        virtual void applicationResuming() =0;

        virtual ~IApplicationObserver() {};
	};

} //close namespace TA_Base_Core

#endif // !defined(IAPPLICATION_OBSERVER_H)
