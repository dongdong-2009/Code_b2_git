/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/src/RadioCallStackUpdateTask.cpp $
  * @author  R. van Hugten
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2008/11/28 16:26:01 $
  * Last modified by : $Author: builder $
  *
  * Declares an abstract base class for elements that can appear on the
  * RadioBackgroundProcessor request queue.
  *
  */
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#include "bus/radio/RadioAgentCorbaDef_4669/idl/src/IRadioCorbaDef.h"

#include "app/radio/RadioTetraAgent/src/RadioCallStackUpdateTask.h"
#include "app/radio/RadioTetraAgent/src/RadioTcpServer.h"
#include "app/radio/RadioTetraAgent/src/RadioHelper.h"
#include "app/radio/RadioTetraAgent/src/RadioSessionHelper.h"
#include "app/radio/RadioTetraAgent/src/RadioSessionException.h"
#include "app/radio/RadioTetraAgent/src/RadioSubscriberMonitor.h"
#include "app/radio/RadioTetraAgent/src/RadioSession.h"

namespace TA_IRS_App
{

    RadioCallStackUpdateTask::RadioCallStackUpdateTask
    (   RadioTcpServer * radio,
        const TA_IRS_App::CallDetailsType& callDetails,
		TA_IRS_App::ECallPriority defaultEmergencyCallPriority)
        : RadioTask(radio)
        , m_callDetails(callDetails)
		, m_DefaultEmergencyCallPriority(defaultEmergencyCallPriority)
    {
        m_taskName = "RadioCallStackUpdateTask";
        
        // add directly to the call stack
        //getRadio().addCallToCallStack(m_callDetails);
    }


    RadioCallStackUpdateTask::~RadioCallStackUpdateTask()
    {
    }

	void RadioCallStackUpdateTask::setDefaultEmergencyCallPriority(TA_IRS_App::ECallPriority defaultEmergencyCallPriority)
	{
		m_DefaultEmergencyCallPriority = defaultEmergencyCallPriority;
	}


    void RadioCallStackUpdateTask::perform()
    {
        FUNCTION_ENTRY("perform()");
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "callRef:%lu sessionRef:%lu ", m_callDetails.callID, m_callDetails.sessionRef);

        unsigned long callRef = m_callDetails.callID;
        unsigned long sessRef = m_callDetails.sessionRef;

        RadioSessionHelper * helper = new RadioSessionHelper(sessRef, getRadio());

        QueryKeyValueMap callValues;
        callValues[(unsigned long)RadioMethodQueryReference::K_PRIORITY] = "";
        callValues[(unsigned long)RadioMethodQueryReference::K_CALLING] = "";
        callValues[(unsigned long)RadioMethodQueryReference::K_CALLED] = "";
        callValues[(unsigned long)RadioMethodQueryReference::K_CALL_TYPE] = "";
		helper->getCallDetails( callRef, callValues );
		// WAS:
        // helper->queryReferenceAsMap( callRef, RadioMethodQueryReference::K_KEY_ALL_DATACALL, callValues);

        //
        // CALL CONNECT TIME not available on un-monitored calls (which this one may very well not be!)
        // however callTime and time fields will be set by addCallToCallStack()

        //
        // Emergency/Priority detail
        // The isEmergency flag is usually cleared by default.  If it is
        // already set, leave it alone, otherwise get the priority from 
        // the RCS TCPServer
        if (!m_callDetails.isEmergency)
        {
            std::string priority_str = RadioHelper::getStdStringValue(callValues, RadioMethodQueryReference::K_PRIORITY);
            int priority = atoi(priority_str.c_str());
            m_callDetails.isEmergency = (priority == static_cast<int> (m_DefaultEmergencyCallPriority));
        }

		std::string call_type_str = RadioHelper::getStdStringValue(callValues, RadioMethodQueryReference::K_CALL_TYPE);
		ECallType call_type = helper->convert2CallType(call_type_str);
		// bool isIndividualCall = (call_type == SingleCall);

