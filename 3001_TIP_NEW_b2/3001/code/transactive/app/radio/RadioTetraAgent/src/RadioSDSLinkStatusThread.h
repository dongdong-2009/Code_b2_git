/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: \\depot\3001_TIP_NEW\3001\transactive\app\radio\RadioTetraAgent\src\RadioSDSLinkStatus.h $
 * @author:  Lucky Cuizon
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2013/11/03 15:18:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 * This class handles the Datapoint updates for SDS Server Link status
 */

#ifndef _RADIO_SDS_LINK_STATUS_THREAD_H_
#define _RADIO_SDS_LINK_STATUS_THREAD_H_

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/Semaphore.h"
#include "app/radio/RadioTetraAgent/src/RadioSDSSummary.h"

namespace TA_Base_Bus
{
	class DataPoint;
}

namespace TA_IRS_App
{
	class IRadioSDSLinkStatus
	{
	public:
		/**
         * Sets the link status and update the datapoint
		 *
		 * @param : status - Link status
         */
		virtual void setLinkStatus(bool status) = 0;
	};
}

namespace TA_IRS_App
{
	class RadioSDSLinkStatusThread : public TA_Base_Core::Thread, public IRadioSDSLinkStatus
	{
	public:
		typedef std::map <unsigned long, TA_Base_Bus::DataPoint *> DataPointList;
		/**
		 * Constructor
		 */
		RadioSDSLinkStatusThread();

		/**
		 * Destructor
		 */
		virtual ~RadioSDSLinkStatusThread();

		/**
         * Set the server status datapoint configuration
         *
		 * @param  : DataPointList& - Datapoint list
         * @param  : RadioSDSSummary* sdsSummary - Contains the Datapoint name based on entityparameter value
         */
        void setDataPointConfig( DataPointList& dpList, RadioSDSSummary* sdsSummary );

		/**
         * Sets the link status and update the datapoint
		 *
		 * @param : status - Link status
         */
        void setLinkStatus( bool status );

        
        /**
         * Processing loop
         */
        void run();

        /**
         * Signals the thread to stop, called by terminateAndWait
         */
        void terminate();

	private:
		/**
		 * Disable copy and assignment operator
		 */
		RadioSDSLinkStatusThread(const RadioSDSLinkStatusThread& );
		RadioSDSLinkStatusThread& operator= (const RadioSDSLinkStatusThread& );

		/**
         * Set the server status datapoint
         *
		 * @param  : TA_Base_Bus::DataPoint* dataPoint - Datapoint to update the SDS Link
         * @return : bool - Return true if successfully set
         */
		void setDataPoint(TA_Base_Bus::DataPoint* dataPoint);

		/**
         * The server status datapoint
         */
        TA_Base_Bus::DataPoint* m_dataPoint;

        /**
         * Whether to stop running
         */
        bool m_terminate;

        /**
         * The server link status
         */
        volatile bool m_status;

        /**
         * Used to control the thread
         */
		TA_Base_Core::Semaphore m_semaphore;
		TA_Base_Core::ReEntrantThreadLockable	m_statusLock;

	};
}
#endif