/**
  * The source code in this file is the property of
  * MI Services Group and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/trains/train_agent/src/TrainATS.cpp $
  * @author:  Glen Kidd
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  *
  */

#ifdef __WIN32__
#pragma warning(disable:4250 4503)
#endif

#include "app/trains/train_agent/src/TrainATS.h" 

#include "core/utilities/src/TAAssert.h"
#include "core/naming/src/NamedObject.h"
#include "core/data_access_interface/entity_access/src/TrainAgentEntityData.h"
#include "bus/ats/ats_agent/IDL/src/IAtsTrainCorbaDef.h"
#include "app/trains/train_agent/src/TrainException.h"
#include "TrainEventSubscriber.h"
#include "TrainAgent.h"
#include "TrainManager.h"
#include "TrainCorbaTypes.h"
#include "OperationModeManager.h"

#include "app/trains/train_agent/TrainProtocolLibrary/src/miscChangeAreaCommand.h"

namespace 
{
	
	typedef TA_Base_Core::NamedObject<TA_IRS_Bus::IAtsTrainCorbaDef,
		TA_IRS_Bus::IAtsTrainCorbaDef_ptr,
		TA_IRS_Bus::IAtsTrainCorbaDef_var> AtsNamedObject;

}

using namespace TA_IRS_App;

/**********************
** Background thread **
**********************/

AtsTrainInfoListHelper::AtsTrainInfoListHelper(const TrainEventSubscriber & callback, TrainAgent & trainAgent, const std::string &atsEntityName) 
: m_callback(callback), 
m_trainAgent(trainAgent), 
m_atsEntityName(atsEntityName), 
m_terminate(false) 
{
}

AtsTrainInfoListHelper::~AtsTrainInfoListHelper()
{
	terminateAndWait();
	//terminate();
}

void AtsTrainInfoListHelper::run() 
{
	LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, "Commencing background load of ATS getTrainInformationList()");
	bool success = false;
	
	AtsNamedObject ats;
	ats.setEntityName(m_atsEntityName,false);
	int retry_delay = 5; // initial delay (in sec) if we cannot get ATS info
	int retry_attempts = 0;

	TA_THREADGUARD(atsSubscriptionLock); // to prevent new notification coming in out of sequence
	while(!success && !m_terminate)
	{
		try
		{
			TA_IRS_Bus::IAtsTrainCorbaDef::TrainInfoList * trainInfo;
			CORBA_CALL_RETURN( trainInfo,
			                   ats,
			                   getTrainInformationList,
			                   () );
			
			for (unsigned int i = 0; i < trainInfo->length(); ++i)
			{
				ATSTrainMessage atsMsg = convertATSEvent(&(*trainInfo)[i]);

				{
					std::stringstream s;
					s << "Processing ATS Info message " << atsMsg;
					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, s.str().c_str());
				}

                // pass onto method below for processing
                m_callback.m_trainManager.onATSEvent(atsMsg);
			}
			
			success = true;
		}
		catch (TA_Base_Core::NotAvailableException & e)
		{
			std::stringstream s;
			s << "ATS claims Train Table is unavailable [" << e.reason << "]";
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugWarn, s.str().c_str());			
		}
		catch (std::exception & e) // should get TransactiveExceptions
		{
			std::stringstream s;
			s << "Got exception from ATS: " << typeid(e).name();
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  
				TA_Base_Core::DebugUtil::DebugError, s.str().c_str());
		}
		catch (...)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  
				TA_Base_Core::DebugUtil::DebugError, "Unexpected exception ... ");
			
			//throw; don't throw, we want the thread to retry
		}
		
		if (!success) 
		{
			retry_attempts++;
			if (5 == retry_attempts)
			{
				retry_delay = 10;
			} 
			else if (10 == retry_attempts)
			{
				retry_delay = 30;
			}
			std::stringstream s;
			s << "Unsuccessful ATS getTrainInformationList(), retrying in " << retry_delay << " seconds" 
				<< std::ends;
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugWarn, s.str().c_str());
			
			/// @note would be better to interupt the sleep, but no core libs exist to tell the thread to do so.
			for (int i = 0 ; i < retry_delay && !m_terminate; ++i)
			{
				sleep(1000); // retry_delay msec in total, unless m_terminate received
			}
			
		}
	}
	
	LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, "Completed background load of ATS getTrainInformationList()");
}

