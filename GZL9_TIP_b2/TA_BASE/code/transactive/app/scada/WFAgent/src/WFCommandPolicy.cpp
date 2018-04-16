/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/TA_Common_V1_TIP/transactive/app/scada/WFAgent/src/WFCommandPolicy.cpp $
  * @author  A. Parker
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2016/01/22 14:37:26 $
  * Last modified by : $Author: CM $
  *
  */

#include "app/scada/WFAgent/src/WFCommandPolicy.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"

using namespace TA_Base_Bus;

namespace TA_IRS_App
{

	int				WfCommandPolicy::m_maxControlState = 0;
    PolicyResult*	WfCommandPolicy::m_policyData = 0;
    TA_Base_Core::ReEntrantThreadLockable WfCommandPolicy::m_policyDataLock;

    ECommandPolicyResult WfCommandPolicy::updateControlState(TA_Base_Bus::DataPoint* dp, EDataPointControlState newControlState,std::string strInfo, bool isRequestedBySystem)
    {
		ECommandPolicyResult ret = PR_ACCEPTED;
        if (0 == m_policyData)
        {
            initPolicyData();
        }
        TA_ASSERT(m_policyData != 0, "m_policyData == 0");

		const EDataPointControlState currentState = dp->getControlState().controlState;
		if (currentState < m_maxControlState && newControlState < m_maxControlState)
		{
			PolicyResult& thePolicy = m_policyData[m_maxControlState * currentState + newControlState];
			ret = thePolicy.m_result;

			if (PR_IGNORED == thePolicy.m_result)
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
					"datapoint:%s updated control state %s, result is:%d, ignored",
					dp->getDataPointName().c_str(), thePolicy.m_desc.c_str(), thePolicy.m_result);
			}
			else
			{
				if (0 == strInfo.length())
				{
					dp->setControlState (thePolicy.m_newControlState, isRequestedBySystem);
				}
				else
				{
					dp->setControlState (thePolicy.m_newControlState,strInfo,isRequestedBySystem);
				}

				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
					"datapoint:%s updated control state %s, result is:%d, new control state is:%d ,info:%s.",
					dp->getDataPointName().c_str(), thePolicy.m_desc.c_str(), thePolicy.m_result, thePolicy.m_newControlState,strInfo.c_str());
			}
		}
		else
		{
			dp->updateControlState(newControlState, isRequestedBySystem);

			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
				"datapoint:%s, one of the control state current[%d] or new[%d] is big then m_maxControlState[%d], directly updated to new state",
				dp->getDataPointName().c_str(), currentState, newControlState, m_maxControlState);
			ret = PR_ACCEPTED;
		}
		return ret;
    };

	void WfCommandPolicy::initPolicyData()
	{
		TA_THREADGUARD( m_policyDataLock );

        if (0 == m_policyData)
        {
			m_maxControlState = TA_Base_Bus::deviceControlException + 1;
			if (m_maxControlState > 1024)
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
                        "m_maxControlState[%d] is bigger then 1024", m_maxControlState);
			}

			PolicyResult* policyData = new PolicyResult[m_maxControlState * m_maxControlState];

		policyData[m_maxControlState * noControl                + noControl ]                = PolicyResult(PR_ACCEPTED, noControl ,                "from noControl to noControl ");
		policyData[m_maxControlState * written                  + noControl ]                = PolicyResult(PR_ACCEPTED, noControl ,                "from written to noControl ");
		policyData[m_maxControlState * pending                  + noControl ]                = PolicyResult(PR_ACCEPTED, noControl ,                "from pending to noControl ");
		policyData[m_maxControlState * conditionalSuccess       + noControl ]                = PolicyResult(PR_ACCEPTED, noControl ,                "from conditionalSuccess to noControl ");
		policyData[m_maxControlState * success                  + noControl ]                = PolicyResult(PR_ACCEPTED, noControl ,                "from success to noControl ");
		policyData[m_maxControlState * failed                   + noControl ]                = PolicyResult(PR_ACCEPTED, noControl ,                "from failed to noControl ");
		policyData[m_maxControlState * comms_failed             + noControl ]                = PolicyResult(PR_ACCEPTED, noControl ,                "from comms_failed to noControl ");
		policyData[m_maxControlState * selectCmdWritten         + noControl ]                = PolicyResult(PR_ACCEPTED, noControl ,                "from selectCmdWritten to noControl ");
		policyData[m_maxControlState * selectCmdConfirm         + noControl ]                = PolicyResult(PR_ACCEPTED, noControl ,                "from selectCmdConfirm to noControl ");
		policyData[m_maxControlState * selectCmdConfirmTimeout  + noControl ]                = PolicyResult(PR_ACCEPTED, noControl ,                "from selectCmdConfirmTimeout to noControl ");
		policyData[m_maxControlState * selectCmdCfmNegative     + noControl ]                = PolicyResult(PR_ACCEPTED, noControl ,                "from selectCmdCfmNegative to noControl ");
		policyData[m_maxControlState * deviceControlException   + noControl ]                = PolicyResult(PR_ACCEPTED, noControl ,                "from deviceControlException to noControl ");
		policyData[m_maxControlState * noControl                + written ]                  = PolicyResult(PR_ACCEPTED, written ,                  "from noControl to written ");
		policyData[m_maxControlState * written                  + written ]                  = PolicyResult(PR_ACCEPTED, written ,                  "from written to written ");
		policyData[m_maxControlState * pending                  + written ]                  = PolicyResult(PR_ACCEPTED, written ,                  "from pending to written ");
		policyData[m_maxControlState * conditionalSuccess       + written ]                  = PolicyResult(PR_ACCEPTED, written ,                  "from conditionalSuccess to written ");
		policyData[m_maxControlState * success                  + written ]                  = PolicyResult(PR_ACCEPTED, written ,                  "from success to written ");
		policyData[m_maxControlState * failed                   + written ]                  = PolicyResult(PR_ACCEPTED, written ,                  "from failed to written ");
		policyData[m_maxControlState * comms_failed             + written ]                  = PolicyResult(PR_ACCEPTED, written ,                  "from comms_failed to written ");
		policyData[m_maxControlState * selectCmdWritten         + written ]                  = PolicyResult(PR_ACCEPTED, written ,                  "from selectCmdWritten to written ");
		policyData[m_maxControlState * selectCmdConfirm         + written ]                  = PolicyResult(PR_ACCEPTED, written ,                  "from selectCmdConfirm to written ");
		policyData[m_maxControlState * selectCmdConfirmTimeout  + written ]                  = PolicyResult(PR_ACCEPTED, written ,                  "from selectCmdConfirmTimeout to written ");
		policyData[m_maxControlState * selectCmdCfmNegative     + written ]                  = PolicyResult(PR_ACCEPTED, written ,                  "from selectCmdCfmNegative to written ");
		policyData[m_maxControlState * deviceControlException   + written ]                  = PolicyResult(PR_ACCEPTED, written ,                  "from deviceControlException to written ");
		policyData[m_maxControlState * noControl                + pending ]                  = PolicyResult(PR_ACCEPTED, pending ,                  "from noControl to pending ");
		policyData[m_maxControlState * written                  + pending ]                  = PolicyResult(PR_ACCEPTED, pending ,                  "from written to pending ");
		policyData[m_maxControlState * pending                  + pending ]                  = PolicyResult(PR_ACCEPTED, pending ,                  "from pending to pending ");
		policyData[m_maxControlState * conditionalSuccess       + pending ]                  = PolicyResult(PR_ACCEPTED, pending ,                  "from conditionalSuccess to pending ");
		policyData[m_maxControlState * success                  + pending ]                  = PolicyResult(PR_ACCEPTED, pending ,                  "from success to pending ");
		policyData[m_maxControlState * failed                   + pending ]                  = PolicyResult(PR_ACCEPTED, pending ,                  "from failed to pending ");
		policyData[m_maxControlState * comms_failed             + pending ]                  = PolicyResult(PR_ACCEPTED, pending ,                  "from comms_failed to pending ");
		policyData[m_maxControlState * selectCmdWritten         + pending ]                  = PolicyResult(PR_ACCEPTED, pending ,                  "from selectCmdWritten to pending ");
		policyData[m_maxControlState * selectCmdConfirm         + pending ]                  = PolicyResult(PR_ACCEPTED, pending ,                  "from selectCmdConfirm to pending ");
		policyData[m_maxControlState * selectCmdConfirmTimeout  + pending ]                  = PolicyResult(PR_ACCEPTED, pending ,                  "from selectCmdConfirmTimeout to pending ");
		policyData[m_maxControlState * selectCmdCfmNegative     + pending ]                  = PolicyResult(PR_ACCEPTED, pending ,                  "from selectCmdCfmNegative to pending ");
		policyData[m_maxControlState * deviceControlException   + pending ]                  = PolicyResult(PR_ACCEPTED, pending ,                  "from deviceControlException to pending ");
		policyData[m_maxControlState * noControl                + conditionalSuccess ]       = PolicyResult(PR_ACCEPTED, conditionalSuccess ,       "from noControl to conditionalSuccess ");
		policyData[m_maxControlState * written                  + conditionalSuccess ]       = PolicyResult(PR_ACCEPTED, conditionalSuccess ,       "from written to conditionalSuccess ");
		policyData[m_maxControlState * pending                  + conditionalSuccess ]       = PolicyResult(PR_ACCEPTED, conditionalSuccess ,       "from pending to conditionalSuccess ");
		policyData[m_maxControlState * conditionalSuccess       + conditionalSuccess ]       = PolicyResult(PR_ACCEPTED, conditionalSuccess ,       "from conditionalSuccess to conditionalSuccess ");
		policyData[m_maxControlState * success                  + conditionalSuccess ]       = PolicyResult(PR_ACCEPTED, conditionalSuccess ,       "from success to conditionalSuccess ");
		policyData[m_maxControlState * failed                   + conditionalSuccess ]       = PolicyResult(PR_ACCEPTED, conditionalSuccess ,       "from failed to conditionalSuccess ");
		policyData[m_maxControlState * comms_failed             + conditionalSuccess ]       = PolicyResult(PR_ACCEPTED, conditionalSuccess ,       "from comms_failed to conditionalSuccess ");
		policyData[m_maxControlState * selectCmdWritten         + conditionalSuccess ]       = PolicyResult(PR_ACCEPTED, conditionalSuccess ,       "from selectCmdWritten to conditionalSuccess ");
		policyData[m_maxControlState * selectCmdConfirm         + conditionalSuccess ]       = PolicyResult(PR_ACCEPTED, conditionalSuccess ,       "from selectCmdConfirm to conditionalSuccess ");
		policyData[m_maxControlState * selectCmdConfirmTimeout  + conditionalSuccess ]       = PolicyResult(PR_ACCEPTED, conditionalSuccess ,       "from selectCmdConfirmTimeout to conditionalSuccess ");
		policyData[m_maxControlState * selectCmdCfmNegative     + conditionalSuccess ]       = PolicyResult(PR_ACCEPTED, conditionalSuccess ,       "from selectCmdCfmNegative to conditionalSuccess ");
		policyData[m_maxControlState * deviceControlException   + conditionalSuccess ]       = PolicyResult(PR_ACCEPTED, conditionalSuccess ,       "from deviceControlException to conditionalSuccess ");
		policyData[m_maxControlState * noControl                + success ]                  = PolicyResult(PR_ACCEPTED, success ,                  "from noControl to success ");
		policyData[m_maxControlState * written                  + success ]                  = PolicyResult(PR_ACCEPTED, success ,                  "from written to success ");
		policyData[m_maxControlState * pending                  + success ]                  = PolicyResult(PR_ACCEPTED, success ,                  "from pending to success ");
		policyData[m_maxControlState * conditionalSuccess       + success ]                  = PolicyResult(PR_ACCEPTED, success ,                  "from conditionalSuccess to success ");
		policyData[m_maxControlState * success                  + success ]                  = PolicyResult(PR_ACCEPTED, success ,                  "from success to success ");
		policyData[m_maxControlState * failed                   + success ]                  = PolicyResult(PR_ACCEPTED, success ,                  "from failed to success ");
		policyData[m_maxControlState * comms_failed             + success ]                  = PolicyResult(PR_ACCEPTED, success ,                  "from comms_failed to success ");
		policyData[m_maxControlState * selectCmdWritten         + success ]                  = PolicyResult(PR_ACCEPTED, success ,                  "from selectCmdWritten to success ");
		policyData[m_maxControlState * selectCmdConfirm         + success ]                  = PolicyResult(PR_ACCEPTED, success ,                  "from selectCmdConfirm to success ");
		policyData[m_maxControlState * selectCmdConfirmTimeout  + success ]                  = PolicyResult(PR_ACCEPTED, success ,                  "from selectCmdConfirmTimeout to success ");
		policyData[m_maxControlState * selectCmdCfmNegative     + success ]                  = PolicyResult(PR_ACCEPTED, success ,                  "from selectCmdCfmNegative to success ");
		policyData[m_maxControlState * deviceControlException   + success ]                  = PolicyResult(PR_ACCEPTED, success ,                  "from deviceControlException to success ");
		policyData[m_maxControlState * noControl                + failed ]                   = PolicyResult(PR_ACCEPTED, failed ,                   "from noControl to failed ");
		policyData[m_maxControlState * written                  + failed ]                   = PolicyResult(PR_ACCEPTED, failed ,                   "from written to failed ");
		policyData[m_maxControlState * pending                  + failed ]                   = PolicyResult(PR_ACCEPTED, failed ,                   "from pending to failed ");
		policyData[m_maxControlState * conditionalSuccess       + failed ]                   = PolicyResult(PR_ACCEPTED, failed ,                   "from conditionalSuccess to failed ");
		policyData[m_maxControlState * success                  + failed ]                   = PolicyResult(PR_ACCEPTED, failed ,                   "from success to failed ");
		policyData[m_maxControlState * failed                   + failed ]                   = PolicyResult(PR_ACCEPTED, failed ,                   "from failed to failed ");
		policyData[m_maxControlState * comms_failed             + failed ]                   = PolicyResult(PR_ACCEPTED, failed ,                   "from comms_failed to failed ");
		policyData[m_maxControlState * selectCmdWritten         + failed ]                   = PolicyResult(PR_ACCEPTED, failed ,                   "from selectCmdWritten to failed ");
		policyData[m_maxControlState * selectCmdConfirm         + failed ]                   = PolicyResult(PR_ACCEPTED, failed ,                   "from selectCmdConfirm to failed ");
		policyData[m_maxControlState * selectCmdConfirmTimeout  + failed ]                   = PolicyResult(PR_ACCEPTED, failed ,                   "from selectCmdConfirmTimeout to failed ");
		policyData[m_maxControlState * selectCmdCfmNegative     + failed ]                   = PolicyResult(PR_ACCEPTED, failed ,                   "from selectCmdCfmNegative to failed ");
		policyData[m_maxControlState * deviceControlException   + failed ]                   = PolicyResult(PR_ACCEPTED, failed ,                   "from deviceControlException to failed ");
		policyData[m_maxControlState * noControl                + comms_failed ]             = PolicyResult(PR_ACCEPTED, comms_failed ,             "from noControl to comms_failed ");
		policyData[m_maxControlState * written                  + comms_failed ]             = PolicyResult(PR_ACCEPTED, comms_failed ,             "from written to comms_failed ");
		policyData[m_maxControlState * pending                  + comms_failed ]             = PolicyResult(PR_ACCEPTED, comms_failed ,             "from pending to comms_failed ");
		policyData[m_maxControlState * conditionalSuccess       + comms_failed ]             = PolicyResult(PR_ACCEPTED, comms_failed ,             "from conditionalSuccess to comms_failed ");
		policyData[m_maxControlState * success                  + comms_failed ]             = PolicyResult(PR_ACCEPTED, comms_failed ,             "from success to comms_failed ");
		policyData[m_maxControlState * failed                   + comms_failed ]             = PolicyResult(PR_ACCEPTED, comms_failed ,             "from failed to comms_failed ");
		policyData[m_maxControlState * comms_failed             + comms_failed ]             = PolicyResult(PR_ACCEPTED, comms_failed ,             "from comms_failed to comms_failed ");
		policyData[m_maxControlState * selectCmdWritten         + comms_failed ]             = PolicyResult(PR_ACCEPTED, comms_failed ,             "from selectCmdWritten to comms_failed ");
		policyData[m_maxControlState * selectCmdConfirm         + comms_failed ]             = PolicyResult(PR_ACCEPTED, comms_failed ,             "from selectCmdConfirm to comms_failed ");
		policyData[m_maxControlState * selectCmdConfirmTimeout  + comms_failed ]             = PolicyResult(PR_ACCEPTED, comms_failed ,             "from selectCmdConfirmTimeout to comms_failed ");
		policyData[m_maxControlState * selectCmdCfmNegative     + comms_failed ]             = PolicyResult(PR_ACCEPTED, comms_failed ,             "from selectCmdCfmNegative to comms_failed ");
		policyData[m_maxControlState * deviceControlException   + comms_failed ]             = PolicyResult(PR_ACCEPTED, comms_failed ,             "from deviceControlException to comms_failed ");
		policyData[m_maxControlState * noControl                + selectCmdWritten ]         = PolicyResult(PR_ACCEPTED, selectCmdWritten ,         "from noControl to selectCmdWritten ");
		policyData[m_maxControlState * written                  + selectCmdWritten ]         = PolicyResult(PR_ACCEPTED, selectCmdWritten ,         "from written to selectCmdWritten ");
		policyData[m_maxControlState * pending                  + selectCmdWritten ]         = PolicyResult(PR_ACCEPTED, selectCmdWritten ,         "from pending to selectCmdWritten ");
		policyData[m_maxControlState * conditionalSuccess       + selectCmdWritten ]         = PolicyResult(PR_ACCEPTED, selectCmdWritten ,         "from conditionalSuccess to selectCmdWritten ");
		policyData[m_maxControlState * success                  + selectCmdWritten ]         = PolicyResult(PR_ACCEPTED, selectCmdWritten ,         "from success to selectCmdWritten ");
		policyData[m_maxControlState * failed                   + selectCmdWritten ]         = PolicyResult(PR_ACCEPTED, selectCmdWritten ,         "from failed to selectCmdWritten ");
		policyData[m_maxControlState * comms_failed             + selectCmdWritten ]         = PolicyResult(PR_ACCEPTED, selectCmdWritten ,         "from comms_failed to selectCmdWritten ");
		policyData[m_maxControlState * selectCmdWritten         + selectCmdWritten ]         = PolicyResult(PR_ACCEPTED, selectCmdWritten ,         "from selectCmdWritten to selectCmdWritten ");
		policyData[m_maxControlState * selectCmdConfirm         + selectCmdWritten ]         = PolicyResult(PR_ACCEPTED, selectCmdWritten ,         "from selectCmdConfirm to selectCmdWritten ");
		policyData[m_maxControlState * selectCmdConfirmTimeout  + selectCmdWritten ]         = PolicyResult(PR_ACCEPTED, selectCmdWritten ,         "from selectCmdConfirmTimeout to selectCmdWritten ");
		policyData[m_maxControlState * selectCmdCfmNegative     + selectCmdWritten ]         = PolicyResult(PR_ACCEPTED, selectCmdWritten ,         "from selectCmdCfmNegative to selectCmdWritten ");
		policyData[m_maxControlState * deviceControlException   + selectCmdWritten ]         = PolicyResult(PR_ACCEPTED, selectCmdWritten ,         "from deviceControlException to selectCmdWritten ");
		policyData[m_maxControlState * noControl                + selectCmdConfirm ]         = PolicyResult(PR_IGNORED,  noControl ,                "from noControl to selectCmdConfirm ");
		policyData[m_maxControlState * written                  + selectCmdConfirm ]         = PolicyResult(PR_FAILED,   deviceControlException ,   "from written to selectCmdConfirm ");
		policyData[m_maxControlState * pending                  + selectCmdConfirm ]         = PolicyResult(PR_FAILED,   deviceControlException ,   "from pending to selectCmdConfirm ");
		policyData[m_maxControlState * conditionalSuccess       + selectCmdConfirm ]         = PolicyResult(PR_IGNORED,  noControl ,                "from conditionalSuccess to selectCmdConfirm ");
		policyData[m_maxControlState * success                  + selectCmdConfirm ]         = PolicyResult(PR_IGNORED,  noControl ,                "from success to selectCmdConfirm ");
		policyData[m_maxControlState * failed                   + selectCmdConfirm ]         = PolicyResult(PR_IGNORED,  noControl ,                "from failed to selectCmdConfirm ");
		policyData[m_maxControlState * comms_failed             + selectCmdConfirm ]         = PolicyResult(PR_IGNORED,  noControl ,                "from comms_failed to selectCmdConfirm ");
		policyData[m_maxControlState * selectCmdWritten         + selectCmdConfirm ]         = PolicyResult(PR_ACCEPTED, selectCmdConfirm ,         "from selectCmdWritten to selectCmdConfirm ");
		policyData[m_maxControlState * selectCmdConfirm         + selectCmdConfirm ]         = PolicyResult(PR_FAILED,   deviceControlException ,   "from selectCmdConfirm to selectCmdConfirm ");
		policyData[m_maxControlState * selectCmdConfirmTimeout  + selectCmdConfirm ]         = PolicyResult(PR_IGNORED,  noControl ,                "from selectCmdConfirmTimeout to selectCmdConfirm ");
		policyData[m_maxControlState * selectCmdCfmNegative     + selectCmdConfirm ]         = PolicyResult(PR_IGNORED,  noControl ,                "from selectCmdCfmNegative to selectCmdConfirm ");
		policyData[m_maxControlState * deviceControlException   + selectCmdConfirm ]         = PolicyResult(PR_IGNORED,  noControl ,                "from deviceControlException to selectCmdConfirm ");
		policyData[m_maxControlState * noControl                + selectCmdConfirmTimeout ]  = PolicyResult(PR_IGNORED,  selectCmdConfirmTimeout ,  "from noControl to selectCmdConfirmTimeout ");
		policyData[m_maxControlState * written                  + selectCmdConfirmTimeout ]  = PolicyResult(PR_IGNORED,  selectCmdConfirmTimeout ,  "from written to selectCmdConfirmTimeout ");
		policyData[m_maxControlState * pending                  + selectCmdConfirmTimeout ]  = PolicyResult(PR_IGNORED,  selectCmdConfirmTimeout ,  "from pending to selectCmdConfirmTimeout ");
		policyData[m_maxControlState * conditionalSuccess       + selectCmdConfirmTimeout ]  = PolicyResult(PR_IGNORED,  selectCmdConfirmTimeout ,  "from conditionalSuccess to selectCmdConfirmTimeout ");
		policyData[m_maxControlState * success                  + selectCmdConfirmTimeout ]  = PolicyResult(PR_IGNORED,  selectCmdConfirmTimeout ,  "from success to selectCmdConfirmTimeout ");
		policyData[m_maxControlState * failed                   + selectCmdConfirmTimeout ]  = PolicyResult(PR_IGNORED,  selectCmdConfirmTimeout ,  "from failed to selectCmdConfirmTimeout ");
		policyData[m_maxControlState * comms_failed             + selectCmdConfirmTimeout ]  = PolicyResult(PR_IGNORED,  selectCmdConfirmTimeout ,  "from comms_failed to selectCmdConfirmTimeout ");
		policyData[m_maxControlState * selectCmdWritten         + selectCmdConfirmTimeout ]  = PolicyResult(PR_ACCEPTED, selectCmdConfirmTimeout ,  "from selectCmdWritten to selectCmdConfirmTimeout ");
		policyData[m_maxControlState * selectCmdConfirm         + selectCmdConfirmTimeout ]  = PolicyResult(PR_IGNORED,  selectCmdConfirmTimeout ,  "from selectCmdConfirm to selectCmdConfirmTimeout ");
		policyData[m_maxControlState * selectCmdConfirmTimeout  + selectCmdConfirmTimeout ]  = PolicyResult(PR_IGNORED,  selectCmdConfirmTimeout ,  "from selectCmdConfirmTimeout to selectCmdConfirmTimeout ");
		policyData[m_maxControlState * selectCmdCfmNegative     + selectCmdConfirmTimeout ]  = PolicyResult(PR_IGNORED,  selectCmdConfirmTimeout ,  "from selectCmdCfmNegative to selectCmdConfirmTimeout ");
		policyData[m_maxControlState * deviceControlException   + selectCmdConfirmTimeout ]  = PolicyResult(PR_IGNORED,  selectCmdConfirmTimeout ,  "from deviceControlException to selectCmdConfirmTimeout ");
		policyData[m_maxControlState * noControl                + selectCmdCfmNegative ]     = PolicyResult(PR_IGNORED,  noControl ,                "from noControl to selectCmdCfmNegative ");
		policyData[m_maxControlState * written                  + selectCmdCfmNegative ]     = PolicyResult(PR_FAILED,   deviceControlException ,   "from written to selectCmdCfmNegative ");
		policyData[m_maxControlState * pending                  + selectCmdCfmNegative ]     = PolicyResult(PR_FAILED,   deviceControlException ,   "from pending to selectCmdCfmNegative ");
		policyData[m_maxControlState * conditionalSuccess       + selectCmdCfmNegative ]     = PolicyResult(PR_IGNORED,  noControl ,                "from conditionalSuccess to selectCmdCfmNegative ");
		policyData[m_maxControlState * success                  + selectCmdCfmNegative ]     = PolicyResult(PR_IGNORED,  noControl ,                "from success to selectCmdCfmNegative ");
		policyData[m_maxControlState * failed                   + selectCmdCfmNegative ]     = PolicyResult(PR_IGNORED,  noControl ,                "from failed to selectCmdCfmNegative ");
		policyData[m_maxControlState * comms_failed             + selectCmdCfmNegative ]     = PolicyResult(PR_IGNORED,  noControl ,                "from comms_failed to selectCmdCfmNegative ");
		policyData[m_maxControlState * selectCmdWritten         + selectCmdCfmNegative ]     = PolicyResult(PR_FAILED,   selectCmdCfmNegative ,     "from selectCmdWritten to selectCmdCfmNegative ");
		policyData[m_maxControlState * selectCmdConfirm         + selectCmdCfmNegative ]     = PolicyResult(PR_FAILED,   deviceControlException ,   "from selectCmdConfirm to selectCmdCfmNegative ");
		policyData[m_maxControlState * selectCmdConfirmTimeout  + selectCmdCfmNegative ]     = PolicyResult(PR_IGNORED,  noControl ,                "from selectCmdConfirmTimeout to selectCmdCfmNegative ");
		policyData[m_maxControlState * selectCmdCfmNegative     + selectCmdCfmNegative ]     = PolicyResult(PR_IGNORED,  noControl ,                "from selectCmdCfmNegative to selectCmdCfmNegative ");
		policyData[m_maxControlState * deviceControlException   + selectCmdCfmNegative ]     = PolicyResult(PR_IGNORED,  noControl ,                "from deviceControlException to selectCmdCfmNegative ");
		policyData[m_maxControlState * noControl                + deviceControlException ]   = PolicyResult(PR_IGNORED,  deviceControlException ,   "from noControl to deviceControlException ");
		policyData[m_maxControlState * written                  + deviceControlException ]   = PolicyResult(PR_FAILED,   deviceControlException ,   "from written to deviceControlException ");
		policyData[m_maxControlState * pending                  + deviceControlException ]   = PolicyResult(PR_FAILED,   deviceControlException ,   "from pending to deviceControlException ");
		policyData[m_maxControlState * conditionalSuccess       + deviceControlException ]   = PolicyResult(PR_IGNORED,  deviceControlException ,   "from conditionalSuccess to deviceControlException ");
		policyData[m_maxControlState * success                  + deviceControlException ]   = PolicyResult(PR_IGNORED,  deviceControlException ,   "from success to deviceControlException ");
		policyData[m_maxControlState * failed                   + deviceControlException ]   = PolicyResult(PR_IGNORED,  deviceControlException ,   "from failed to deviceControlException ");
		policyData[m_maxControlState * comms_failed             + deviceControlException ]   = PolicyResult(PR_IGNORED,  deviceControlException ,   "from comms_failed to deviceControlException ");
		policyData[m_maxControlState * selectCmdWritten         + deviceControlException ]   = PolicyResult(PR_FAILED,   deviceControlException ,   "from selectCmdWritten to deviceControlException ");
		policyData[m_maxControlState * selectCmdConfirm         + deviceControlException ]   = PolicyResult(PR_FAILED,   deviceControlException ,   "from selectCmdConfirm to deviceControlException ");
		policyData[m_maxControlState * selectCmdConfirmTimeout  + deviceControlException ]   = PolicyResult(PR_IGNORED,  deviceControlException ,   "from selectCmdConfirmTimeout to deviceControlException ");
		policyData[m_maxControlState * selectCmdCfmNegative     + deviceControlException ]   = PolicyResult(PR_IGNORED,  deviceControlException ,   "from selectCmdCfmNegative to deviceControlException ");
		policyData[m_maxControlState * deviceControlException   + deviceControlException ]   = PolicyResult(PR_IGNORED,  deviceControlException ,   "from deviceControlException to deviceControlException ");


			m_policyData = policyData;
			
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
				"finished to init WfCommandPolicy, max control command is %d", m_maxControlState - 1);
        }
	}

};
