/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_Review_Branch/TA_BASE/transactive/app/response_plans/plan_agent/src/ActiveDataPointSetStep.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #8 $
  *
  *  Last modification: $DateTime: 2011/06/20 20:46:25 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  Implementation of .
  */

#include "PlanAgent.h"
#if defined ( STEPTYPE_DATA_POINT_SET )


// namespace alias for Corba structures/types

namespace TA_Base_App
{
    const int DP_CONTROL_STATE_DELAY_TIME = 1500; // millisecond

    ActiveDataPointSetStep::ActiveDataPointSetStep( const TA_Base_Core::StepDetail& stepDetail,
													const TA_Base_Core::DataPointSetParameters& params,
													IActivePlanFlow& flowControl,
													bool isGRCType3Step) :
		ActiveStep(stepDetail, flowControl),
		m_params(params),
		m_flowControl(flowControl),
		m_semaphore(0),
		m_cfgvalidsem(0),
		m_isGRCType3Step(isGRCType3Step)
	{
			if ( m_isGRCType3Step )
			{
				m_GRCType3StepType = GRCTYPE3_CHILD_STEP;
			}
	}


	void ActiveDataPointSetStep::processEntityUpdateEvent(unsigned long entityKey , TA_Base_Bus::ScadaEntityUpdateType updateType)
	{
		if(updateType == TA_Base_Bus::ConfigAvailable)
		{
			m_cfgvalidsem.post();
		}
		if(updateType == TA_Base_Bus::ControlStateUpdate)
		{
			TA_Base_Bus::EDataPointControlState dst = m_pDataPoint->getControlState();

			if(TA_Base_Bus::conditionalSuccess == dst || TA_Base_Bus::success == dst)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Datapoint Set response: success");
				// TD20339 Yanrong++
				m_setValueSuccess = true;
				// ++Yanrong
				
				TA_Base_Core::DescriptionParameters descriptionParameters;

				//CL-16389++
				TA_Base_Core::NameValuePair pair1( "PlanName", TA_Base_Bus::PlanDataReader::instance()->getNodePath(m_flowControl.getPlan().plan ));
				descriptionParameters.push_back ( &pair1 );
				
				std::ostringstream tempStrm;
				tempStrm << m_flowControl.getPlan().instance;
				TA_Base_Core::NameValuePair pair2( "InstanceNumber", tempStrm.str());
				descriptionParameters.push_back ( &pair2 );
				//++CL-16389
				AAMessageSender::instance()->sendAuditMessage("", TA_Base_Core::PlanAgentAudit::PlanManualGRCSuccessful, descriptionParameters);
				m_semaphore.post();
			}
			else if(TA_Base_Bus::LCC_failed == dst)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Datapoint Set response: LCC_failed");
				TA_Base_Core::DescriptionParameters descriptionParameters;
				TA_Base_Core::NameValuePair pair0( "EquipmentDescription", m_pDataPoint->getDescription());
				descriptionParameters.push_back ( &pair0 );
				TA_Base_Core::NameValuePair pair1( "EquipmentDescription", m_pDataPoint->getDescription());
				descriptionParameters.push_back ( &pair1 );
				TA_Base_Core::NameValuePair pair2( "LaunchingConditionString", m_pDataPoint->getLaunchingConditionResultString());
				descriptionParameters.push_back ( &pair2 );	 
				AAMessageSender::instance()->sendAuditMessage("", TA_Base_Core::PlanAgentAudit::PlanManualGRCBadLaunch, descriptionParameters);
				m_semaphore.post();
			}
			else if(TA_Base_Bus::RCC_failed == dst)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Datapoint Set response: RCC_failed");
				TA_Base_Core::DescriptionParameters descriptionParameters;
				TA_Base_Core::NameValuePair pair0( "EquipmentDescription", m_pDataPoint->getDescription());
				descriptionParameters.push_back ( &pair0 );
				TA_Base_Core::NameValuePair pair1( "LaunchingConditionString", m_pDataPoint->getLaunchingConditionResultString());
				descriptionParameters.push_back ( &pair1 );	 
				AAMessageSender::instance()->sendAuditMessage("", TA_Base_Core::PlanAgentAudit::PlanManualGRCBadReturn, descriptionParameters);
				m_semaphore.post();
			}
			else if(TA_Base_Bus::failed == dst || TA_Base_Bus::comms_failed == dst)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Datapoint Set response: failed / comms_failed");
				TA_Base_Core::DescriptionParameters descriptionParameters;


				//CL-16389++
				TA_Base_Core::NameValuePair pair1( "PlanName", TA_Base_Bus::PlanDataReader::instance()->getNodePath(m_flowControl.getPlan().plan ));
				descriptionParameters.push_back ( &pair1 );
				
				std::ostringstream tempStrm;
				tempStrm << m_flowControl.getPlan().instance;
				TA_Base_Core::NameValuePair pair2( "InstanceNumber", tempStrm.str());
				descriptionParameters.push_back ( &pair2 );

				//++CL-16389

				AAMessageSender::instance()->sendAuditMessage("", TA_Base_Core::PlanAgentAudit::PlanManualGRCUnsuccessful, descriptionParameters);
				m_semaphore.post();
			}

		}
	}


	TA_Base_Core::EActiveStepState ActiveDataPointSetStep::executeImpl( void )
	{
		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "Data Point Set step %lu", getStepNumber() );

		TA_Base_Core::EActiveStepState stepState = TA_Base_Core::EXECUTED_ACTIVE_STEP_STATE;

		m_remark = "Step executed.";

		std::string message;

