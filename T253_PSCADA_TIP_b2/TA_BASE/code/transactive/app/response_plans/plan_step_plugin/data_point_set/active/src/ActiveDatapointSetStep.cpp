//#include <ace/String_Base_Const.cpp>
//#include <boost/lexical_cast.hpp>
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/DataConversion.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/threads/src/Thread.h"
#include "core/exceptions/src/ScadaProxyException.h"
#include "core/exceptions/IDL/src/CommonExceptionsCorbaDef.h"
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "core/message/types/MessageTypes.h"
#include "core/message/types/PlanAgentAudit_MessageTypes.h"
#include "core/message/src/NameValuePair.h"
#include "core/message/src/AuditMessageSender.h"
#include "core/utilities/src/RunParams.h"
#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
#include "bus/scada/proxy_library/src/IDataPointProxy.h"
#include "bus/scada/common_library/IDL/src/ScadaCorbaTypes.h"
#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "app/response_plans/plan_step_plugin/data_point_set/active/src/ActiveDatapointSetStep.h"
#include "app/response_plans/plan_step_plugin/common_active/src/IActivePlan.h"
#include "app/response_plans/plan_step_plugin/common_active/src/MessageSender.h"
#include "app/response_plans/common/src/StringConstants.h"
#include <boost/lexical_cast.hpp>


namespace TA_Base_App
{

    const int DP_CONTROL_STATE_DELAY_TIME = 1500; // millisecond

    enum DataPointControlType
    {
        eNoOperation = 0,
        eDataPointExecute,
        eDataPointSelect,
    };

    ActiveDataPointSetStep::ActiveDataPointSetStep(const BasicParameter& detail, IActivePlan& flowControl)
        : ActiveStep(m_params, flowControl),
          m_params(detail)
    {
    }

    ActiveDataPointSetStep::~ActiveDataPointSetStep()
    {
    }

