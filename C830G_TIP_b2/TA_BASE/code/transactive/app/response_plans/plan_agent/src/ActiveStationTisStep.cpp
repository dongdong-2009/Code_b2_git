/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_agent/src/ActiveStationTisStep.cpp $
  *  @author:  Bart Golab
  *  @version: $Revision: #2 $
  *
  *  Last modification: $DateTime: 2009/02/10 15:59:52 $
  *  Last modified by:  $Author: builder $
  *
  *  Represents a Station Traveller Info System step in an executing plan.
  */

#include "PlanAgent.h"

#include "core/data_access_interface/tis_agent_4669/src/PredefinedMessageLibraryAccessFactory.h"
#include "core/data_access_interface/tis_agent_4669/IDL/src/TISMessageDataDefinitions.h"
#include "core/data_access_interface/tis_agent_4669/src/PredefinedMessageLibraryTable.h"

#include "bus/signs_4669/tis_agent_access/src/TISAgentAccessFactory.h"

#if defined ( STEPTYPE_STATION_TRAVELLER_INFO_SYSTEM )


// namespace alias for Corba structures/types

namespace TA_Base_App
{

    ActiveStationTisStep::ActiveStationTisStep(const TA_Base_Core::StepDetail& stepDetail,
                                               const TA_Base_Core::StationTravellerInfoSystemParameters& params,
                                               IActivePlanFlow& flowControl ) :
		ActiveStep(stepDetail, flowControl),
		m_params(params),
		m_flowControl(flowControl)
	{
	}

	TA_Base_Core::EActiveStepState ActiveStationTisStep::executeImpl( void )
	{
        // TODO: BartG 23/6/05
        //       TD6840 - to be implemented when the STIS subsystem has been extended to meet all the requirements of this step.
        //
        // The following issues have been raised with the Visual team.
        // 1. Sending STIS messages by station level is, apparently, unimplemented. The ISTISManagerCorbaDef 
        //    enables you to target specific station level(s), however, these not seem to be used for anything.
        // 2. The requirement for the STIS plan step is such that the TIS Agent displays whatever it can even 
        //    if a particular PID/level is not applicable to a particular station. In the current implementation, 
        //    the TIS Agent will throw an exception as soon as it encounters an error. It would be better if it 
        //    were to consolidate all errors and return them after completing the request.
        // 3. Similar to #2, if a request is sent to "all" stations (station name = "AST") and one of the stations
        //    has a mismatching library version, the TIS Agent throws an exception straight away. Again, it would 
        //    be preferable if it could do as many stations as possible and then return any errors afterwards.
        //
        // Also, a comment from Adam Radics re message start/end times:
        // "When we give the start and end times to the STIS server they are full YYYYMMDDHHMMSS strings, so potentially 
        //  they could be any time, although if they are in the past I think it whinges." 
        //
        // For the STIS plan step, the start and end times are based on the duration parameter, i.e. start = now and
        // end = start + duration. In light of Adam's comment it might be necessary to increment the start time slightly
        // to avoid it becoming in the past when the message display request reaches the server. In any event, this needs 
        // investigating...

		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "Station TIS step %lu", getStepNumber() );

		TA_Base_Core::EActiveStepState stepState = TA_Base_Core::EXECUTED_ACTIVE_STEP_STATE;

		m_remark = "Step executed.";

