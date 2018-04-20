// DoubleAtsFailureWorkItem.h: interface for the DoubleAtsFailureWorkItem class.
//
//////////////////////////////////////////////////////////////////////


#ifndef DOUBLE_ATS_FAILURE_WORK_TIME_H
#define DOUBLE_ATS_FAILURE_WORK_TIME_H

#include "core/threads/src/IWorkItem.h"

#include "bus/trains/TrainCorbaProxy/src/TrainOperationModeCorbaProxy.h"
namespace TA_IRS_App
{

	/**
     * Work item used to notify observers of a call end event
     * @author Lucky
     * @version 1.0
     * @created 25-July-2011 2:47:19 PM
     */

	class DoubleAtsFailureWorkItem : public TA_Base_Core::IWorkItem  
	{
	public:
		
		/**
         * Constructor
         *
         * @param observer    The observer to notify
         * @param callDetails    The details of the call that ended
         * @param trainList    The trains involved in the radio call
         */
		DoubleAtsFailureWorkItem( TA_IRS_Bus::TrainOperationModeCorbaProxyPtr observer, bool AtsFailure);

		/**
		 * Destructor
		 */
		virtual ~DoubleAtsFailureWorkItem();

		/**
		 * Executes the work item
		 */
		virtual void executeWorkItem();

	private:

		/**
         * Private default constructor
         */
		DoubleAtsFailureWorkItem();

		/**
		 * The observer to call
		 */
		TA_IRS_Bus::TrainOperationModeCorbaProxyPtr m_observer;

		/**
		 * The bool to pass to the observer
		 */
		bool doubleAtsFailure;

	};

}

#endif // !defined(AFX_DOUBLEATSFAILUREWORKITEM_H__04D6F435_DAB4_4549_A4D3_C248B257EF48__INCLUDED_)
