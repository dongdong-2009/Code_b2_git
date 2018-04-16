/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_agent/src/ActiveDataPointSetBlinkStep.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #9 $
  *
  *  Last modification: $DateTime: 2014/10/07 14:07:45 $
  *  Last modified by:  $Author: haijun.li $
  *
  *  Implementation of .
  */

#include "app/response_plans/common/src/ProjectDefinition.h"

#if defined ( STEPTYPE_DATA_POINT_SET_BLINK )

#include "core/utilities/src/DebugUtil.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/threads/src/Thread.h"
#include "core/exceptions/src/ScadaProxyException.h"
#include "core/exceptions/IDL/src/CommonExceptionsCorbaDef.h"
#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
#include "bus/scada/proxy_library/src/IDataPointProxy.h"
#include "bus/scada/common_library/IDL/src/ScadaCorbaTypes.h"
#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "app/response_plans/plan_agent/src/AAMessageSender.h"
#include "app/response_plans/plan_agent/src/IActivePlanFlow.h"
#include "app/response_plans/plan_agent/src/ActivePlanBulletinBoard.h"
#include "app/response_plans/plan_agent/src/ActiveDataPointSetBlinkStep.h"
#include "app/response_plans/common/src/StringConstants.h"
//#include "app/response_plans/plan_agent/src/PlanAccessFactory.h"

namespace TA_Base_App
{
    const int DP_CONTROL_STATE_DELAY_TIME = 1500; // millisecond

    enum DataPointControlType
    {
        eNoOperation = 0,
        eDataPointExecute,
        eDataPointSelect,
    };

    ActiveDataPointSetBlinkStep::ActiveDataPointSetBlinkStep( const TA_Base_Core::StepDetail& stepDetail,
        TA_Base_Core::Steps_var& varAllSteps, IActivePlanFlow& flowControl) :
	ActiveStep(stepDetail, flowControl),
	m_agentConnectedSem(0),
    m_pDataPoint()
	{
		m_dataPointParameters.clear();
		m_pDataPoint.reset();

		TA_Base_Core::StepNumber stepIndex = 1;
		for( stepIndex = 1; stepIndex <= varAllSteps->length(); stepIndex++ )
		{
            switch( varAllSteps[stepIndex-1].parameters._d() )
			{
				case TA_Base_Core::DATA_POINT_SET_STEP:
				{
					#if defined ( STEPTYPE_DATA_POINT_SET )
					m_dataPointParameters.push_back(varAllSteps[stepIndex-1].parameters.dataPointSet());
					LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Get set blink datapoint %s", varAllSteps[stepIndex-1].parameters.dataPointSet().dataPoint.in());
					#endif
                }
				break;

				default:
				break; 				
            }		
		}
		
	}

    ActiveDataPointSetBlinkStep::~ActiveDataPointSetBlinkStep()
    {
         m_pDataPoint.reset();
    }

