
#include "core/utilities/src/DebugUtil.h"
#include "core/corba/src/CorbaUtil.h"
#include "bus/scada/common_library/src/CommonDefs.h"
#include "app/response_plans/common/src/StringConstants.h"
#include "app/response_plans/plan_step_plugin/common_active/src/IActivePlan.h"
#include "app/response_plans/plan_step_plugin/data_point_check/active/src/ActiveDataPointCheckStep.h"

namespace TA_Base_App
{

ActiveDataPointCheckStep::ActiveDataPointCheckStep(const BasicParameter& detail, IActivePlan& flowControl) : ActiveStep(m_params, flowControl),
m_params(detail),
m_nextStepId(0u)
{
}

ActiveDataPointCheckStep::~ActiveDataPointCheckStep()
{
}

bool ActiveDataPointCheckStep::executeImpl()
{
    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Data Point Check step %lu", id());

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

        TA_Base_Bus::ScadaCorbaTypes::UTag* dataPointState;
        CORBA_CALL_RETURN(dataPointState, m_dataPoint, getFieldValue, (m_ulDpKey));

        // Do comparisons.
        switch(dataPointState->_d())
        {
            case TA_Base_Bus::ScadaCorbaTypes::AnalogueTagType:
                //? why didn't deal with enum data type? case TA_Base_Bus::ScadaCorbaTypes::EnumTagType:
                {
                    if(NUMERIC_DATA_POINT != m_params.valueType)
                    {
                        remark( RemarkAttemptCompareDPwithNonfloatValue );
                        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                                    "Attempt to compare float data point with non-float value for data point: %s", m_params.dataPoint.c_str());
                    }
                    else
                    {
                        if(isQualityGood(dataPointState->analogueTag().quality))
                        {
                            std::stringstream ss(m_params.value);
                            double tmp;
                            ss >> tmp;
                            success = test(tmp, dataPointState->analogueTag().value, m_params.testType);
                            bExecSucceed = true;
                        }
                        else
                        {
                            remark( RemarkBadDatapointQuality );
                            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "quality not good for data point: %s", m_params.dataPoint.c_str());
                        }
                    }
                }
                break;

