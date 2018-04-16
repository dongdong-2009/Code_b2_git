/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/ApprovedPlanTreeView.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2015/01/08 17:33:12 $
  * Last modified by:  $Author: qi.huang $
  *
  * <description>
  *
  **/

#include "StdAfx.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/exceptions/src/TransactiveException.h"
// CL 15087 (2) ++
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/src/AuditMessageSender.h"
#include "core/message/types/AlarmAudit_MessageTypes.h"
// ++ CL 15087 (2) 
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "bus/response_plans/active_plans_display/src/PlanAgentLocationAccess.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/MessageBox.h"
#include "Resource.h"
#include "PlanNode.h"
#include "ControlPlanThread.h"
#include "PlanControllerThread.h"
#include "FloatingWindowThreadRegister.h"


namespace TA_Base_App
{


/////////////////////////////////////////////////////////////////////////////
// ControlPlanThread

ControlPlanThread::ControlPlanThread(PlanNode* pNode, unsigned long location, unsigned long instanceId /*= -1*/, const std::string& planName /*= ""*/, bool exec/*=false*/) :
m_pPlanNode(pNode),
m_planName(planName),
m_Location(location),
m_InstanceId(instanceId),
m_bStop(true),
m_execPlan(exec)
{
}

ControlPlanThread::~ControlPlanThread()
{

}

void ControlPlanThread::terminate()
{
    m_bStop = true;
}


// CL16116++
void TA_Base_App::ControlPlanThread::run()
{
	// CL 15087 (2) ++
	std::auto_ptr<TA_Base_Core::AuditMessageSender> messageSender(TA_Base_Core::MessagePublicationManager::getInstance().getAuditMessageSender(
		TA_Base_Core::AlarmAudit::Context));
	// get entity key
	std::auto_ptr<TA_Base_Core::IEntityData> planManagerEntityData(TA_Base_Core::EntityAccessFactory::getInstance().getEntity("PlanManager"));
	unsigned long entityKey = planManagerEntityData->getKey();
	// get session
	std::string session = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
	TA_Base_Core::DescriptionParameters dp;
	std::ostringstream strOut;
	strOut << "DSS Run for " << m_planName;
	TA_Base_Core::NameValuePair alarmDesc("alarmDescription", strOut.str().c_str());
	// ++ CL 15087 (2) 

	m_bStop = false;
	if(m_pPlanNode)
	{
		try
		{
			// We will start a new instance for --exec-plan-id
			// If no plan instance specialized and we call PlanNode.controlPlan(PlanInstanceId)
			// it will retrieve plan instances from all plan agent for this node, it is very slow>.
			// start a new instance do not need this at all, unless instance was assigned
			if(m_InstanceId != -1)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
					"[ControlPlanThread]Call controlPlan() with plan instance:%u", 
					m_InstanceId);
				m_pPlanNode->controlPlan(PlanInstanceId(m_InstanceId, m_Location));
			}
			else
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
					"[ControlPlanThread]Call controlPlan() without parameter");
				m_pPlanNode->controlPlan();
			}

			// get control plan dialog thread
			CPlanControllerThread* pControlThreadDlg = FloatingWindowThreadRegister::getInstance().getPlanControllerThread(m_pPlanNode->getNodeId());
			if (!pControlThreadDlg)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
					"[ControlPlanThread] Can not get controlPlan Dialog Thread for PlanNode:%s", 
					m_pPlanNode->getNodeName().c_str());				
                throw TA_Base_Core::TransactiveException("");
			}

            if (m_execPlan)
            {
				// send start new instance message for auto run a plan
				pControlThreadDlg->GetMainWnd()->PostMessage(WM_COMMAND, ID_PLAN_EXECUTE);

				// send audit message for DSS Run success
				// CL 15087 (2) ++
				TA_Base_Core::NameValuePair alarmValue("Value", "SUCCESSFUL");	
				dp.clear();
				dp.push_back(&alarmValue);
				dp.push_back(&alarmDesc);	
				// DSS Run for Successful message (without asset)
				messageSender->sendAuditMessage(TA_Base_Core::AlarmAudit::AlarmPlanStateUpdated,
					entityKey, dp, "", session, "", "", "");	
				// ++ CL 15087 (2)
            }
		}
		catch(...)
		{
			TA_Base_Bus::TransActiveMessage userMsg;
			userMsg << m_pPlanNode->getNodeName();
			userMsg.showMsgBox(IDS_UE_210045);

            if (m_execPlan)
            {
			    // send audit message for DSS Run Failed
			    // CL 15087 (2) ++
			    TA_Base_Core::NameValuePair alarmValue("Value", "FAIL");
			    dp.clear();
			    dp.push_back(&alarmValue);
			    dp.push_back(&alarmDesc);	
			    // DSS Run for Successful message (without asset)
			    messageSender->sendAuditMessage(TA_Base_Core::AlarmAudit::AlarmPlanStateUpdated,
				    entityKey, dp, "", session, "", "", "");	
			    // ++ CL 15087 (2)
            }
		}
	}
}

}