    bool ActiveDataPointSetStep::executeImpl()
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "ActiveDataPointSetStep %lu", id());

        bool bExecSucceed = false;
        int waitMillSec = 0;
        std::string strValue = "";
        std::string strSession = "";

        if(initialize(strSession, strValue, waitMillSec))
        {
            bExecSucceed = sendControl(strSession, strValue, waitMillSec);
        }

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "dataPoint [%s] setValue %s", m_params.dataPoint.c_str(), bExecSucceed ? "successfully" : "unsuccessfully" );
        return bExecSucceed;
    }

    void ActiveDataPointSetStep::processEntityUpdateEvent(unsigned long entityKey , TA_Base_Bus::ScadaEntityUpdateType updateType)
    {
        if(TA_Base_Bus::AgentConnected == updateType)
        {
            m_agentConnectedSem.post();
            return;
        }

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Received entity %lu update %d when %d control", entityKey, updateType, m_nControlType);

        if((TA_Base_Bus::ControlStateUpdate != updateType) || (eNoOperation == m_nControlType))
        {
            // entity update type is not in interest, or not in control state, ignore the update
            return;
        }

        switch(m_pDataPoint->getControlState())
        {
            case TA_Base_Bus::conditionalSuccess:
            case TA_Base_Bus::success:
                {
                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "dataPoint %lu Set response: success", entityKey);

                    m_bRccSucceed = true;

                    m_semaphore.post();

                    std::string strPlanName = m_iPlan.name();
                    const unsigned long ulInstanceNum = m_iPlan.id().instance;
                    submitAuditManualGrcSucceed(strPlanName, ulInstanceNum);

                    break;
                }
            case TA_Base_Bus::LCC_failed:
                {
                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "dataPoint %lu Set response: LCC_failed", entityKey);

                    // Both select and execute accept LCC fail
                    m_semaphore.post();

                    const std::string strEquipDesc = m_pDataPoint->getDescription();
                    const std::string strLcc = m_pDataPoint->getLaunchingConditionResultString();
                    submitAuditManualGrcBadLaunch(strEquipDesc, strLcc);

                    break;
                }
            case TA_Base_Bus::RCC_failed:
                {
                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "dataPoint %lu Set response: RCC_failed", entityKey);

                    // Both select and execute accept RCC fail
                    m_semaphore.post();

                    const std::string strEquipDesc = m_pDataPoint->getDescription();
                    const std::string strRcc = m_pDataPoint->getLaunchingConditionResultString();
                    submitAuditManualGrcBadReturn(strEquipDesc, strRcc);

                    break;
                }
            case TA_Base_Bus::failed:
            case TA_Base_Bus::comms_failed:
                {
                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "dataPoint %lu Set response: failed / comms_failed", entityKey);

                    m_semaphore.post();

                    std::string strPlanName = m_iPlan.name();
                    const unsigned long ulInstanceNum = m_iPlan.id().instance;
                    submitAuditManualGrcFailed("", strPlanName, "", ulInstanceNum, m_params.basic.id);

                    break;
                }
            default:
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Unknown control state for dataPoint %lu ", entityKey);
                break;
        }
    }

    bool ActiveDataPointSetStep::sendControl(const std::string& strSession, const std::string& strValue, const int waitMillSec)
    {
        bool bSuccess = false;

        // Clear all signal of m_semaphore, only wait for m_pDataPoint->setValue response.
        while(m_semaphore.tryWait())
        {
            // Do nothing
        }

        m_bRccSucceed = false;
        m_nControlType = eDataPointExecute;

        long tryCount = 0;

        try
        {
            while(tryCount++ < 100)
            {
                try
                {
                    m_pDataPoint->setValue(strValue, strSession);
                    bSuccess = true;
                    break;
                }
                catch(TA_Base_Core::ScadaProxyException& e)
                {
                    if(tryCount >= 100)
                    {
                        LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::ScadaProxyException", e.what());

                        break;
                    }

                    TA_Base_Core::Thread::sleep(500);
                }
            }
        }
        catch(std::exception& expWhat)
        {
            std::ostringstream msg;
            msg << dumpContext() << ": " << expWhat.what() << " for data point " << m_params.dataPoint << ".";
            LOG_EXCEPTION_CATCH(SourceInfo, typeid(expWhat).name(), msg.str().c_str());
        }
        catch(TA_Base_Core::BadParameterException& e)
        {
            std::ostringstream msg;
            msg << dumpContext() << ": " << e.reason.in() << " for data point " << m_params.dataPoint << ".";
            LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::BadParameterException", msg.str().c_str());
        }
        catch(TA_Base_Core::OperationModeException& e)
        {
            std::ostringstream msg;
            msg << dumpContext() << ": " << e.reason.in() << " for data point " << m_params.dataPoint << ".";
            LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::OperationModeException", msg.str().c_str());
        }
        catch(TA_Base_Bus::ScadaCorbaTypes::NotWriteable& e)
        {
            std::ostringstream msg;
            msg << dumpContext() << ": " << e.reason.in() << " for data point " << m_params.dataPoint << ".";
            LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Bus::ScadaCorbaTypes::NotWriteable", msg.str().c_str());
        }
        catch(TA_Base_Core::NotAvailableException& e)
        {
            std::ostringstream msg;
            msg << dumpContext() << ": " << e.reason.in() << " for data point " << m_params.dataPoint << ".";
            LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::NotAvailableException", msg.str().c_str());
        }
        catch(TA_Base_Core::AccessDeniedException& e)
        {
            std::ostringstream msg;
            msg << dumpContext() << ": " << e.reason.in() << " for data point " << m_params.dataPoint << ".";
            LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::AccessDeniedException", msg.str().c_str());
        }
        catch(CORBA::Exception& e)
        {
            std::ostringstream msg;
            msg << dumpContext() << ": " << TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e) << " for data point " << m_params.dataPoint << ".";
            LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::Exception", msg.str().c_str());
        }
        catch(...)
        {
            std::ostringstream msg;
            msg << dumpContext() << ": " << " for data point " << m_params.dataPoint << ".";
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", msg.str().c_str());
        }

        //only wait when there are RCC checking
        if((0 != waitMillSec) && bSuccess)
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Waiting for Entity Update Event: millsec=%d", waitMillSec);
            if(!m_semaphore.timedWait(waitMillSec + DP_CONTROL_STATE_DELAY_TIME))
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "%s", "Waiting for Entity Update Event: time out");
                bSuccess = false;
            }
            else if(!m_bRccSucceed)
            {
                bSuccess = false;
            }
        }

        m_nControlType = eNoOperation;

        return bSuccess;
    }

    bool ActiveDataPointSetStep::initialize(std::string& strSession, std::string& strValue, int& waitMillSec)
    {
        std::auto_ptr<TA_Base_Core::IEntityData> dpEntity(NULL);
        try
        {
            dpEntity.reset(TA_Base_Core::EntityAccessFactory::getInstance().getEntity(m_params.dataPoint));
        }
        catch(...)
        {
            remark( RemarkInvalidDatapoint );

            std::ostringstream msg;
            msg << dumpContext() << ": " << " for data point " << m_params.dataPoint << ".";
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", msg.str().c_str());

            return false;
        }

        strSession = m_iPlan.session();

        try
        {
            boost::shared_ptr<TA_Base_Core::DataPointEntityData> ptrEntity(dynamic_cast<TA_Base_Core::DataPointEntityData*>(dpEntity.release()));
            waitMillSec = ptrEntity->getABETimeoutPeriodSeconds() * 1000;

            TA_Base_Bus::ScadaProxyFactory::getInstance().createDataPointProxy(ptrEntity, *this, m_pDataPoint);

            if(!m_agentConnectedSem.timedWait(5000))
            {
                remark( RemarkInvalidDatapoint );
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "%s", "dataPoint is not config valid");
                return false;
            }

            strValue = m_params.newValue;
        }
        catch(std::exception& expWhat)
        {
            remark( RemarkInvalidDatapoint );

            std::ostringstream msg;
            msg << dumpContext() << ": " << expWhat.what() << " for data point " << m_params.dataPoint.c_str() << ".";
            LOG_EXCEPTION_CATCH(SourceInfo, typeid(expWhat).name(), msg.str().c_str());

            return false;
        }
        catch(...)
        {
            remark( RemarkInvalidDatapoint );

            std::ostringstream msg;
            msg << dumpContext() << ": " << " for data point " << m_params.dataPoint.c_str() << ".";
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", msg.str().c_str());

            return false;
        }

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                    "dataPoint <%s> setValue: value=%s, sessionId=%s, semaphore timeout = %d",
                    m_params.dataPoint.c_str(), strValue.c_str(), strSession.c_str(), waitMillSec);

        return true;
    }

    void ActiveDataPointSetStep::submitAuditManualGrcBadLaunch(const std::string& strEquipDesc, const std::string& strLcc)
    {
        TA_Base_Core::DescriptionParameters vecParameters;

        TA_Base_Core::NameValuePair pirEquipDescA("EquipmentDescription", strEquipDesc);
        vecParameters.push_back(&pirEquipDescA);
        TA_Base_Core::NameValuePair pirEquipDescB("EquipmentDescription", strEquipDesc);
        vecParameters.push_back(&pirEquipDescB);
        TA_Base_Core::NameValuePair pirLcc("LaunchingConditionString", strLcc);
        vecParameters.push_back(&pirLcc);

        sendAuditMessage("", TA_Base_Core::PlanAgentAudit::PlanManualGRCBadLaunch, vecParameters);
    }

    void ActiveDataPointSetStep::submitAuditManualGrcSucceed(const std::string& strPlanName, const unsigned long ulInstanceNum)
    {
        TA_Base_Core::DescriptionParameters vecParameters;

        TA_Base_Core::NameValuePair pirValue("DataPointValue", "SUCCESSFUL");
        vecParameters.push_back(&pirValue);

        TA_Base_Core::NameValuePair pirPlanName("PlanName", strPlanName);
        vecParameters.push_back(&pirPlanName);

        TA_Base_Core::NameValuePair pirInstanceNum("InstanceNumber", TA_Base_Core::DataConversion::toString(ulInstanceNum));
        vecParameters.push_back(&pirInstanceNum);

        sendAuditMessage("", TA_Base_Core::PlanAgentAudit::PlanManualGRCSuccessful, vecParameters);
    }

    void ActiveDataPointSetStep::submitAuditManualGrcBadReturn(const std::string& strEquipDesc, const std::string& strRcc)
    {
        TA_Base_Core::DescriptionParameters vecParameters;

        TA_Base_Core::NameValuePair pirEquipDesc("EquipmentDescription", strEquipDesc);
        vecParameters.push_back(&pirEquipDesc);
        TA_Base_Core::NameValuePair pirRcc("LaunchingConditionString", strRcc);
        vecParameters.push_back(&pirRcc);

        sendAuditMessage("", TA_Base_Core::PlanAgentAudit::PlanManualGRCBadReturn, vecParameters);
    }

    void ActiveDataPointSetStep::submitAuditManualGrcFailed(const std::string& strSession,
                                                            const std::string& strPlanName,
                                                            const std::string& strReason,
                                                            const unsigned long ulInstanceNum,
                                                            const unsigned long ulStepNum)
    {
        TA_Base_Core::DescriptionParameters vecParameters;

        TA_Base_Core::NameValuePair pirValue("DataPointValue", "UNSUCCESSFUL");
        vecParameters.push_back(&pirValue);

        TA_Base_Core::NameValuePair pirPlanName("PlanName", strPlanName);
        vecParameters.push_back(&pirPlanName);

        TA_Base_Core::NameValuePair pirInstanceNum("InstanceNumber", TA_Base_Core::DataConversion::toString(ulInstanceNum));
        vecParameters.push_back(&pirInstanceNum);

        TA_Base_Core::NameValuePair pirStepNum("StepNumber", TA_Base_Core::DataConversion::toString(ulStepNum));
        vecParameters.push_back(&pirStepNum);

        TA_Base_Core::NameValuePair pirReason("Reason", strReason);
        vecParameters.push_back(&pirReason);

        sendAuditMessage(strSession, TA_Base_Core::PlanAgentAudit::PlanManualGRCUnsuccessful, vecParameters);
    }

    void ActiveDataPointSetStep::sendAuditMessage(const std::string& session,
                                                  const TA_Base_Core::MessageType& auditMessageType,
                                                  const std::vector<TA_Base_Core::NameValuePair*>& description)
    {
		MessageSender::instance().sendAuditMessage(session, auditMessageType, description);
    }
}