            case TA_Base_Bus::ScadaCorbaTypes::EnumTagType:
                {
                    if(COMBINED_DATA_POINT != m_params.valueType)
                    {
                        remark( RemarkAttemptCompareDPwithNonfloatValue );
                        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                                    "Attempt to compare float data point with non-float value for data point: %s", m_params.dataPoint.c_str());
                    }
                    else
                    {
                        if(isQualityGood(dataPointState->enumTag().quality))
                        {
                            std::stringstream ss(m_params.value);
                            unsigned long tmp;
                            ss >> tmp;

                            success = test(tmp, dataPointState->enumTag().value, m_params.testType);
                            bExecSucceed = true;
                        }
                        else
                        {
                            remark( RemarkBadDatapointQuality );
                            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "quality not good for data point: %s", m_params.dataPoint.c_str());
                        }
                    }
                }
                break;

            case TA_Base_Bus::ScadaCorbaTypes::BooleanTagType:
                {
                    if(BOOLEAN_DATA_POINT != m_params.valueType)
                    {
                        remark( RemarkAttemptCompareDPwithNonBoolValue );
                        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                                    "Attempt to compare boolean data point with non-boolean value for data point: %s", m_params.dataPoint.c_str());
                    }
                    else
                    {
                        if(isQualityGood(dataPointState->booleanTag().quality))
                        {
                            std::stringstream ss(m_params.value);
                            bool tmp;
                            ss >> tmp;
                            success = test(tmp, dataPointState->booleanTag().value, m_params.testType);
                            bExecSucceed = true;
                        }
                        else
                        {
                            remark( RemarkBadDatapointQuality );
                            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "quality not good for data point: %s", m_params.dataPoint.c_str());
                        }
                    }
                }
                break;

            case TA_Base_Bus::ScadaCorbaTypes::TextTagType:
                {
                    if(TEXT_DATA_POINT != m_params.valueType)
                    {
                        remark( RemarkAttemptCompareDPwithNonTextValue );
                        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                                    "Attempt to compare text data point with non-text value for data point: %s", m_params.dataPoint.c_str());
                    }
                    else
                    {
                        if(isQualityGood(dataPointState->textTag().quality))
                        {
                            std::string lhs = m_params.value;
                            TA_Base_Bus::ScadaCorbaTypes::ByteSequence readSeq = dataPointState->textTag().value;
                            std::string rhs = "";
                            int length = readSeq.length();

                            for(int i = 0; i < length; i++)
                            {
                                rhs += static_cast<char>(readSeq[i]);
                            }

                            success = test(lhs, rhs, m_params.testType);
                            bExecSucceed = true;
                        }
                        else
                        {
                            remark( RemarkBadDatapointQuality );
                            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "quality not good for data point: %s", m_params.dataPoint.c_str());
                        }
                    }
                }
                break;

            default:
                {
                    remark( RemarkInvalidDatapointValue );
                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Bad data point value: %s", m_params.dataPoint.c_str());
                }
                break;
        }
    }
    catch(const TA_Base_Core::ObjectResolutionException& e)
    {
        remark( RemarkInvalidDatapoint );
        std::ostringstream msg;
        msg << dumpContext() << ": " << e.what() << " for data point " << m_params.dataPoint << ".";
        LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::ObjectResolutionException", msg.str().c_str());
    }
    catch(const TA_Base_Bus::ScadaCorbaTypes::NotWriteable&)
    {
        remark( RemarkInvalidDatapoint );
        std::ostringstream msg;
        msg << dumpContext() << ": No access to data point " << m_params.dataPoint << ".";
        LOG_EXCEPTION_CATCH(SourceInfo, "nexus::DataPointDmIf::NoAccess", msg.str().c_str());
    }
    catch(...)
    {
        remark( RemarkInvalidDatapoint );
        std::ostringstream msg;
        msg << dumpContext() << ": No access to data point " << m_params.dataPoint << ".";
        LOG_EXCEPTION_CATCH(SourceInfo, "Unknown exception", msg.str().c_str());
    }

	if (bExecSucceed)
	{
		m_nextStepId = success ? m_params.successStep : m_params.failureStep;
	}		

    return bExecSucceed;
}

std::list<ActiveStep*> ActiveDataPointCheckStep::getNextStep()
{
	std::list<ActiveStep*> outgoingStepList;

	for( std::list<ActiveStep*>::iterator itLoop = m_lstOutgoingStep.begin(); itLoop != m_lstOutgoingStep.end(); ++itLoop )
	{
		if( m_nextStepId == (*itLoop)->id() )
		{
			outgoingStepList.push_back( *itLoop );
			break;
		}
	}

	return outgoingStepList;
}


template<class T>
bool ActiveDataPointCheckStep::test(const T& lhs, const T& rhs, const int testType)
{
    switch(testType)
    {
        case EQUAL_TO_TEST:
            return lhs == rhs;

        case NOT_EQUAL_TO_TEST:
            return lhs != rhs;

        case LESS_THAN_TEST:
            return lhs < rhs;

        case GREATER_THAN_TEST:
            return lhs > rhs;

        case LESS_THAN_OR_EQUAL_TO_TEST:
            return lhs <= rhs;

        case GREATER_THAN_OR_EQUAL_TO_TEST:
            return lhs >= rhs;

        default:
            return false;
    }
}

bool ActiveDataPointCheckStep::isQualityGood(const int quality)
{
    return ( TA_Base_Bus::ScadaCorbaTypes::QualityGoodNoSpecificReason == quality
            || TA_Base_Bus::ScadaCorbaTypes::QualityGoodLocalOverride == quality );
}

}