void AtsTrainInfoListHelper::terminate() 
{
	FUNCTION_ENTRY("terminate");
	
	m_terminate = true;
//	terminateAndWait();
	
	FUNCTION_EXIT;
}


/***********************
**
***********************/

TrainATS::TrainATS(TrainAgent & trainAgent, ITrainManager& callback, 
				   ITrainNotificationFacade& notifier, 
				   TrainTypes::TrainAgentLocationType agentLocationType, 
				   ITrainStatusUpdateMonitor& trainStatusUpdateMonitor,
				   OperationModeManager& operationModeManager,
				   TrainActivityMonitor& trainActivityMonitor,
                   TrainWorkQueueManager& trainWorkQueueManager) 
: 
m_trainAgent(trainAgent),
m_trainManager(callback),
m_notifier(notifier),
m_agentLocationType(agentLocationType),
m_trainStatusUpdateMonitor(trainStatusUpdateMonitor),
m_operationModeManager(operationModeManager),
m_trainActivityMonitor(trainActivityMonitor),
m_workQueueManager(trainWorkQueueManager)
{
	m_trainManager.addDelegate(this);
    m_trainManager.addListener(this);
}



TrainATS::~TrainATS()
{
}

bool TrainATS::delegate(ATSTrainMessage& msg)
{
    bool consumed = true;

	onATSEvent(msg);
    
	return consumed;
}



