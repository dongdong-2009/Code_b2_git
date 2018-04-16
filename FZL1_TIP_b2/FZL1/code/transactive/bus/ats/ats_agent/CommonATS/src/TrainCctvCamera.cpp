/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_Review_Branch/4669/transactive/app/ats/AtsAgent/src/TrainCctvCamera.cpp $
  * @author:  Ripple
  * @version: $Revision: #7 $
  *
  * Last modification: $DateTime: 2011/04/13 14:25:16 $
  * Last modified by:  $Author: bihui.luo $
  *
  */

#include <strstream>
#include <string>

#include "core/utilities/src/TAAssert.h"
#include "core/alarm/src/AlarmHelper.h"
#include "core/alarm/src/AlarmModificationHelper.h"
#include "core/alarm/src/AlarmHelperManager.h"
#include "core/alarm/IDL/src/AlarmMessageCorbaDef.h"
#include "core/alarm/IDL/src/AlarmAgentCorbaDef.h"
#include "core/message/types/TrainCctvCameraAlarms_MessageTypes.h"
#include "core/exceptions/src/FatalConfigurationException.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/src/SessionAccessFactory.h"

#include "bus/mathematical_evaluation/src/ExpressionEvaluator.h"
#include "bus/mathematical_evaluation/src/Tokeniser.h"

#include "bus/ats/ats_agent/CommonATS/src/CctvBooleanContext.h"
#include "bus/ats/ats_agent/CommonATS/src/CctvRealNumberContext.h"

#include "bus/ats/ats_agent/CommonATS/src/AtsCctvData.h"
#include "bus/ats/ats_agent/CommonATS/src/TrainCctvCamera.h"

using TA_Base_Core::DebugUtil;

namespace TA_IRS_Bus
{
	TrainCctvCamera::TrainCctvCamera( TA_Base_Core::TrainCctvCameraEntityDataPtr	trainCctvCameraEntityAccess )
		: m_entityData (trainCctvCameraEntityAccess),
		  m_expressionEvaluator(NULL), 
		  m_booleanContext(NULL), 
		  m_realNumberContext(NULL),
		  m_cctvData(NULL),
		  m_alarmGenerated(false),
		  /*/TD18093, jianghp TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().(TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper()),
          m_alarmModHelper(TA_Base_Core::AlarmHelperManager::getInstance().getModificationAlarmHelper()),*/
		  m_cameras (""),
		  m_cameraAlarmExpression ("")
	{
		// Obtain handle to the Alarm Helper
		if (getCameraAlarmExpression() == "")
		{
			TA_THROW(TA_Base_Core::FatalConfigurationException(
						"Configuration Error: No Valid Camera Alarm Expression"));
		}
	}

	TrainCctvCamera::~TrainCctvCamera()
	{
		// delete expression evaluator stuff
		if ( NULL != m_expressionEvaluator )
		{
			delete m_expressionEvaluator;
			m_expressionEvaluator = NULL;
		}

		if ( NULL != m_booleanContext )
		{
			delete m_booleanContext;
			m_booleanContext = NULL;
		}

		if ( NULL != m_realNumberContext )
		{
			delete m_realNumberContext;
			m_realNumberContext = NULL;
		}

		TA_Base_Core::AlarmHelperManager::getInstance().cleanUp();

		m_cctvData = NULL;
	}

	//
	//	IEntity Interface Implementation
	//

	/**
	  * isValid
	  *
	  * This is part of the implementation of the IEntity Interface
	  * This method returns true if the TrainCctvCamera entity is 
	  * valid. Otherwise it will return false.
	  *
	  * @return TRUE if the entity is valid, FALSE otherwise 
	  */
	bool TrainCctvCamera::isValid()
	{
		return true;
	}

	
    /**
      * start
      *
	  * This is part of the implementation of the IEntity Interface
      * This method must start any object specific tasks,
      * such as creating its own thread.
      */
	void TrainCctvCamera::start()
	{
	}

    /**
      * stop
      *
	  * This is part of the implementation of the IEntity Interface
      * This method will stop any object specific tasks, such 
      * as stopping its own thread. 
      */
	void TrainCctvCamera::stop()
	{
	}