        // 
        // We might encounter exceptions with retrieving subscriber details, but
        // we already have the two most important TSIs so might as well stick
        // them in the structure.
        std::string subscriberTSI = RadioHelper::getStdStringValue(callValues, RadioMethodQueryReference::K_CALLING);
        m_callDetails.callerTSI       = subscriberTSI;
        subscriberTSI = RadioHelper::getStdStringValue(callValues, RadioMethodQueryReference::K_CALLED);
        m_callDetails.calledTSI       = subscriberTSI;

        try
        {
            //
            // CALLING Subscriber details

            QueryKeyValueMap subscriberDetails;
            subscriberDetails[(unsigned long)RadioMethodQueryReference::K_NAME] = "";
            subscriberDetails[(unsigned long)RadioMethodQueryReference::K_ORGANISATION] = "";
            int num_details = helper->getSubscriberDetails(m_callDetails.callerTSI, subscriberDetails);
            if ( num_details == 2 )
            {
                m_callDetails.callerName      = subscriberDetails[(unsigned long)RadioMethodQueryReference::K_NAME];
                m_callDetails.callerLocation  = subscriberDetails[(unsigned long)RadioMethodQueryReference::K_ORGANISATION];

                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Call participant details, callerName:%s callerTSI:%s callerLocation:%s", m_callDetails.callerName.c_str(), m_callDetails.callerTSI.c_str(), m_callDetails.callerLocation.c_str());
            }
            else
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Could not retrieve Calling subscriber details");
            }

            if (m_callDetails.callerName.empty())
            {
                m_callDetails.callerName = m_callDetails.callerTSI;
            }

            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Voice Call callerName:%s callerTSI:%s callerLocation:%s", m_callDetails.callerName.c_str(), m_callDetails.callerTSI.c_str(), m_callDetails.callerLocation.c_str());

            //
            // CALLED Subscriber details

            // QueryKeyValueMap subscriberDetails;
            subscriberDetails[(unsigned long)RadioMethodQueryReference::K_NAME] = "";
            subscriberDetails[(unsigned long)RadioMethodQueryReference::K_ORGANISATION] = "";
            num_details = helper->getSubscriberDetails(m_callDetails.calledTSI, subscriberDetails);
            if ( num_details == 2 )
            {
                m_callDetails.calledName      = subscriberDetails[(unsigned long)RadioMethodQueryReference::K_NAME];
                m_callDetails.calledLocation  = subscriberDetails[(unsigned long)RadioMethodQueryReference::K_ORGANISATION];

                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Call participant details, calledName:%s calledTSI:%s calledLocation:%s", m_callDetails.calledName.c_str(), m_callDetails.calledTSI.c_str(), m_callDetails.calledLocation.c_str());
            }
            else
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Could not retrieve Called subscriber details");
            }

            if (m_callDetails.calledName.empty())
            {
                m_callDetails.calledName = m_callDetails.calledTSI;
            }

            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Voice Call calledName:%s calledTSI:%s calledLocation:%s", m_callDetails.calledName.c_str(), m_callDetails.calledTSI.c_str(), m_callDetails.calledLocation.c_str());
        }
        catch (RadioSessionException rse)
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Caught RadioSessionException while trying to retrieve subscriber details: %s", rse.what());
        }
        catch (RadioFallbackException rfe)
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Radio in Fallback. Will not retrieve subscriber details");

            std::string replacement = helper->getFallbackReplacementString();
            m_callDetails.callerName = replacement;
            m_callDetails.callerLocation = replacement;
            m_callDetails.calledName = replacement;
            m_callDetails.calledLocation = replacement;
        }

        // the call status and category are generally populated by the generating Radio::function
        // LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "CallStackUpdate details, callStatus:%d calledCategory:%d", (int)(m_callDetails->callStatus), (int)(m_callDetails->callCategory));

        // now that the call details have been populated, add the call to the stack.
        getRadio().addCallToCallStack(m_callDetails);


		if (m_callDetails.callSource == IncomingCall)
		{
			// can only do this after adding to the callstack
			if (getRadio().getSubscriberMonitor().isListenCall(callRef))
			{
				RadioSession * session = getRadio().findSession(sessRef);

				if (0 != session)
				{
					session->answerCall(callRef, "");
				}
			}
		} 
        // call stack has a by-value copy of the details now
       
    }
} // namespace TA_IRS_App
