/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_mft_agent_icdv4/src/TimeoutCallback.h $
 * @author:  Glen Kidd
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 * Manage timeout so caller doesn't have to.
 */

#ifndef TIMEOUT_CALLBACK_H_INCLUDED
#define TIMEOUT_CALLBACK_H_INCLUDED

#pragma warning (disable : 4786)

#include <string>

#include "ace/Future.h"
#include "app/radio/radio_mft_agent_icdv4/src/FutureResult.h"

typedef MConsoleResult_T<unsigned int> FutureResultT;

class TimeoutCallback
{
	public:

		TimeoutCallback(const ACE_Time_Value& timeout);
		virtual ~TimeoutCallback();

		void setFuture(ACE_Future< MConsoleResult_T<unsigned int> >& future);

        ACE_Time_Value getTimeoutTime() { return m_timeoutTime;};

		virtual int handle_timeout(const ACE_Time_Value &current_time,const void *arg);

		ACE_Future<FutureResultT> getFuture();

		static std::string commandToString(unsigned int id);

	private:

		ACE_Future<FutureResultT> m_future;
        ACE_Time_Value m_timeoutTime;
};


#endif 
