/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_mft_agent/src/EventToResultFactory.h $
 * @author:  Glen Kidd
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2010/08/12 11:47:25 $
 * Last modified by:  $Author: builder $
 * 
 * Manage the mapping of incoming RadioEvents to appropriate FutureResult's.
 */

#ifndef EVENT_TO_RESULT_FACTORY_H_INCLUDED_
#define EVENT_TO_RESULT_FACTORY_H_INCLUDED_


#include "app/radio/radio_mft_agent/src/FutureResult.h"
#include "app/radio/radio_mft_agent/src/FutureManager.h"
#include "app/radio/radio_mft_agent/src/RemoteAPIRadioEvent.h"

using namespace TA_IRS_App;

namespace TA_IRS_App
{
	class RemoteAPIRadioEvent;
}
class EventToResultFactory
{
	public:

		static ACE_Future<FutureResultT> getResult(FutureManager* pFutureMgr,TA_IRS_App::RemoteAPIRadioEvent* event);
	private:
	    //Disabled Methods
		EventToResultFactory();
		virtual ~EventToResultFactory();
		EventToResultFactory(const EventToResultFactory&);
		EventToResultFactory& operator=(const EventToResultFactory&);

};
#endif // !defined({9B83EC6D-597A-4132-9837-4F1122F19B21}__INCLUDED_)