	/**
      * update
      *
	  * This is part of the implementation of the IEntity Interface
      * This method is called by the agent controlling the entity to ask 
      * the entity to update its configuration. Called after an update 
      * configuration event is received.
      */
	void TrainCctvCamera::update(const TA_Base_Core::ConfigUpdateDetails& updateEvent)
	{
		TA_Base_Core::ThreadGuard guard(m_lock);

		m_entityData->invalidate();
		m_cameras = "";
		m_cameraAlarmExpression = "";
		m_variableStates.clear();
	}

	/**
      * remove
      *
	  * This is part of the implementation of the IEntity Interface
      * When a configuration update event is received to delete an entity 
      * from an agent, the agent can call this method to tell the entity 
      * to commence any clean up opperations.
      * 
      * The agent then still has to remove the object from its vector of entities.
      */
	void TrainCctvCamera::remove()
	{
	}

	/**
      * setToControlMode
      *
	  * This is part of the implementation of the IEntity Interface
      * This method can be called by the agent responsible for the entity in
      * the event of its mode of operation changing to control. This enables the
      * entity to perform tasks such as connecting to any field devices it is
      * responsible for communicating with.
      */
	void TrainCctvCamera::setToControlMode()
	{
		changeOperationMode(TA_Base_Core::Control);
	}

	/**
      * setToMonitorMode
      *
	  * This is part of the implementation of the IEntity Interface
      * This method can be called by the agent responsible for the entity in
      * the event of its mode of operation changing to monitor. This enables the
      * entity to perform tasks such as disconnecting from any field devices it was
      * communicating with.
      */
	void TrainCctvCamera::setToMonitorMode()
	{
		changeOperationMode(TA_Base_Core::Monitor);
	}

	TA_Base_Core::EOperationMode TrainCctvCamera::getOperationMode()
	{
		TA_Base_Core::ThreadGuard guard(m_lock);

		return m_operationMode;
	}

	bool TrainCctvCamera::changeOperationMode(TA_Base_Core::EOperationMode operationMode)
	{
		TA_Base_Core::ThreadGuard guard(m_lock);

		if (m_operationMode == operationMode)
		{
			// Already in this Mode
			return false;
		}

		// save the operation mode
		m_operationMode = operationMode;

		return true;
	}

	void TrainCctvCamera::addExpressionData(AtsCctvData* cctvData)
	{		
		// create an instance of the expression evaluator (TA_Bus library)
		m_expressionEvaluator = new TA_Base_Bus::ExpressionEvaluator ();

		// add the context to the ExpressionEvaluator
		m_booleanContext = new CctvBooleanContext (*cctvData);
		m_expressionEvaluator->addBooleanContext ( m_booleanContext );
		m_realNumberContext = new CctvRealNumberContext (*cctvData);
		m_expressionEvaluator->addRealNumberContext ( m_realNumberContext );
		m_cctvData = cctvData;
	}