	void ActiveDataPointSetBlinkStep::processEntityUpdateEvent(unsigned long entityKey , TA_Base_Bus::ScadaEntityUpdateType updateType)
	{
		if( TA_Base_Bus::AgentConnected == updateType && entityKey == m_dataPointKey)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Received entity %lu update %d", entityKey, updateType );
			m_agentConnectedSem.post();
            return;
		}
    }

    bool ActiveDataPointSetBlinkStep::initialize(std::string &strValue, unsigned int nIndex)
    {
        m_remark = "";

        std::auto_ptr<TA_Base_Core::IEntityData> dpEntity(NULL);
        try
        {
            dpEntity.reset(TA_Base_Core::EntityAccessFactory::getInstance().getEntity(m_dataPointParameters[nIndex].dataPoint.in()));
        }
        catch (...) 
        {
            m_remark = RemarkInvalidDatapoint;

            std::ostringstream msg;
            msg << dumpContext() << ": " << " for data point " << m_dataPointParameters[nIndex].dataPoint.in() << ".";
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", msg.str().c_str() );

            return false;
        }


        std::string reason = "";
		m_dataPointKey = dpEntity->getKey();
        if ( !ActivePlanBulletinBoard::instance()->isActionPermitted(m_strSession, m_dataPointKey, TA_Base_Bus::aca_DATA_POINT_WRITE, reason) )
        {
            m_remark = RemarkAccessDataPointDenied;
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "%s Write Access to Data Point %s not allowed.", dumpContext().c_str(), m_dataPointParameters[nIndex].dataPoint.in() );
            return false;
        }

        try
        {
            boost::shared_ptr<TA_Base_Core::DataPointEntityData> ptrEntity( dynamic_cast<TA_Base_Core::DataPointEntityData*>(dpEntity.release()) );
			m_pDataPoint.reset();
            TA_Base_Bus::ScadaProxyFactory::getInstance().createDataPointProxy( ptrEntity,*this, m_pDataPoint );

            if(!m_agentConnectedSem.timedWait(5000))
            {
                m_remark = RemarkInvalidDatapoint;
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "%s", "DataPoint is not config valid");
                return false;
            }

            if(m_pDataPoint->isDerived())
            {
                strValue = m_pDataPoint->getDerivedStateAsString( static_cast<unsigned long>(m_dataPointParameters[nIndex].newValue.numericValue().value) );
            }
            else if(m_pDataPoint->isText())
            {
                strValue = m_dataPointParameters[nIndex].newValue.textValue();
            }
            else if(m_pDataPoint->isNumber())
            {
				std::stringstream ss;
				ss << m_dataPointParameters[nIndex].newValue.numericValue().value;
                strValue = ss.str();
            }
            else if(m_pDataPoint->isBoolean())
            {
                strValue = m_pDataPoint->getBooleanLabel(m_dataPointParameters[nIndex].newValue.booleanValue());
            }
            else
            {
                m_remark = RemarkInvalidDatapoint;
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s", "Unknow DataPoint Type");
                return false;
            }
        }
        catch( std::exception& expWhat )
        {
            m_remark = RemarkInvalidDatapoint;

            std::ostringstream msg;
            msg << dumpContext() << ": " << expWhat.what() << " for data point " << m_dataPointParameters[nIndex].dataPoint.in() << ".";
            LOG_EXCEPTION_CATCH( SourceInfo, typeid(expWhat).name(), msg.str().c_str() );

            return false;
        }
        catch (...) 
        {
            m_remark = RemarkInvalidDatapoint;

            std::ostringstream msg;
            msg << dumpContext() << ": " << " for data point " << m_dataPointParameters[nIndex].dataPoint.in() << ".";
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", msg.str().c_str() );

            return false;
        }

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
            "DataPoint <%s> setValue: value=%s, sessionId=%s",
            m_dataPointParameters[nIndex].dataPoint.in(), strValue.c_str(), m_strSession.c_str());

        return true;
    }

	bool ActiveDataPointSetBlinkStep::sendPScadaBlink(const std::string &strValue, unsigned int nIndex)
	{
        bool bSuccess = false;
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
            "Enter executePScadaBlink session: %s, value: %s ", m_strSession.c_str(), strValue.c_str() );

       long tryCount = 0;

        try
        {
            while( tryCount++ < 100 )
            {
                try
                {
                    m_pDataPoint->setControlValue(strValue, m_strSession, TA_Base_Bus::CONTROL_BLINK);
                    bSuccess = true;
                    break;
                }
                catch (TA_Base_Core::ScadaProxyException &e)
                {
                    if ( tryCount >= 100 )
                    {
                        LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::ScadaProxyException", e.what() );

                        break;
                    }

                    TA_Base_Core::Thread::sleep( 500 );
                }
            }
        }
        catch( std::exception& expWhat )
        {
            std::ostringstream msg;
            msg << dumpContext() << ": " << expWhat.what() << " for data point " << m_dataPointParameters[nIndex].dataPoint.in() << ".";
            LOG_EXCEPTION_CATCH( SourceInfo, typeid(expWhat).name(), msg.str().c_str() );
        }
        catch( TA_Base_Core::BadParameterException& e)
        {
            std::ostringstream msg;
            msg << dumpContext() << ": " << e.reason.in() << " for data point " << m_dataPointParameters[nIndex].dataPoint.in() << ".";
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::BadParameterException", msg.str().c_str() );
        }
        catch( TA_Base_Core::OperationModeException& e)
        {
            std::ostringstream msg;
            msg << dumpContext() << ": " << e.reason.in() << " for data point " << m_dataPointParameters[nIndex].dataPoint.in() << ".";
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::OperationModeException", msg.str().c_str() );
        }
        catch( TA_Base_Bus::ScadaCorbaTypes::NotWriteable& e)
        {
            std::ostringstream msg;
            msg << dumpContext() << ": " << e.reason.in() << " for data point " << m_dataPointParameters[nIndex].dataPoint.in() << ".";
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::ScadaCorbaTypes::NotWriteable", msg.str().c_str() );
        }
        catch( TA_Base_Core::NotAvailableException& e)
        {
            std::ostringstream msg;
            msg << dumpContext() << ": " << e.reason.in() << " for data point " << m_dataPointParameters[nIndex].dataPoint.in() << ".";
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::NotAvailableException", msg.str().c_str() );
        }
        catch( TA_Base_Core::AccessDeniedException& e)
        {
            std::ostringstream msg;
            msg << dumpContext() << ": " << e.reason.in() << " for data point " << m_dataPointParameters[nIndex].dataPoint.in() << ".";
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::AccessDeniedException", msg.str().c_str() );
        }
        catch( CORBA::Exception& e )
        {
            std::ostringstream msg;
            msg << dumpContext() << ": " << TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e) << " for data point " << m_dataPointParameters[nIndex].dataPoint.in() << ".";
            LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", msg.str().c_str() );
        }
        catch( ... )
        {
            std::ostringstream msg;
            msg << dumpContext() << ": " << " for data point " << m_dataPointParameters[nIndex].dataPoint.in() << ".";
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", msg.str().c_str() );
        }

		return bSuccess;
    }

	TA_Base_Core::EActiveStepState ActiveDataPointSetBlinkStep::executeImpl( void )
    {
        m_strSession = ActivePlanBulletinBoard::instance()->getSession(m_flowControl.getPlan());
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "ActiveDataPointSetBlinkStep %lu, session:%s", getStepNumber(), m_strSession.c_str());
		

        bool bAllSuccess = true;
        std::string strValue = "";
		unsigned int nIndex = 0;
		for( nIndex = 0; nIndex < m_dataPointParameters.size(); nIndex++ )
		{
			bool bSuccess = false;
	        if ( initialize(strValue, nIndex ))
	        {
			    bSuccess = sendPScadaBlink(strValue, nIndex);

				if (bSuccess)
				{
					LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "DataPoint [%s] setBlink successfully", m_dataPointParameters[nIndex].dataPoint.in() );
				}
				else
				{
					bAllSuccess = false;
					LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "DataPoint [%s] setBlink failed", m_dataPointParameters[nIndex].dataPoint.in() );
					break;
				}
			}

		}

        if( bAllSuccess )
        {
            m_remark = RemarkStepExecutionSucceed;
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "All of the PSCADA control datapoint setBlink successfully");
            m_flowControl.setNextStep();	        
		}		
        return ( bAllSuccess ? TA_Base_Core::EXECUTED_ACTIVE_STEP_STATE : TA_Base_Core::FAILED_ACTIVE_STEP_STATE );
	}

} // TA_Base_App


#endif //  STEPTYPE_DATA_POINT_SET_BLINK