//		TA_Base_Core::IEntityData* entity = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(m_params.dataPoint.in());

//		unsigned long dpKey = entity->getKey();

		int millsec = -1;
        TA_Base_Core::IEntityData* dpEntity;
        try
        {
            dpEntity = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(m_params.dataPoint.in());
        }
        catch (...) 
        {
            std::ostringstream msg;
            msg << dumpContext() << ": "
                << " for data point "
                << m_params.dataPoint.in() << ".";
            
            m_remark = "Creating datapoint entity failed.";
            LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "Unknown Exception", msg.str().c_str() );
            stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;

            return stepState;
        }

		// zn++ CL-21150: move this code block down to get entity key
		//[[CL-21150 wuzhongyi, recover duty check , comment it on TD2771
		unsigned long action = TA_Base_Bus::aca_DATA_POINT_WRITE;
		std::string session = ActivePlanBulletinBoard::instance()->getSession(m_flowControl.getPlan());
		std::string reason;
		
		// check concrete resource instead of use PlanAgent as a resource
		//if ( !ActivePlanBulletinBoard::instance()->isActionPermitted(session, action, reason) )
		if ( !ActivePlanBulletinBoard::instance()->isActionPermitted(session, dpEntity->getKey(), action, reason) )
		{
			m_remark = "Write Access to Data Point write denied";
			message = "Write Access to Data Point ";
			message += m_params.dataPoint.in();
			message += " not allowed.";
			LOG( SOURCE_INFO, GENERIC_LOG_TYPE, WARN_LOG_LEVEL, message.c_str() );
			stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
			return stepState;
		}
		//]]
		//++zn

		try
		{	
			millsec = (dynamic_cast< TA_Base_Core::DataPointEntityData*>(dpEntity))->getABETimeoutPeriodSeconds() * 1000;
			TA_Base_Bus::ScadaProxyFactory*	m_proxyFactory = &(TA_Base_Bus::ScadaProxyFactory::getInstance());
			TA_ASSERT ( ( 0 != m_proxyFactory ), "SCADA Proxy Factory not created");
			m_proxyFactory->setProxiesToControlMode();
			
			m_proxyFactory->createDataPointProxy(
			boost::shared_ptr<TA_Base_Core::DataPointEntityData>(dynamic_cast< TA_Base_Core::DataPointEntityData*>(dpEntity)), 
				*this, m_pDataPoint);
			if(!m_cfgvalidsem.timedWait(5000))
			{
				LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "DataPoint is not config valid");
				return TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
			}

			std::string tmp;
			std::stringstream str;
			if(m_pDataPoint->isDerived())
			{
				tmp = m_pDataPoint->getDerivedStateAsString( static_cast<unsigned long>(m_params.newValue.numericValue().value) );
			}
			else if(m_pDataPoint->isText())
			{
				tmp = m_params.newValue.textValue();
			}
			else if(m_pDataPoint->isNumber())
			{
				str << m_params.newValue.numericValue().value;
				tmp = str.str();
			}
			else if(m_pDataPoint->isBoolean())
			{
				
				tmp = m_pDataPoint->getBooleanLabel(m_params.newValue.booleanValue());
			}
			else
			{
				LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Unknow DataPoint Type");
				return TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
			}

			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Datapoin <%s> setValue: value=%s, sessionId=%s, semaphore timeout = %d",
				m_params.dataPoint.in(), tmp.c_str(), session.c_str(), millsec );

			m_setValueSuccess = false;

            // Clear all signal of m_semaphore, only wait for m_pDataPoint->setValue response.
            while ( m_semaphore.tryWait() )
            {
                // Do nothing
            }
			// ++Yanrong

            // CL-20597 yingliang ++
            long tryCount = 0;
            while( tryCount++ < 100 )
            {
                try
                {
                    m_pDataPoint->setValue(tmp, session);
                    break;
                }
                catch (TA_Base_Core::ScadaProxyException &e)
                {
                    if ( tryCount >= 100 )
                    {
                        LOG_EXCEPTION_CATCH( SOURCE_INFO, "TA_Base_Core::ScadaProxyException", e.what() );
                        stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
                        return stepState;
                    }
                    TA_Base_Core::Thread::sleep( 500 );
                }
            }
            // ++yingliang

			// TD20339 Yanrong ++, we shouldn't setNextStep before receiving the result of entity event update.
			//m_flowControl.setNextStep();
			// ++Yanrong
		}
		catch(TA_Base_Core::ScadaProxyException& e)
		{
			std::ostringstream msg;
			msg << dumpContext() << ": "
			    << e.what() << " for data point "
			    << m_params.dataPoint.in() << ".";
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "TA_Base_Core::ScadaProxyException", msg.str().c_str() );
			stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
			return stepState;
		}
		// TD20339 Yanrong++
		catch( TA_Base_Core::TransactiveException& e )
		{
			std::ostringstream msg;
			msg << dumpContext() << ": "
			    << e.what() << " for data point "
			    << m_params.dataPoint.in() << ".";
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "TA_Base_Core::TransactiveException", msg.str().c_str() );
			stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
			return stepState;
		}
		catch( std::exception& )
		{
			std::ostringstream msg;
			msg << dumpContext() << ": "
			    << " for data point "
			    << m_params.dataPoint.in() << ".";
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "std::exception", msg.str().c_str() );
			stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
			return stepState;
		}
		catch( TA_Base_Core::BadParameterException& e)
		{
			std::ostringstream msg;
			msg << dumpContext() << ": "
			    << e.reason.in() << " for data point "
			    << m_params.dataPoint.in() << ".";
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "TA_Base_Core::BadParameterException", msg.str().c_str() );
			stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
			return stepState;
		}
		catch( TA_Base_Core::OperationModeException& e)
		{
			std::ostringstream msg;
			msg << dumpContext() << ": "
			    << e.reason.in() << " for data point "
			    << m_params.dataPoint.in() << ".";
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "TA_Base_Core::OperationModeException", msg.str().c_str() );
			stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
			return stepState;
		}
		catch( TA_Base_Bus::ScadaCorbaTypes::NotWriteable& e)
		{
			std::ostringstream msg;
			msg << dumpContext() << ": "
			    << e.reason.in() << " for data point "
			    << m_params.dataPoint.in() << ".";
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "TA_Base_Bus::ScadaCorbaTypes::NotWriteable", msg.str().c_str() );
			stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
			return stepState;
		}
		catch( TA_Base_Core::NotAvailableException& e)
		{
			std::ostringstream msg;
			msg << dumpContext() << ": "
			    << e.reason.in() << " for data point "
			    << m_params.dataPoint.in() << ".";
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "TA_Base_Core::NotAvailableException", msg.str().c_str() );
			stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
			return stepState;
		}
		catch( TA_Base_Core::AccessDeniedException& e)
		{
			std::ostringstream msg;
			msg << dumpContext() << ": "
			    << e.reason.in() << " for data point "
			    << m_params.dataPoint.in() << ".";
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "TA_Base_Core::AccessDeniedException", msg.str().c_str() );
			stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
			return stepState;
		}
		catch( CORBA::Exception& e )
		{
			std::ostringstream msg;
			msg << dumpContext() << ": "
				<< TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e) << " for data point "
			    << m_params.dataPoint.in() << ".";
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "CORBA::Exception", msg.str().c_str() );
			stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
			return stepState;
		}
		// ++Yanrong
		catch( ... )
		{
			std::ostringstream msg;
			msg << dumpContext() << ": "
			    << " for data point "
			    << m_params.dataPoint.in() << ".";

			m_remark = "Data point write failed.";
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "Unknown Exception", msg.str().c_str() );
			stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
			return stepState;
		}

		//only wait when there are RCC checking
		if (millsec !=0 )
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Waiting for Entity Update Event: millsec=%d", millsec );
			if( !m_semaphore.timedWait(millsec + DP_CONTROL_STATE_DELAY_TIME))
			{
				LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Wait for set datapoint time out");
				stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
			}
			
			// TD20339 Yanrong++
			else
			{
				if( !m_setValueSuccess )
					stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
			}
			// ++Yanrong
		}	
		

		if( stepState != TA_Base_Core::FAILED_ACTIVE_STEP_STATE )
		{
			m_remark = "Step execution successful";

			ActivePlanBulletinBoard::instance()->update( m_flowControl.getPlan(), getStepNumber(),
														 TA_Base_Core::EXECUTED_ACTIVE_STEP_STATE, m_remark );
		}

		if ( m_isGRCType3Step )
		{
			if( stepState != TA_Base_Core::FAILED_ACTIVE_STEP_STATE )
			{
				m_execResult.set(true);
			}
			else
			{
				m_execResult.set(false);
			}
		}
		else
		{
			// TD20339 Yanrong++
			if( stepState != TA_Base_Core::FAILED_ACTIVE_STEP_STATE )
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Datapoint<%s> setValue successfully, setNextStep to flowControl",
					m_params.dataPoint.in() );
				m_flowControl.setNextStep();
			}
		}
        return( stepState );
	}
	
} // TA_Base_App


#endif //  STEPTYPE_DATA_POINT_SET