	void TrainCctvCamera::updateCamera(bool isAlarmAgentAvailable)
	{
		std::ostringstream fetext;
		fetext << "updateCamera() train " << m_entityData->getPhysicalTrainNumber();
		fetext << " camera " << getCameras();
		FUNCTION_ENTRY(fetext.str().c_str());

		if ( (TA_Base_Core::Control != getOperationMode() ) || ( m_cctvData == NULL) )
		{
			FUNCTION_EXIT;
			return;
		}

		bool cameraAlarmState = false;
		bool updateCameraAlarm = false;

		if (m_cctvData->isAtStation(m_entityData->getLocation()) ) 
		{	
			// Only evaluate if OA1 or OA2 valid
			if (m_cctvData->isOaValid())
			{
				// if the Evaluator is available
				if ( 0 != m_expressionEvaluator )
				{
					try
					{
						m_expressionEvaluator->setNewExpression ( m_entityData->getCameraAlarmExpression() );
						cameraAlarmState = m_expressionEvaluator->evaluateBooleanExpression();
						updateCameraAlarm = true;

						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
									"updateCamera() - train %d camera %s expression %s evaluated to %s",
									m_entityData->getPhysicalTrainNumber(), 
									getCameras().c_str(),
									m_entityData->getCameraAlarmExpression().c_str(),
									(cameraAlarmState?"true":"false"));
					}
					catch (TA_Base_Core::MathematicalEvaluationException&)
					{
						// Will have been logged a few times by now...
					}
				}
			}
			else
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
						"updateCamera() - OA1 and OA2 not valid - not updated");
			}
		}
		else
		{
			// Train not currently in this location
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
						"updateCamera() - train %d camera %s  - not at location %d",
						m_entityData->getPhysicalTrainNumber(), 
						getCameras().c_str(),
						m_entityData->getLocation());

			cameraAlarmState = false;
			updateCameraAlarm = true;
		}

		if (updateCameraAlarm && isAlarmAgentAvailable)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
						"updateCamera() - train %d camera %d %s alarm",
						m_entityData->getPhysicalTrainNumber(), 
						getCameras().c_str(),
						cameraAlarmState?"setting":"clearing" );

			

		 if ( !m_cctvData->isAtsDepot () ) //cl-21647 bihui
			 setCameraAlarmState(cameraAlarmState);
		}

		FUNCTION_EXIT;
	}

	void TrainCctvCamera::clearCameraAlarmState(bool isAlarmAgentAvailable)
	{
		m_alarmGenerated = true;
		if(isAlarmAgentAvailable == true)
		{
			setCameraAlarmState(false);
		}
	}

	std::string TrainCctvCamera::getCameras()
	{
		TA_Base_Core::ThreadGuard guard(m_lock);

		std::vector<unsigned long> cameraVector = m_entityData->getCameraNumbers();

		if (cameraVector.size() != 0 && m_cameras.length() == 0)
		{
			std::vector<unsigned long>::iterator cameraIt;
			std::ostringstream cameraStream;
			for (cameraIt = cameraVector.begin(); cameraIt != cameraVector.end(); cameraIt++)
			{
				if (cameraIt != cameraVector.begin())
				{
					cameraStream << ",";
				}
				cameraStream << (*cameraIt);
			}

			m_cameras = cameraStream.str();
		}

		return m_cameras;
	}

	std::string TrainCctvCamera::getCameraAlarmExpression()
	{
		TA_Base_Core::ThreadGuard guard(m_lock);

		std::string alarmExpression = m_entityData->getCameraAlarmExpression();

		if (alarmExpression.length() != 0 && m_cameraAlarmExpression.length() == 0)
		{
			bool expressionError = false;

			try
			{
				m_variableStates.clear();

				TA_Base_Bus::Tokeniser tokeniser(alarmExpression);

				// Check expression and build list of variables
				std::vector< boost::shared_ptr< TA_Base_Bus::Token > > tokens = tokeniser.getTokens();
				std::vector< boost::shared_ptr< TA_Base_Bus::Token > >::iterator it;
				

				std::string variable = "";
				bool equality = false;

				for (it = tokens.begin(); it != tokens.end() && expressionError == false; it++)
				{
					switch ((*it)->getType())
					{
					case TA_Base_Bus::EETT_Variable:
						if (variable == "")
						{
							// {Varaible} found
							variable = (*it)->getVariable();
						}
						else
						{
							// Variable can not follow a variable
							expressionError = true;

							LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
									"{Variable} following {Variable} in Camera Alarm Expression, Entity Key=%u, Expresstion %s",
									m_entityData->getKey(),
									alarmExpression.c_str());
						}
						break;

					case TA_Base_Bus::EETT_BooleanOperator:
						// This is OK, but we are particularly looking for equality here
						if ((*it)->getBooleanOperator() == "=")
						{
							if (variable != "")
							{
								// {Variable}= found
								equality = true;
							}
							else
							{
								// Equality operator must follow a variable
								expressionError = true;

								LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
									"Equality operator without {Variable} in Camera Alarm Expression, Entity Key=%u, Expresstion %s",
									m_entityData->getKey(),
									alarmExpression.c_str());
							}
						}
						else if ((*it)->getBooleanOperator() !=	"OR")
						{
							// Only allowing OR as the logic gets too complex for the events otherwise
							expressionError = true;

							LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
								"Invalid Operator in Camera Alarm Expression, Entity Key=%u, Expresstion %s",
								m_entityData->getKey(),
								alarmExpression.c_str());
						}
						break;

					case TA_Base_Bus::EETT_BooleanValue:
						if (variable != "")
						{
							if (equality)
							{
								// Store the variable
								VariableState state;
								state.variable = variable;
								state.value = (*it)->getBooleanValue();
								state.matches = false;

								m_variableStates.push_back(state);

								variable = "";
								equality = false;
							}
							else
							{
								// boolean value must follow {Variable}=
								expressionError = true;

								LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
									"Boolean value without {Variable}= in Camera Alarm Expression, Entity Key=%u, Expresstion %s",
									m_entityData->getKey(),
									alarmExpression.c_str());
							}
						}
						break;

					case TA_Base_Bus::EETT_OpenParenthesis:
					case TA_Base_Bus::EETT_CloseParenthesis:
						// These are OK, just need to make sure we weren't expecting
						// something for a variable
						if (variable != "")
						{
							expressionError = true;

							LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
								"Variable followed by Parenthesis in Camera Alarm Expression, Entity Key=%u, Expresstion %s",
								m_entityData->getKey(),
								alarmExpression.c_str());
						}
						break;


					case TA_Base_Bus::EETT_BooleanUnaryOperator:
					case TA_Base_Bus::EETT_RealNumOperator:
					case TA_Base_Bus::EETT_RealValue:
					case TA_Base_Bus::EETT_IntegerValue:
					case TA_Base_Bus::EETT_StringLiteral:
					default:
						// Don't allow these for cameras
						expressionError = true;

						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
							"Invalid Operator/Value in Camera Alarm Expression, Entity Key=%u, Expresstion %s",
							m_entityData->getKey(),
							alarmExpression.c_str());

						break;
					}
				}
			}
			catch (...)
			{
				expressionError = true;

				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
					"Unexpected exception parsing Camera Alarm Expression, Entity Key=%u, Expresstion %s",
					m_entityData->getKey(),
					alarmExpression.c_str());
			}

			if (expressionError)
			{
				m_cameraAlarmExpression = "";
			}
			else
			{
				m_cameraAlarmExpression = alarmExpression;
			}
		}

		return m_cameraAlarmExpression;
	}

	std::string TrainCctvCamera::getEvents()
	{
		std::string events = "";
		std::vector<VariableState>::iterator it;

		for (it = m_variableStates.begin(); it != m_variableStates.end(); it++)
		{
			if((*it).matches)
			{
				if (events.length() > 0)
				{
					events += ", ";
				}

				events += (*it).variable;
			}
		}

		return events;
	}

	bool TrainCctvCamera::updateEvents()
	{
		bool eventsChanged = false;

		std::vector<VariableState>::iterator it;

		for (it = m_variableStates.begin(); it != m_variableStates.end(); it++)
		{
			if (m_booleanContext != NULL)
			{
				try
				{
					bool matches = (m_booleanContext->getValue((*it).variable) == (*it).value);

					if (matches != (*it).matches)
					{
						(*it).matches = matches;
						eventsChanged = true;
					}
				}
				catch (...)
				{
				}
			}
		}

		return eventsChanged;
	}

	void TrainCctvCamera::setCameraAlarmState(bool isInAlarm)
	{
		FUNCTION_ENTRY("setCameraAlarmState()");
		
		// if the current comms status is bad then raise an alarm
		if ( true == isInAlarm  )
		{
			// Evaluate Events
			bool eventsChanged = updateEvents();
			
			if (true == m_alarmGenerated && eventsChanged)
			{
				// close the unique alarm
				closeAlarm();
			}
			
			// if no alarm and event have been generated
			if ( false == m_alarmGenerated )
			{
				openAlarm();
			}
		}
		
		//
		// Else if the current camera state is good then close the alarm
		//
		else
		{
			// if alarm and event have been generated
			if ( true == m_alarmGenerated )
			{
				closeAlarm();
			}
		}

		FUNCTION_EXIT;
	}

    void TrainCctvCamera::closeAlarm()
    {
        static std::string session = TA_Base_Core::SessionAccessFactory::getInstance().getSuperSessionId();

        bool closedAndAcked = false;

        try
        {
            // get the alarm to close
            TA_Base_Core::AlarmDetailCorbaDef* alarm = 
                TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().getActiveAlarmForEntityOfType( TA_Base_Core::TrainCctvCameraAlarms::TrainCctvCameraAlarm,
                                                              m_entityData->getKey(),
                                                              m_entityData->getLocation() );

			if (alarm)
			{
                // set the session ID
                // for some stupid f#$(@&@ reason it gets unset somewhere 
                TA_Base_Core::RunParams::getInstance().set(RPARAM_SESSIONID, session.c_str());

                std::vector<TA_Base_Core::AlarmDetailCorbaDef*> alarms;
                alarms.push_back(alarm);
				// TD 15011 to differentiate between operator and system close/ack alarms
				// use the non-operator interfaces for ack and close
                // TA_Base_Core::AlarmHelperManager::getInstance().getModificationAlarmHelper().closeAndAcknowledgeAlarms ( alarms );

				//  [7/13/2009 cuilike++]:CL-18778 This defect sais the alarm should just be closed but acknowleged, so comment this line
				//TA_Base_Core::AlarmHelperManager::getInstance().getModificationAlarmHelper().acknowledgeAlarms( alarms );
				//  [7/13/2009 ++cuilike]:CL-18778
				TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().closeAlarmAtLocation( TA_Base_Core::TrainCctvCameraAlarms::TrainCctvCameraAlarm, m_entityData->getKey(),
                                                              m_entityData->getLocation(),"NORMAL" );

                closedAndAcked = true;

                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
					"Closed and Acked alarm AlarmTypeKey=%s, Location=%u",
					alarm->alarmID.in(),
					m_entityData->getLocation());

                delete alarm;
			}
        }
        catch (TA_Base_Core::TransactiveException& te)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", te.what() );
        }
        catch (const TA_Base_Core::AlarmAgentCorbaDef::AlarmException& ae)
        {
            // means the alarm wasnt found
            LOG_EXCEPTION_CATCH( SourceInfo, "AlarmAgentException", ae.reason.in() );
        }
        catch( const CORBA::Exception& e )
		{
			std::string text = "closeAlarm()" 
								+ TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e );
			LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", text );
		}
        catch (...)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "while closing and acking alarm", "..." );
        }

        // failsafe
        if (false == closedAndAcked)
        {
			TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().closeAlarmAtLocation ( TA_Base_Core::TrainCctvCameraAlarms::TrainCctvCameraAlarm,
										m_entityData->getKey(),
										m_entityData->getLocation());

            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
				"Alarm closed BY TYPE at location. AlarmTypeKey=%s, Key=%u, Location=%u",
				TA_Base_Core::TrainCctvCameraAlarms::TrainCctvCameraAlarm.getTypeKey().c_str(),
				m_entityData->getKey(),
				m_entityData->getLocation());
        }

		m_alarmGenerated = false;
    }
    
    void TrainCctvCamera::openAlarm()
    {
        // construct the DescriptionParameters list
		TA_Base_Core::DescriptionParameters descriptionParameters;
		
		std::ostringstream trainNumber;
		unsigned int phisicalNum = (unsigned int) m_entityData->getPhysicalTrainNumber();
		if (phisicalNum < 10)
		{
			trainNumber << "0";
		}
		
		trainNumber << phisicalNum;

		unsigned long serviceNum = m_cctvData->getServiceNumber();
		trainNumber << "/" << serviceNum;

		TA_Base_Core::NameValuePair pair1 ( "PhysicalTrainNumber", trainNumber.str() );
		descriptionParameters.push_back ( &pair1 );	

		std::string camera = getCameras();

		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "[TD15561] before replacement, camera is %s",
					camera.c_str());

		std::string replacement = "_";
		std::string findMe = ","; 
		replaceChars(camera,findMe, replacement);

		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "[TD15561] after replacement, camera is %s",
					camera.c_str());

		TA_Base_Core::NameValuePair pair2 ( "CameraNumber", camera );
		descriptionParameters.push_back ( &pair2 );
	

		std::string location = m_entityData->getLocationName();
		TA_Base_Core::NameValuePair pair3 ( "LOCATION", location );
		descriptionParameters.push_back ( &pair3 );