void TrainATS::onATSEvent(ATSTrainMessage &atsMsg)
{
	FUNCTION_ENTRY("onATSEvent");

	/// Ensure only one ats message processed at a time
	TA_THREADGUARD(m_atsEventLock);

	// Process the ATS message
	if (atsMsg.isValid)
	{	
		bool					newTrain = false;
		unsigned long			oldLocation = 0;
		TrainTypes::AtsValid	oldValidity = TrainTypes::AtsValid_Unknown;
		double					oldCctvZone = -1;
		const unsigned long		agentLocation = m_trainAgent.getLocalTrainAgentEntity()->getLocation();

		try
		{
			oldLocation = m_trainManager.getTrainStatusManager().getTrainLocation(atsMsg.trainID);
			oldValidity = m_trainManager.getTrainStatusManager().getTrainAtsValid(atsMsg.trainID);
			oldCctvZone = m_trainManager.getTrainStatusManager().getTrainCctvZone(atsMsg.trainID);
		}
		catch (TrainInvalidTrainException &)
		{
			newTrain = true;
		}

		if (oldValidity != TrainTypes::AtsValid_Valid)
		{
			newTrain = true;
		}
			
		// setTrainDetails() ensures that a new train record is created if it doesn't exist yet
		m_trainManager.getTrainStatusManager().setTrainDetails(
			atsMsg.trainID,
			atsMsg.currentLocation,
			atsMsg.currentCctvZone,
			atsMsg.serviceNumber,
			atsMsg.track,
			atsMsg.isMute,
			atsMsg.isLocalisationValid
			);

        // dont act on it if this agent isnt in control
        if (m_operationModeManager.isRunningControl() == false)
		{
		    FUNCTION_EXIT;
		    return;
		}


		if(newTrain || oldCctvZone!=atsMsg.currentCctvZone || oldLocation!=atsMsg.currentLocation)
		{
			m_trainStatusUpdateMonitor.onCctvZoneUpdate(atsMsg);
		}

        switch (m_trainAgent.getAgentLocationType())
        {
        case TrainTypes::LOC_OCC:
            {
                // if this agent is in control of the trains area
                if ( m_operationModeManager.isInControlOfLocation(atsMsg.currentLocation) )
                {
					// if this train is new or has moved from a location this agent is not in control of
                    if ( newTrain || !m_operationModeManager.isInControlOfLocation(oldLocation) )
                    {
                        // check if this train has come from the depot
                        bool changeArea = ( m_operationModeManager.getLocationType(oldLocation) == TrainTypes::LOC_DEPOT );

                        if ( true == atsMsg.isLocalisationValid )
                        {

                            // take control of the train
                            TrainMiscChangeModeCommand* command = new TrainMiscChangeModeCommand(
                                m_trainAgent.getTrainMisc(),
                                atsMsg.trainID,
                                TrainMisc::Radio_Normal,
                                "",
							    changeArea,
                                miscChangeAreaCommand::MAINLINE );

				            m_workQueueManager.perform( command );
                        }
                        
                        // monitor the train for OA1/ATC activity
                        m_trainActivityMonitor.addTrain(atsMsg.trainID);

						{
							std::ostringstream s;

							s << "At the OCC, taking control of train on ATS event [trainID=" << atsMsg.trainID << "] ";
							LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, s.str().c_str());
						}
                        
                        if ( true == changeArea )
                        {
                            m_trainAgent.defaultTrainCCTV( atsMsg.trainID );
                        }
                    }

                    // if this train was previously controlled by this agent, but is not talking
                    // then try to take control of the train again
                    if ( ( m_trainManager.getTrainStatusManager().getOa1AlarmId(atsMsg.trainID).compare("") != 0 ) ||
                         ( m_trainManager.getTrainStatusManager().getAtcAlarmId(atsMsg.trainID).compare("") != 0 ) )
                    {
                        if ( true == atsMsg.isLocalisationValid )
                        {
						    // check if this train has come from the depot
                            bool changeArea = ( m_operationModeManager.getLocationType(oldLocation) == TrainTypes::LOC_DEPOT );

                            // take control of the train
                            TrainMiscChangeModeCommand * command = new TrainMiscChangeModeCommand(
                                m_trainAgent.getTrainMisc(),
                                atsMsg.trainID,
                                TrainMisc::Radio_Normal,
                                "",
							    changeArea,
                                miscChangeAreaCommand::MAINLINE );

				            m_workQueueManager.perform( command );
                        }
                    }
                }
                else // not in control of the train's area
                {
                    // if this agent was previously in control of the train
                    if ( m_operationModeManager.isInControlOfLocation(oldLocation) )
                    {
                        // release all control of the train
                        m_trainActivityMonitor.removeTrain(atsMsg.trainID);
                        m_trainAgent.getTrainMisc().removeTrain(atsMsg.trainID);
                        m_trainAgent.getTrainPEC().invalidatePec(atsMsg.trainID);
                    }
                }
            }
            break;

        case TrainTypes::LOC_STATION:
            {
                // if the train is in this agents location
                if ( atsMsg.currentLocation == agentLocation )
                {
                    // if this agent is in control of this location
                    if ( m_operationModeManager.isInControlOfLocation(agentLocation) )
                    {
                        // if this train is new
                        // otherwise if this train or has moved from another location
                        if ( newTrain || (atsMsg.currentLocation != oldLocation) )
                        {
                            // check if this train has come from the depot
                            bool changeArea = ( m_operationModeManager.getLocationType(oldLocation) == TrainTypes::LOC_DEPOT );

                            if ( true == atsMsg.isLocalisationValid )
                            {
							    // take control of the train
							    TrainMiscChangeModeCommand * command = new TrainMiscChangeModeCommand(
								    m_trainAgent.getTrainMisc(),
                                    atsMsg.trainID,
                                    TrainMisc::Radio_Degraded,
                                    "",
								    changeArea,
                                    miscChangeAreaCommand::MAINLINE );

				                m_workQueueManager.perform( command );
                            }

                            // monitor the train for OA1/ATC activity
                            m_trainActivityMonitor.addTrain(atsMsg.trainID);

							{
								std::ostringstream s;

								s << "At a STATION, taking control of train on ATS event [trainID=" << atsMsg.trainID << "] ";
								LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, s.str().c_str());
							}

                            if ( true == changeArea )
                            {
                                m_trainAgent.defaultTrainCCTV( atsMsg.trainID );
                            }
                        }
                    }
                    else // not in control of the train
                    {
                        // release all control of the train
                        m_trainActivityMonitor.removeTrain(atsMsg.trainID);
                        m_trainAgent.getTrainMisc().removeTrain(atsMsg.trainID);
                        m_trainAgent.getTrainPEC().invalidatePec(atsMsg.trainID);
                    }
                }
                else
                {
                    // if it moved out of this location
                    if ( oldLocation == agentLocation )
                    {
                        // release all control of the train
                        m_trainActivityMonitor.removeTrain(atsMsg.trainID);
                        m_trainAgent.getTrainMisc().removeTrain(atsMsg.trainID);
                        m_trainAgent.getTrainPEC().invalidatePec(atsMsg.trainID);

                        // invalidate the train
                        m_trainManager.getTrainStatusManager().invalidateTrain(atsMsg.trainID);
                    }
                }
            }
            break;

        case TrainTypes::LOC_DEPOT:
            {
                // if the train has moved
                if ( atsMsg.currentLocation != oldLocation )
                {
                    // into the depot, or new
                    if ( newTrain || (atsMsg.currentLocation == agentLocation) )
                    {
                        // check if this train has come from the mainline
                        bool changeArea = ( m_operationModeManager.getLocationType(oldLocation) != TrainTypes::LOC_DEPOT );

                        if ( true == atsMsg.isLocalisationValid )
                        {
                            // take control of the train
                            TrainMiscChangeModeCommand * command = new TrainMiscChangeModeCommand(
                                m_trainAgent.getTrainMisc(),
                                atsMsg.trainID,
                                TrainMisc::Radio_Normal,
                                "",
							    changeArea,
                                miscChangeAreaCommand::DEPOT );

				            m_workQueueManager.perform( command );
                        }

                        // monitor the train for OA1/ATC activity
                        m_trainActivityMonitor.addTrain(atsMsg.trainID);

						{
							std::ostringstream s;

							s << "At the DEPOOT, taking control of train on ATS event [trainID=" << atsMsg.trainID << "] ";
							LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, s.str().c_str());
						}

                        if ( true == changeArea )
                        {
                            m_trainAgent.defaultTrainCCTV( atsMsg.trainID );
                        }
                    }
                    // out of the depot
                    else
                    {
                        // release all control of the train
                        m_trainActivityMonitor.removeTrain(atsMsg.trainID);
                        m_trainAgent.getTrainMisc().removeTrain(atsMsg.trainID);
                        m_trainAgent.getTrainPEC().invalidatePec(atsMsg.trainID);
                    }
                }
            }
            break;
        }

    }
	else
	{
		if (atsMsg.trainID != 0)
		{
			//TD14205
			//zhou yuan++
			try
			{
				// release all control of the train
				m_trainActivityMonitor.removeTrain(atsMsg.trainID);
				m_trainAgent.getTrainMisc().removeTrain(atsMsg.trainID);
				m_trainAgent.getTrainPEC().invalidatePec(atsMsg.trainID);

				// ATS is telling us the train is not valid - invalidate the train
				m_trainManager.getTrainStatusManager().invalidateTrain(atsMsg.trainID);
			}
			catch (...)
			{
				//do nothing, 
			}
			//++zhou yuan
		}
	}

	FUNCTION_EXIT;
}

void TrainATS::receive(TrainEvent& event)
{
    // a train event has been received

    unsigned char trainId = event.getTrainID();

//#define NEED_MISSING_TRAINS_TO_BE_SHOWN
#if NEED_MISSING_TRAINS_TO_BE_SHOWN
    // close the degraded radio mode alarm if it is open
    m_trainManager.getTrainStatusManager().setAtsValidityToUnknown(trainId);
#endif
}
