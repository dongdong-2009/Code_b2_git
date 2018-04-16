/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/TA_Common_V1_TIP/transactive/app/scada/WFAgent/src/WFCommandPolicy.h $
  * @author  J. Welton
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2016/01/22 14:37:26 $
  * Last modified by : $Author: CM $
  *
  *
  * wf control command control state changing policy.
  *
  *
  */

#ifndef WF_COMMAND_POLICY_H
#define WF_COMMAND_POLICY_H

#include <string>
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"

namespace TA_IRS_App
{
	enum ECommandPolicyResult
	{
		PR_FAILED,
		PR_ACCEPTED,
		PR_IGNORED,
	};

	class PolicyResult
	{
	public:
		PolicyResult():
		  m_result(PR_ACCEPTED),
		  m_newControlState(TA_Base_Bus::noControl),
		  m_desc("unknow policy, just accept.")
		{
		};
		PolicyResult(ECommandPolicyResult result,
			TA_Base_Bus::EDataPointControlState newControlState,
			std::string desc)
		{
			m_result = result;
			m_newControlState = newControlState;
			m_desc = desc;
		};

		ECommandPolicyResult m_result;
		TA_Base_Bus::EDataPointControlState m_newControlState;
		std::string	m_desc;
	};

    class WfCommandPolicy
    {
    public:
		 static ECommandPolicyResult updateControlState(TA_Base_Bus::DataPoint* dp, TA_Base_Bus::EDataPointControlState newControlState,std::string strInfo, bool isRequestedBySystem);
   
    protected:
        WfCommandPolicy();
        virtual ~WfCommandPolicy();
		static void initPolicyData();
	
	private:
        static TA_Base_Core::ReEntrantThreadLockable    m_policyDataLock;
		static int				m_maxControlState;
		static PolicyResult*	m_policyData;	// ori control state to new control state.
    };
}
#endif