//		std::string alarmValue("ALARM");
//		TA_Base_Core::NameValuePair pair4 ( "DataPointValue", alarmValue );
//		descriptionParameters.push_back ( &pair4 );

		// format the event according to customer's requirement.
		std::string strEvent = getEvents();
		std::string::size_type n = strEvent.find(':');
		
		std::string firstPart = strEvent.substr(0,n+1);
		
		std::ostringstream  streamEvent;
		streamEvent << firstPart << " Car " << strEvent.substr(n+1, strEvent.size()) ;

		std::string oa1Oa2Events = streamEvent.str();

		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "[TD15561] before replacement, Oa1Oa2 events is %s",
					oa1Oa2Events.c_str());

		replacement = "-";
		findMe = ":";
		replaceChars(oa1Oa2Events,findMe,replacement);

		replacement = "_";
		findMe = ","; 
		replaceChars(oa1Oa2Events,findMe, replacement);

		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "[TD15561] after replacement, Oa1Oa2 events is %s",
					oa1Oa2Events.c_str());

	
		
		TA_Base_Core::NameValuePair pair4 ( "Oa1Oa2Events", oa1Oa2Events);
		descriptionParameters.push_back ( &pair4 );

		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "[TD15561] trainNumber:%s,cameraNumber:%s,Oa1Oa2Events:%s,location:%s",
					trainNumber.str().c_str(),camera.c_str(),oa1Oa2Events.c_str(),location.c_str());

		try
		{
			//[[Wuzhongyi CL-20937 2009-9-22 submit an unique alarm, use the getPhysicalSubsystem key
			unsigned long correctSubsystem = 0;
			try
			{
				correctSubsystem = ( m_entityData->getPhysicalSubsystem() > 0 ? m_entityData->getPhysicalSubsystem() : m_entityData->getSubsystem() );
			}
			catch(TA_Base_Core::TransactiveException& err)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", err.what());
				correctSubsystem = m_entityData->getSubsystem();
			}
			TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().submitAlarmWithAsset ( TA_Base_Core::TrainCctvCameraAlarms::TrainCctvCameraAlarm,
										 m_entityData->getKey(),			// entity key
										 m_entityData->getTypeKey(),		// entity type key
										 descriptionParameters,
										 m_entityData->getName(),		// entity name
										 m_entityData->getLocation(),	// location key
										 correctSubsystem,
										 m_entityData->getAssetName(),
										 TA_Base_Core::AlarmConstants::defaultTime );
        
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
				"Alarm submitted. AlarmTypeKey=%s, Key=%u, TypeKey=%u, Name=%s, locationKey=%u, subsystemKey=%u",
				TA_Base_Core::TrainCctvCameraAlarms::TrainCctvCameraAlarm.getTypeKey().c_str(),
				m_entityData->getKey(),
				m_entityData->getTypeKey(),
				m_entityData->getName().c_str(),	
				m_entityData->getLocation(),
				correctSubsystem);
			//]]Wuzhongyi CL-20937

			// set up the flags
			m_alarmGenerated = true;
		}
		catch ( ... )
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", 
								 "setCameraAlarmState() - Unknown Exception caught while submitting alarm.");
		}
    }

	const unsigned char TrainCctvCamera::getPhysicalTrainNumber()
	{
		return (const unsigned char)m_entityData->getPhysicalTrainNumber();
	}


	void TrainCctvCamera::replaceChars(std::string& modifyMe,  std::string findMe, std::string newChars)
	{
		std::string::size_type startIndex  = 0;
		std::string::size_type i = 0;
		
		do {	
			
			// Look in modifyMe for the "find string"
			// starting at position 0
			i = modifyMe.find(findMe, startIndex);
			// Did we find the string to replace?
			if(i != std::string::npos)
			{
				// Replace the find string with newChars
				modifyMe.replace(i,newChars.size(),newChars);
				startIndex = i + 1;
			}
			
		} while(i != std::string::npos);
	}
}
