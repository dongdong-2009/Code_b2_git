#include <time.h>
#include "ActiveDataPointTimeCheckStep.h"
#include "app/response_plans/plan_step_plugin/common_active/src/IActivePlan.h"
#include "app/response_plans/common/src/StringConstants.h"
//#include "bus/response_plans/plan_manager/IDL/src/IPlanManagerCorbaDef.h"
#include "bus/scada/common_library/src/CommonDefs.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/message/src/CommsMessageSender.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/types/PlanAgentComms_MessageTypes.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"


namespace TA_Base_App
{

    const static unsigned long MAX_TOLERANT_TIME_MISMATCH = 5u;

    ActiveDataPointTimeCheckStep::ActiveDataPointTimeCheckStep(const BasicParameter& detail, IActivePlan& flowControl)
        : ActiveStep(m_params, flowControl),
          m_params(detail),
		  m_nextStepId(0)
    {
    }

    ActiveDataPointTimeCheckStep::~ActiveDataPointTimeCheckStep()
    {
    }

    bool ActiveDataPointTimeCheckStep::executeImpl()
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Data Point Check time step %lu", id());

        bool bExecSucceed = false;
        bool success = false;

        try
        {
            // get named object of datapoint agent
            if(m_dataPoint.getObjectName().empty())
            {
                std::auto_ptr<TA_Base_Core::IEntityData> dpEntity(TA_Base_Core::EntityAccessFactory::getInstance().getEntity(m_params.dataPoint));
                std::string dpAgentName = dpEntity->getAgentName();
                m_ulDpKey = dpEntity->getKey();
                m_dataPoint.setCorbaName(dpAgentName, dpAgentName + TA_Base_Bus::DP_ACCESS_INTERFACE);
            }

            TA_Base_Bus::ScadaCorbaTypes::UTag_var dataPointState;
            CORBA_CALL_RETURN(dataPointState, m_dataPoint, getFieldValue, (m_ulDpKey));

            // Do comparisons.
            unsigned long ulDpTime = 0u;
            int eQuality = 0;

            switch(dataPointState->_d())
            {
                case TA_Base_Bus::ScadaCorbaTypes::AnalogueTagType:
                    ulDpTime = dataPointState->analogueTag().timeStamp.seconds;
                    eQuality = dataPointState->analogueTag().quality;
                    break;
                case TA_Base_Bus::ScadaCorbaTypes::BooleanTagType:
                    ulDpTime = dataPointState->booleanTag().timeStamp.seconds;
                    eQuality = dataPointState->booleanTag().quality;
                    break;
                case TA_Base_Bus::ScadaCorbaTypes::EnumTagType:
                    ulDpTime = dataPointState->enumTag().timeStamp.seconds;
                    eQuality = dataPointState->enumTag().quality;
                    break;
                case TA_Base_Bus::ScadaCorbaTypes::TextTagType:
                    ulDpTime = dataPointState->textTag().timeStamp.seconds;
                    eQuality = dataPointState->textTag().quality;
                    break;
                default:
                    {
                        m_remark = RemarkInvalidDatapointValue;
                        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Bad data point value: %s", m_params.dataPoint.c_str());
                        break;
                    }
            }

            if(!m_bLastQualityGood)
            {
                m_bLastQualityGood = ((TA_Base_Bus::ScadaCorbaTypes::QualityGoodNoSpecificReason == eQuality) ||
                                      (TA_Base_Bus::ScadaCorbaTypes::QualityGoodLocalOverride == eQuality));
            }

            unsigned long ulNow = time(NULL);

            if(ulNow < ulDpTime)
            {
                if((ulDpTime - ulNow) > MAX_TOLERANT_TIME_MISMATCH)
                {
                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                                "Time is mismatch too much between equipment [%d] and server [%d]", ulDpTime, ulNow);

                    m_remark = RemarkDataPointTimeLaterThanNow;
                }
                else
                {
                    // success = false in default
                    bExecSucceed = true;
                }
            }
            else
            {
                success = ((ulNow - ulDpTime) >= m_params.timeSpan) && m_bLastQualityGood;
                bExecSucceed = true;
            }
        }
        catch(const TA_Base_Core::ObjectResolutionException& e)
        {
            std::ostringstream msg;
            msg << dumpContext() << ": " << e.what() << " for data point " << m_params.dataPoint << ".";
            m_remark = RemarkInvalidDatapoint;

            LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::ObjectResolutionException", msg.str().c_str());
        }
        catch(const std::exception& e)
        {
            std::ostringstream msg;
            msg << dumpContext() << ": " << e.what() << " for data point " << m_params.dataPoint << ".";
            m_remark = RemarkInvalidDatapoint;

            LOG_EXCEPTION_CATCH(SourceInfo, typeid(e).name(), msg.str().c_str());
        }
        catch(const TA_Base_Bus::ScadaCorbaTypes::NotWriteable&)
        {
            std::ostringstream msg;
            msg << dumpContext() << ": No access to data point " << m_params.dataPoint << ".";
            m_remark = RemarkInvalidDatapoint;

            LOG_EXCEPTION_CATCH(SourceInfo,  "nexus::DataPointDmIf::NoAccess", msg.str().c_str());
        }
        catch(...)
        {
            std::ostringstream msg;
            msg << dumpContext() << ": Unknown exception caught: " << m_params.dataPoint << ".";
            m_remark = RemarkInvalidDatapoint;

            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown exception", msg.str().c_str());
        }

		if (bExecSucceed)
		{
			m_nextStepId = success ? m_params.successStep : m_params.failureStep;
		}

		return bExecSucceed;
	}

	std::list<ActiveStep*> ActiveDataPointTimeCheckStep::getNextStep()
	{
		std::list<ActiveStep*> outgoingStepList;
		std::list<ActiveStep*>::iterator itActiveStep = m_lstOutgoingStep.begin();
		for(; itActiveStep != m_lstOutgoingStep.end(); ++itActiveStep)
		{
			if(m_nextStepId == (*itActiveStep)->id())
			{
				outgoingStepList.push_back(*itActiveStep);
				break;
			}
		}
		return outgoingStepList;
	}

}