		try
		{
			//check the version 
			unsigned short currentVersion;
            CORBA_CALL_RETURN( currentVersion, TA_Base_Bus::TISAgentAccessFactory::getInstance().getSTISAgent(), getCurrentSTISMessageLibraryVersion, () );//limin
			
			if(currentVersion == m_params.message.libraryVersion)
			{
				//get the message from DB
				std::string libraryType(TA_Base_Core::TA_TISAgentDAI::LIBRARY_TYPE_STIS);
				TA_Base_Core::PredefinedMessage message = 
					TA_Base_Core::PredefinedMessageLibraryAccessFactory::getInstance().getPredefinedMessage(currentVersion,
																libraryType, 
																(int)m_params.message.librarySection, (unsigned short)m_params.message.messageId);
				
				//get start time and ent time
				//get the current time as the start time 
				time_t startTimet = time(NULL);
				time_t endTimet = startTimet + m_params.duration;
				
				struct tm* startTM = localtime(&startTimet);
				std::stringstream timeString;
				timeString	<< startTM->tm_year + 1900 
							<< std::setw(2) << std::setfill('0') << startTM->tm_mon + 1
							<< std::setw(2) << std::setfill('0') << startTM->tm_mday
							<< std::setw(2) << std::setfill('0') << startTM->tm_hour 
							<< std::setw(2) << std::setfill('0') << startTM->tm_min 
							<< std::setw(2) << std::setfill('0') << startTM->tm_sec;
				std::string startTime = timeString.str();

				struct tm* endTM = localtime(&endTimet);
				timeString.str("");
				if(startTM->tm_mday != endTM->tm_mday)
				{
					//the end time is in the next day, so use the "23:59:59" instead
					timeString	<< startTM->tm_year + 1900 
								<< std::setw(2) << std::setfill('0') << startTM->tm_mon + 1
								<< std::setw(2) << std::setfill('0') << startTM->tm_mday
								<< std::setw(2) << std::setfill('0') << 23 
								<< std::setw(2) << std::setfill('0') << 59
								<< std::setw(2) << std::setfill('0') << 59;					
				}
				else
				{
					timeString	<< endTM->tm_year + 1900 
								<< std::setw(2) << std::setfill('0') << endTM->tm_mon + 1
								<< std::setw(2) << std::setfill('0') << endTM->tm_mday
								<< std::setw(2) << std::setfill('0') << endTM->tm_hour 
								<< std::setw(2) << std::setfill('0') << endTM->tm_min 
								<< std::setw(2) << std::setfill('0') << endTM->tm_sec;
				}
				std::string endTime = timeString.str();
				
				//convert the destinatonList
				//in plan we only can select all station or one station, 
				//so the destionationList length is 1
				TA_Base_Core::STISDestinationList destinationList;
				destinationList.length(1);

				if(m_params.destination.allStations)
				{
					destinationList[0].station = "AST";
				}
				else
				{
					destinationList[0].station = m_params.destination.station;
				}

				destinationList[0].levels.length(1);//only one level can be selected in plan
				destinationList[0].levels[0] = m_params.stationLevel;
				
				destinationList[0].pids.length(m_params.stationPids.length());
				for(int i = 0 ; i < m_params.stationPids.length() ; ++i)
				{
					destinationList[0].pids[i] = m_params.stationPids[i];
				}

				std::string sessionId = ActivePlanBulletinBoard::instance()->getSession(m_flowControl.getPlan());
				//call tis agent to send the message
				CORBA_CALL( TA_Base_Bus::TISAgentAccessFactory::getInstance().getSTISAgent(), submitPredefinedDisplayRequest, ( destinationList, 
					( message.librarySection == 1 ? TA_Base_Core::NORMAL_SECTION : TA_Base_Core::EMERGENCY_SECTION ), 
									currentVersion, m_params.message.messageId, startTime.c_str(), endTime.c_str(), message.priority,
									sessionId.c_str() ) );//limin

				//On successful completion of the step
				m_flowControl.setNextStep();
			}
			else
			{
				std::ostringstream msg;
				msg << dumpContext() << ": ";
				stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "%s STIS message version conflict. Failed to run plan.", msg.str().c_str());
			}
		}
		catch( const TA_Base_Core::DatabaseException& e)
		{
			std::ostringstream msg;
			msg << dumpContext() << ": "
				<< e.what();
			
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "TA_Base_Core::DatabaseException", msg.str().c_str() );
			stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
		}
		catch( const TA_Base_Core::DataException & e)
		{
			std::ostringstream msg;
			msg << dumpContext() << ": "
				<< e.what();
			
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "TA_Base_Core::DataException", msg.str().c_str() );
			stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
		}
		catch ( const TA_Base_Bus::ISTISManagerCorbaDef::STISCommunicationTimeoutException& e)
		{
			std::ostringstream msg;
			msg << dumpContext() << ": "
				<< "Timed out while waiting for STIS server response";
			
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "ISTISManagerCorbaDef::STISCommunicationTimeoutException", msg.str().c_str() );
			stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
		}
		catch ( const TA_Base_Bus::ISTISManagerCorbaDef::STISCommunicationFailureException& e)
		{
			std::ostringstream msg;
			msg << dumpContext() << ": "
				<< "STIS Communications error.";
			
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "ISTISManagerCorbaDef::STISCommunicationFailureException", msg.str().c_str() );
			stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
		}
		catch ( const TA_Base_Bus::ISTISManagerCorbaDef::STISServerNotConnectedException& e)
		{
			std::ostringstream msg;
			msg << dumpContext() << ": "
				<< "TIS Agent not connected to STIS Server.";
			
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "ISTISManagerCorbaDef::STISServerNotConnectedException", msg.str().c_str() );
			stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
		}
		catch ( const TA_Base_Bus::ISTISManagerCorbaDef::STISInvalidParameterException& e)
		{
			std::ostringstream msg;
			msg << dumpContext() << ": "
				<< e.details.in() ;
			
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "ISTISManagerCorbaDef::STISInvalidParameterException", msg.str().c_str() );
			stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
		}
		catch( const TA_Base_Core::ObjectResolutionException& e )
		{
			std::ostringstream msg;
			msg << dumpContext() << ": "
				<< e.what();
			
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "TA_Base_Core::ObjectResolutionException", msg.str().c_str() );
			stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
		}
		catch ( const CORBA::Exception& e )
		{
			std::ostringstream msg;
			msg << dumpContext() << ": "
				<< TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str();
			
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "CORBA::Exception", msg.str().c_str() );
			stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
		}
		catch( const ActiveStepParamException& e )
		{
			std::ostringstream msg;
			msg << dumpContext() << ": "
				<< e.what();
			
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "TA_Base_App::ActiveStepParamException", msg.str().c_str() );
			stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
		}
		catch( ... )
		{
			std::ostringstream msg;
			msg << dumpContext() << ": ";
			
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "Unknown Exception", msg.str().c_str() );
			stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
		}
		
		return stepState;
	}

} // TA_Base_App


#endif //  STEPTYPE_STATION_TRAVELLER_INFO_SYSTEM
