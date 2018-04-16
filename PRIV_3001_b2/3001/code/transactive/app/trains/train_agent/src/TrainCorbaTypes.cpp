/**
  * The source code in this file is the property of
  * MI Services Group and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/trains/train_agent/src/TrainCorbaTypes.cpp $
  * @author:  Glen Kidd
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  *
  */

#include <ostream>

#include "app/trains/train_agent/src/TrainCorbaTypes.h" 
#include "app/trains/train_agent/src/TrainStatusManager.h"
#include "bus/trains/TrainAgentCorbaDef/idl/src/ITrainCorbaDef.h"

#include "core/utilities/src/TAAssert.h"

using namespace TA_IRS_App;
using namespace TrainStatus;

namespace TA_IRS_App
{

	TrainTypes::Track convertATSTrack(TA_IRS_Bus::ETrack t)
	{
		FUNCTION_ENTRY("convertATSTrack");

		TrainTypes::Track track;
		switch (t)
		{
		case TA_IRS_Bus::TrackInnerbound:
			track = TrainTypes::Track_Inbound;
			break;
		case TA_IRS_Bus::TrackCentralInnerbound:
			track = TrainTypes::Track_Inbound;
			break;
		case TA_IRS_Bus::TrackOuterbound:
			track = TrainTypes::Track_Outbound;
			break;
		case TA_IRS_Bus::TrackCentralOuterbound:
			track = TrainTypes::Track_Outbound;
			break;
		default:
			track = TrainTypes::Track_Unknown;
		}
		return track;
	}

	ATSTrainMessage convertATSEvent(TA_IRS_Bus::IAtsTrainCorbaDef::AtsTrainMessageCorbaDef* x)
	{
		ATSTrainMessage result;
		result.trainID = x->physicalTrainNumber;
		result.isValid = x->trainValid;
		result.isLocalisationValid = x->localisation.localisationValid;
		result.currentLocation = x->localisation.currentLocation;
		result.currentCctvZone = x->localisation.currentCctvZone;
		result.upDirection = x->localisation.upDirection;
		result.serviceNumber = x->localisation.physicalTrainService;
		result.isMute = x->localisation.isMute;
		result.track = convertATSTrack(x->localisation.track);
		return result;
	}

	TA_Base_Bus::ITrainCorbaDef::TrainStateUpdateCollection convertTrainStateUpdate(std::vector<TA_IRS_App::TrainStatus::TrainInformation> & trainList)
	{
		using namespace TA_Base_Bus;
		ITrainCorbaDef::TrainStateUpdateCollection eventList;

		eventList.length(trainList.size());
		for (unsigned int i = 0; i != trainList.size(); ++i)
		{
	//		TrainStatus::TrainInformation & trainInfo = m_trainManager.getStatusManager().getTrainInformation(trainList[i]);

			ITrainCorbaDef::TrainStateUpdate event;
			event.trainID = trainList[i].trainID;
			
			event.sessionAllocation.length(trainList[i].sessionAllocation.size());
			{
				int count = 0;
				for (TrainStatus::SessionAllocationMap::iterator j = trainList[i].sessionAllocation.begin(); j != trainList[i].sessionAllocation.end(); ++j, ++count)
				{
					ITrainCorbaDef::ResourceCategory resource;
					switch ((*j).first)
					{
					case TrainStatus::Resource_CCTV:
						resource = ITrainCorbaDef::Resource_CCTV;
						break;
					case TrainStatus::Resource_CallDriver:
						resource = ITrainCorbaDef::Resource_CallDriver;
						break;
					case TrainStatus::Resource_PEC:
						resource = ITrainCorbaDef::Resource_PEC;
						break;
					case TrainStatus::Resource_PA:
						resource = ITrainCorbaDef::Resource_PA;
						break;
					case TrainStatus::Resource_TTIS:
						resource = ITrainCorbaDef::Resource_TTIS;
						break;
					default:
						// Assert("Invalid enumeration Value")
						;
					}
					event.sessionAllocation[count].resource = resource;
					event.sessionAllocation[count].value = CORBA::string_dup((*j).second.c_str());
				}
			}

			event.callAllocation.length(trainList[i].callAllocation.size());
			{
				int count = 0;
				for (TrainStatus::CallAllocationMap::iterator j = trainList[i].callAllocation.begin(); j != trainList[i].callAllocation.end(); ++j, ++count)
				{
					ITrainCorbaDef::ResourceCategory resource;
					switch ((*j).first)
					{
					case TrainStatus::Resource_CCTV:
						resource = ITrainCorbaDef::Resource_CCTV;
						break;
					case TrainStatus::Resource_CallDriver:
						resource = ITrainCorbaDef::Resource_CallDriver;
						break;
					case TrainStatus::Resource_PEC:
						resource = ITrainCorbaDef::Resource_PEC;
						break;
					case TrainStatus::Resource_PA:
						resource = ITrainCorbaDef::Resource_PA;
						break;
					case TrainStatus::Resource_TTIS:
						resource = ITrainCorbaDef::Resource_TTIS;
						break;
					default:
						// Assert("Invalid enumeration Value")
						;
					}
					event.callAllocation[count].resource = resource;
					std::ostringstream s;
					s << (*j).second << std::ends;
					event.callAllocation[count].value = CORBA::string_dup(s.str().c_str());
				}
			}

			event.autoContinueAck.length(trainList[i].autoContinueAck.size());
			{
				int count = 0;
				for (TrainStatus::CallAllocationMap::iterator j = trainList[i].callAllocation.begin(); j != trainList[i].callAllocation.end(); ++j, ++count)
				{
					ITrainCorbaDef::ResourceCategory resource;
					switch ((*j).first)
					{
					case TrainStatus::Resource_CCTV:
						resource = ITrainCorbaDef::Resource_CCTV;
						break;
					case TrainStatus::Resource_CallDriver:
						resource = ITrainCorbaDef::Resource_CallDriver;
						break;
					case TrainStatus::Resource_PEC:
						resource = ITrainCorbaDef::Resource_PEC;
						break;
					case TrainStatus::Resource_PA:
						resource = ITrainCorbaDef::Resource_PA;
						break;
					case TrainStatus::Resource_TTIS:
						resource = ITrainCorbaDef::Resource_TTIS;
						break;
					default:
						// Assert("Invalid enumeration Value")
						;
					}
					event.autoContinueAck[count].resource = resource;
					std::ostringstream s;
					s << static_cast<int>((*j).second) << std::ends;
					event.autoContinueAck[count].value = CORBA::string_dup(s.str().c_str());
				}
			}

			event.currentLocation = trainList[i].currentLocation;
			event.serviceNumber = CORBA::string_dup(trainList[i].serviceNumber.c_str());
			event.isMute = CORBA::Boolean(trainList[i].isMute);
			ITrainCorbaDef::ETrack track;
			switch (trainList[i].track)
			{
			case TrainTypes::Track_Inbound:
				track = ITrainCorbaDef::TrackInnerbound;
				break;
			case TrainTypes::Track_Outbound:
				track = ITrainCorbaDef::TrackOuterbound;
				break;
			case TrainTypes::Track_Unknown:
				track = ITrainCorbaDef::TrackUnknown;
				break;
			default:
				// Assert("Invalid enumeration Value")
				;
			}
			event.track = track;

			//event.isLocalisationValid;
			ITrainCorbaDef::EAtsValid atsValid;
			switch (trainList[i].atsValid)
			{
			case TrainTypes::AtsValid_Unknown:
				atsValid = ITrainCorbaDef::AtsValid_Unknown;
				break;
			case TrainTypes::AtsValid_Invalid:
				atsValid = ITrainCorbaDef::AtsValid_Invalid;
				break;
			case TrainTypes::AtsValid_Valid:
				atsValid = ITrainCorbaDef::AtsValid_Valid;
				break;
			default:
				// Assert("Invalid enumeration Value")
				;
			}
			event.atsValid = atsValid;

			event.trainTSI[0] = CORBA::string_dup(trainList[i].mpuTSI[0].c_str());
			event.trainTSI[1] = CORBA::string_dup(trainList[i].mpuTSI[1].c_str());
			event.activeTrainTSI = trainList[i].activeMPU;

			event.TTISLibraryVersion = trainList[i].TTISLibraryVersion;
			event.TTISScheduleVersion = trainList[i].TTISScheduleVersion;
			ITrainCorbaDef::ETTISStatus ttisStatus;
			switch (trainList[i].TTISStatus)
			{
			case TrainTypes::TTIS_Unknown:
				ttisStatus = ITrainCorbaDef::TTIS_Unknown;
				break;
			case TrainTypes::TTIS_Library:
				ttisStatus = ITrainCorbaDef::TTIS_Library;
				break;
			case TrainTypes::TTIS_Schedule:
				ttisStatus = ITrainCorbaDef::TTIS_Schedule;
				break;
			default:
				// Assert("Invalid enumeration Value")
				;
			}
			event.TTISStatus = ttisStatus;

			// skip library data

			event.PALibraryVersion = trainList[i].PALibraryVersion;
			event.PAAnnouncementID = trainList[i].PAAnnouncementID;

			ITrainCorbaDef::ETrainStatus trainStatus;
			switch (trainList[i].track)
			{
			case TrainTypes::IDLE:
				trainStatus = ITrainCorbaDef::IDLE;
				break;
			case TrainTypes::CAB_TO_CAB:
				trainStatus = ITrainCorbaDef::CAB_TO_CAB;
				break;
			case TrainTypes::DRIVER_PA:
				trainStatus = ITrainCorbaDef::DRIVER_PA;
				break;
			default:
				// Assert("Invalid enumeration Value")
				;
			}
			event.trainStatus = trainStatus;

			event.activePECPoints.length(trainList[i].activePECPoints.size());
			int activePointIdx = -1;
			{
				int count = 0;
				for (PECPointCollection::iterator j = trainList[i].activePECPoints.begin(); j != trainList[i].activePECPoints.end(); ++j, ++count)
				{
					ITrainCorbaDef::PECPoint point;
					point.car = (*j).car;
					point.PEC = (*j).PEC;
					ITrainCorbaDef::PECSource pecSource;
					switch ((*j).source)
					{
					case TrainTypes::PEC_UNASSIGNED:
						pecSource = ITrainCorbaDef::PEC_UNASSIGNED;
						break;
					case TrainTypes::PEC_OPERATOR:
						pecSource = ITrainCorbaDef::PEC_OPERATOR;
						break;
					case TrainTypes::PEC_DRIVER:
						pecSource = ITrainCorbaDef::PEC_DRIVER;
						break;
					default:
						// Assert("Invalid enumeration Value")
						;
					}
					point.source = pecSource;
					point.creationTime = (*j).creationTime;
                    point.validity = (*j).validity;
					event.activePECPoints[count] = point;
					
					if (activePointIdx == -1)
					{
						if (true == TrainStatusManager::comparePECPoint()(*j,trainList[i].connectedPECPoint))
						{
							activePointIdx = count;
						}
					}
				}
			}
			event.connectedPECPoint = activePointIdx;

			eventList[i] = event;
		}
		return eventList;
	}

	std::vector<TA_IRS_App::TrainStatus::TrainInformation> convertTrainStateUpdate(const class TA_Base_Bus::ITrainCorbaDef::TrainStateUpdateCollection& corbaList)
	{
		using namespace TA_IRS_Bus;
		using TA_Base_Bus::ITrainCorbaDef;

		std::vector<TA_IRS_App::TrainStatus::TrainInformation> updateList;

		for (unsigned int i = 0; i < corbaList.length(); ++i)
		{
			TA_IRS_App::TrainStatus::TrainInformation update;

			update.trainID = corbaList[i].trainID;

			for (unsigned int session = 0; session < corbaList[i].sessionAllocation.length(); ++session)
			{
				TrainStatus::ResourceCategory resource = static_cast<TrainStatus::ResourceCategory>(-1); 
				switch (corbaList[i].sessionAllocation[session].resource)
				{
				case ITrainCorbaDef::Resource_CCTV:
					resource = TrainStatus::Resource_CCTV;
					break;
				case ITrainCorbaDef::Resource_CallDriver:
					resource = TrainStatus::Resource_CallDriver;
					break;
				case ITrainCorbaDef::Resource_PEC:
					resource = TrainStatus::Resource_PEC;
					break;
				case ITrainCorbaDef::Resource_PA:
					resource = TrainStatus::Resource_PA;
					break;
				case ITrainCorbaDef::Resource_TTIS:
					resource = TrainStatus::Resource_TTIS;
					break;
				default:
						// Assert("Invalid enumeration Value")
						;

				}
				update.sessionAllocation[ resource ] = corbaList[i].sessionAllocation[session].value;
			}

			for (unsigned int call = 0; call < corbaList[i].callAllocation.length(); ++call)
			{
				TrainStatus::ResourceCategory resource = static_cast<TrainStatus::ResourceCategory>(-1); 
				switch (corbaList[i].callAllocation[call].resource)
				{
				case ITrainCorbaDef::Resource_CCTV:
					resource = TrainStatus::Resource_CCTV;
					break;
				case ITrainCorbaDef::Resource_CallDriver:
					resource = TrainStatus::Resource_CallDriver;
					break;
				case ITrainCorbaDef::Resource_PEC:
					resource = TrainStatus::Resource_PEC;
					break;
				case ITrainCorbaDef::Resource_PA:
					resource = TrainStatus::Resource_PA;
					break;
				case ITrainCorbaDef::Resource_TTIS:
					resource = TrainStatus::Resource_TTIS;
					break;
				default:
						// Assert("Invalid enumeration Value")
						;

				}
				std::stringstream s;
				s << corbaList[i].callAllocation[call].value << std::ends;
				unsigned long value;
				s >> value;
				update.callAllocation[ resource ] = value;
			}

			for (unsigned int ack = 0; ack < corbaList[i].autoContinueAck.length(); ++ack)
			{
				TrainStatus::ResourceCategory resource = static_cast<TrainStatus::ResourceCategory>(-1); 
				switch (corbaList[i].autoContinueAck[ack].resource)
				{
				case ITrainCorbaDef::Resource_CCTV:
					resource = TrainStatus::Resource_CCTV;
					break;
				case ITrainCorbaDef::Resource_CallDriver:
					resource = TrainStatus::Resource_CallDriver;
					break;
				case ITrainCorbaDef::Resource_PEC:
					resource = TrainStatus::Resource_PEC;
					break;
				case ITrainCorbaDef::Resource_PA:
					resource = TrainStatus::Resource_PA;
					break;
				case ITrainCorbaDef::Resource_TTIS:
					resource = TrainStatus::Resource_TTIS;
					break;
				default:
						// Assert("Invalid enumeration Value")
						;

				}
				std::stringstream s;
				s << corbaList[i].autoContinueAck[ack].value << std::ends;
				unsigned char value;
				s >> value;
				update.callAllocation[ resource ] = value;
			}

			update.currentLocation = corbaList[i].currentLocation;
			update.serviceNumber = corbaList[i].serviceNumber;
			update.isMute = corbaList[i].isMute;
			
			TrainTypes::Track track = static_cast<TrainTypes::Track>(-1);
			switch (corbaList[i].track)
			{
			case ITrainCorbaDef::TrackInnerbound:
				track = TrainTypes::Track_Inbound;
				break;
			case ITrainCorbaDef::TrackOuterbound:
				track = TrainTypes::Track_Outbound;
				break;
			case ITrainCorbaDef::TrackUnknown:
				track = TrainTypes::Track_Unknown;
				break;
			default:
				// Assert("Invalid enumeration Value")
				;
			}
			update.track = track;

			update.isLocalisationValid; /// @todo add localisation flag to idl

			TrainTypes::AtsValid atsValid = static_cast<TrainTypes::AtsValid>(-1);
			switch (corbaList[i].atsValid)
			{
			case ITrainCorbaDef::AtsValid_Invalid:
				atsValid = TrainTypes::AtsValid_Invalid;
				break;
			case ITrainCorbaDef::AtsValid_Valid:
				atsValid = TrainTypes::AtsValid_Valid;
				break;
			case ITrainCorbaDef::AtsValid_Unknown:
				atsValid = TrainTypes::AtsValid_Unknown;
				break;
			default:
				// Assert("Invalid enumeration Value")
				;
			}
			update.atsValid = atsValid;


			update.mpuTSI[0] = corbaList[i].trainTSI[0];
			update.mpuTSI[1] = corbaList[i].trainTSI[1];
			update.activeMPU = static_cast<TrainTypes::MPUState>(corbaList[i].activeTrainTSI);

			update.TTISLibraryVersion = corbaList[i].TTISLibraryVersion;
			update.TTISScheduleVersion = corbaList[i].TTISScheduleVersion;

			TrainTypes::TTISStatus ttis = static_cast<TrainTypes::TTISStatus>(-1);
			switch(corbaList[i].TTISStatus)
			{
			case ITrainCorbaDef::TTIS_Unknown:
				ttis = TrainTypes::TTIS_Unknown;
				break;
			case ITrainCorbaDef::TTIS_Library:
				ttis = TrainTypes::TTIS_Library;
				break;
			case ITrainCorbaDef::TTIS_Schedule:
				ttis = TrainTypes::TTIS_Schedule;
				break;
			default:
				// Assert("Invalid enumeration Value")
				;
			}
			update.TTISStatus = ttis;

			// skip library data

			update.PALibraryVersion = corbaList[i].PALibraryVersion;
			update.PAAnnouncementID = corbaList[i].PAAnnouncementID;

			TrainTypes::TrainStatus trainStatus;
			switch (corbaList[i].track)
			{
			case ITrainCorbaDef::IDLE:
				trainStatus = TrainTypes::IDLE;
				break;
			case ITrainCorbaDef::CAB_TO_CAB:
				trainStatus = TrainTypes::CAB_TO_CAB;
				break;
			case ITrainCorbaDef::DRIVER_PA:
				trainStatus = TrainTypes::DRIVER_PA;
				break;
			default:
				// Assert("Invalid enumeration Value")
				;
			}
			update.trainStatus = trainStatus;

			for (unsigned int pec = 0; pec < corbaList[i].activePECPoints.length(); ++pec)
			{
				PECPoint point;
				point.car = corbaList[i].activePECPoints[pec].car;
				point.PEC = corbaList[i].activePECPoints[pec].PEC;

				TrainTypes::PECSource pecSource = static_cast<TrainTypes::PECSource>(-1);
				switch (corbaList[i].activePECPoints[pec].source)
				{
				case ITrainCorbaDef::PEC_UNASSIGNED:
					pecSource = TrainTypes::PEC_UNASSIGNED;
					break;
				case ITrainCorbaDef::PEC_OPERATOR:
					pecSource = TrainTypes::PEC_OPERATOR;
					break;
				case ITrainCorbaDef::PEC_DRIVER:
					pecSource = TrainTypes::PEC_DRIVER;
					break;
				default:
					// Assert("Invalid enumeration Value")
					;
				}
				point.source = pecSource;
				point.creationTime = corbaList[i].activePECPoints[pec].creationTime;
                point.validity = corbaList[i].activePECPoints[pec].validity;
				
				update.activePECPoints.push_back(point);
				
				if (pec == corbaList[i].connectedPECPoint )
				{
					update.connectedPECPoint = point;
				}
			}

			update.syncState = TrainStatus::syncNOP;

			updateList.push_back(update);
		}

		return updateList;
	}

	DutyTrainMessage convertDutyEvent(TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemState* x)
	{
		DutyTrainMessage dutyTrainMessage;

		/*******************************************************************
		* addState and remState originated from an "internal" header file:
		* bus\security\duty_agent\duty_agent_state\src\SubsystemState.h
		* Reomve the cast (TrainTypes::DutyState) and put in a switch state 
		* when the original becomes available.
		********************************************************************/
		dutyTrainMessage.addState = (TrainTypes::DutyState)x->currState;
		dutyTrainMessage.remState = (TrainTypes::DutyState)x->prevState;
        dutyTrainMessage.subsystemKey = x->subsystemKey;
        // ss++
        // TD12082
        dutyTrainMessage.locationKey = x->regionKey;
		// ++ss

		return dutyTrainMessage;
	}

    // ss++ Commented for now until duty agent is fixed
//    DutyTrainMessage convertDutyEventIncomplete(TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemDuty* x)
//	{
//		DutyTrainMessage dutyTrainMessage;
//
//        // these are incomplete and must be queried
//		dutyTrainMessage.addState = TrainTypes::SS_NORMAL;
//		dutyTrainMessage.remState = TrainTypes::SS_NORMAL;
//
//        // these are complete
//        dutyTrainMessage.subsystemKey = x->subsystemKey;
//        dutyTrainMessage.locationKey = x->regionKey;
//
//		return dutyTrainMessage;
//	}
	// ++ss

	PECPoint convertPECPoint(const TA_Base_Bus::ITrainCorbaDef::PECPoint& corbaPECPoint)
	{
		FUNCTION_ENTRY("convertPECPoint");
		PECPoint trainPECPoint;

		trainPECPoint.car = corbaPECPoint.car;
		trainPECPoint.PEC = corbaPECPoint.PEC;
		switch(corbaPECPoint.source)
		{
			case TA_Base_Bus::ITrainCorbaDef::PEC_UNASSIGNED:
			{
				trainPECPoint.source = TrainTypes::PEC_UNASSIGNED;
				break;
			}
			case TrainTypes::PEC_DRIVER:
			{
				trainPECPoint.source = TrainTypes::PEC_DRIVER;
				break;
			}
			case TrainTypes::PEC_OPERATOR:
			{
				trainPECPoint.source = TrainTypes::PEC_OPERATOR;
				break;
			}
			default:
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugWarn, "PEC point source in invalid"); 
				break;
			}
		}
		trainPECPoint.creationTime = corbaPECPoint.creationTime;
        trainPECPoint.validity = corbaPECPoint.validity;
		FUNCTION_EXIT;
		return trainPECPoint;
	}

	TA_Base_Bus::ITrainCorbaDef::PECPoint convertPECPoint(
		const PECPoint& trainPECPoint)
	{
		FUNCTION_ENTRY("convertPECPoint");
		TA_Base_Bus::ITrainCorbaDef::PECPoint corbaPECPoint;

		corbaPECPoint.car = trainPECPoint.car;
		corbaPECPoint.PEC = trainPECPoint.PEC;
		switch(trainPECPoint.source)
		{
			case TrainTypes::PEC_UNASSIGNED:
			{
				corbaPECPoint.source = TA_Base_Bus::ITrainCorbaDef::PEC_UNASSIGNED;
				break;
			}
			case TrainTypes::PEC_DRIVER:
			{
				corbaPECPoint.source = TA_Base_Bus::ITrainCorbaDef::PEC_DRIVER;
				break;
			}
			case TrainTypes::PEC_OPERATOR:
			{
				corbaPECPoint.source = TA_Base_Bus::ITrainCorbaDef::PEC_OPERATOR;
				break;
			}
			default:
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugWarn, "PEC point source in invalid"); 
				break;
			}
		}
		corbaPECPoint.creationTime = trainPECPoint.creationTime;
        corbaPECPoint.validity = trainPECPoint.validity;
		FUNCTION_EXIT;
		return corbaPECPoint;
	}


	TA_Base_Bus::ITrainCorbaDef::TrainStateUpdateCallDriver convertCallDriver(
		const TrainStatus::TrainInformation& trainInformation)
	{
		FUNCTION_ENTRY("convertCallDriver");
		TA_Base_Bus::ITrainCorbaDef::TrainStateUpdateCallDriver event;
		std::string sessionID;
		unsigned long callID = 0;

		event.trainID = trainInformation.trainID;
		switch(trainInformation.trainStatus)
		{
			case TrainTypes::IDLE:
			{
				event.trainStatus = TA_Base_Bus::ITrainCorbaDef::IDLE;
				break;
			}
			case TrainTypes::CAB_TO_CAB:
			{
				event.trainStatus = TA_Base_Bus::ITrainCorbaDef::CAB_TO_CAB;
				break;
			}
			case TrainTypes::DRIVER_PA:
			{
				event.trainStatus = TA_Base_Bus::ITrainCorbaDef::DRIVER_PA;
				break;
			}
			default:
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugWarn, "PEC point source in invalid"); 
				break;
			}

		}		
		sessionID = getResourceSessionID(trainInformation, Resource_CCTV);
		event.sessionID = sessionID.c_str();
		callID = getResourceCallID(trainInformation, Resource_CCTV);
		event.callID = callID;

		FUNCTION_EXIT;
		return event;
	}

	TA_IRS_App::TrainStatus::TrainInformation convertCallDriver(
		const TA_Base_Bus::ITrainCorbaDef::TrainStateUpdateCallDriver& message)
	{
		FUNCTION_ENTRY("convertCallDriver");
		TA_IRS_App::TrainStatus::TrainInformation trainInformation;

		trainInformation.trainID =  message.trainID;
		switch(message.trainStatus)
		{
			case TA_Base_Bus::ITrainCorbaDef::IDLE:
			{
				trainInformation.trainStatus = TrainTypes::IDLE;
				break;
			}
			case TA_Base_Bus::ITrainCorbaDef::CAB_TO_CAB:
			{
				trainInformation.trainStatus = TrainTypes::CAB_TO_CAB;
				break;
			}
			case TA_Base_Bus::ITrainCorbaDef::DRIVER_PA:
			{
				trainInformation.trainStatus = TrainTypes::DRIVER_PA;
				break;
			}
			default:
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugWarn, "Uknown sychronisation CallDrive state"); 
				break;
			}
		}
		trainInformation.sessionAllocation[Resource_CallDriver] = message.sessionID;
		trainInformation.callAllocation[Resource_CallDriver] = message.callID;
		trainInformation.syncState = syncCallDriver;

		FUNCTION_EXIT;
		return trainInformation;
	}


	TA_Base_Bus::ITrainCorbaDef::TrainStateUpdateCctv convertCctv(
		const TrainStatus::TrainInformation& trainInformation)
	{
		FUNCTION_ENTRY("convertCctv");

		TA_Base_Bus::ITrainCorbaDef::TrainStateUpdateCctv event;
		std::string sessionID;
		unsigned long callID = 0;

		event.trainID =  trainInformation.trainID;
		event.cctvActivated = trainInformation.cctvActivated;
		event.cctvInitialised = trainInformation.cctvInitialised;
		event.cctvPreConflict = trainInformation.cctvPreConflict;
		event.preConflictAlarmId = trainInformation.preConflictAlarmId.c_str();
		switch(trainInformation.cctvSwitchState.mode)
		{
			case TrainStatus::cam:
			{
				event.switchState.mode = TA_Base_Bus::ITrainCorbaDef::cam;
				break;
			}
			case TA_Base_Bus::ITrainCorbaDef::seq:
			{
				event.switchState.mode = TA_Base_Bus::ITrainCorbaDef::seq;
				break;
			}
			case TA_Base_Bus::ITrainCorbaDef::quad:
			{
				event.switchState.mode = TA_Base_Bus::ITrainCorbaDef::quad;
				break;
			}
			default:
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugError, "Uknown sychronisation Cctv mode state"); 
				break;
			}
		}
		event.switchState.item1 = trainInformation.cctvSwitchState.item1;
		event.switchState.item2 = trainInformation.cctvSwitchState.item2;
		event.switchState.item3 = trainInformation.cctvSwitchState.item3;
		event.switchState.item4 = trainInformation.cctvSwitchState.item4;
		sessionID = getResourceSessionID(trainInformation, Resource_CCTV);
		event.sessionID = sessionID.c_str();
		callID = getResourceCallID(trainInformation, Resource_CCTV);
		event.callID = callID;

		FUNCTION_EXIT;
		return event;
	}

	TA_IRS_App::TrainStatus::TrainInformation convertCctv(
		const TA_Base_Bus::ITrainCorbaDef::TrainStateUpdateCctv& message )
	{
		FUNCTION_ENTRY("convertCctv");

		TA_IRS_App::TrainStatus::TrainInformation trainInformation;
		std::vector<TA_IRS_App::TrainStatus::TrainInformation> stateUpdate;

		trainInformation.trainID =  message.trainID;
		trainInformation.cctvActivated = message.cctvActivated;
		trainInformation.cctvInitialised = message.cctvInitialised;
		trainInformation.cctvPreConflict = message.cctvPreConflict;
		trainInformation.preConflictAlarmId = message.preConflictAlarmId;
		switch(message.switchState.mode)
		{
			case TA_Base_Bus::ITrainCorbaDef::cam:
			{
				trainInformation.cctvSwitchState.mode = TrainStatus::cam;
				break;
			}
			case TA_Base_Bus::ITrainCorbaDef::seq:
			{
				trainInformation.cctvSwitchState.mode = TrainStatus::seq;
				break;
			}
			case TA_Base_Bus::ITrainCorbaDef::quad:
			{
				trainInformation.cctvSwitchState.mode = TrainStatus::quad;
				break;
			}
			default:
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugWarn, "Uknown sychronisation Cctv mode state"); 
				break;
			}
		}
		trainInformation.cctvSwitchState.item1 = message.switchState.item1;
		trainInformation.cctvSwitchState.item2 = message.switchState.item2;
		trainInformation.cctvSwitchState.item3 = message.switchState.item3;
		trainInformation.cctvSwitchState.item4 = message.switchState.item4;
		trainInformation.sessionAllocation[Resource_CCTV] = message.sessionID;
		trainInformation.callAllocation[Resource_CCTV] = message.callID;
		trainInformation.syncState = syncCctv;

		FUNCTION_EXIT;
		return trainInformation;
	}


	TA_Base_Bus::ITrainCorbaDef::TrainStateUpdateLibVers convertLibVers(
		const TrainStatus::TrainInformation& trainInformation)
	{
		FUNCTION_ENTRY("convertLibVers");
		TA_Base_Bus::ITrainCorbaDef::TrainStateUpdateLibVers event;

		event.trainID = trainInformation.trainID;
		event.TTISLibraryVersion = trainInformation.TTISLibraryVersion;
		event.PALibraryVersion = trainInformation.PALibraryVersion;
		event.TTISScheduleVersion = trainInformation.TTISScheduleVersion;

		FUNCTION_EXIT;
		return event;
	}

	TA_IRS_App::TrainStatus::TrainInformation convertLibVers(
		const TA_Base_Bus::ITrainCorbaDef::TrainStateUpdateLibVers& message )
	{
		FUNCTION_ENTRY("convertLibVers");

		TA_IRS_App::TrainStatus::TrainInformation trainInformation;

		trainInformation.trainID =  message.trainID;
		trainInformation.TTISLibraryVersion =  message.TTISLibraryVersion;
		trainInformation.PALibraryVersion =  message.PALibraryVersion;
		trainInformation.TTISScheduleVersion =  message.TTISScheduleVersion;
		trainInformation.syncState = syncLibraryVersions;

		FUNCTION_EXIT;
		return trainInformation;
	}

	TA_Base_Bus::ITrainCorbaDef::TrainStateUpdatePA convertPA(
		const TrainStatus::TrainInformation& trainInformation)
	{
		FUNCTION_ENTRY("convertPA");

		TA_Base_Bus::ITrainCorbaDef::TrainStateUpdatePA event;
		std::string sessionID;
		unsigned long callID = 0;

		event.trainID = trainInformation.trainID;
		event.PAAnnouncementID = trainInformation.PAAnnouncementID;
		sessionID = getResourceSessionID(trainInformation, Resource_CCTV);
		event.sessionID = sessionID.c_str();
		callID = getResourceCallID(trainInformation, Resource_CCTV);
		event.callID = callID;

		FUNCTION_EXIT;
		return event;
	}

	TA_IRS_App::TrainStatus::TrainInformation convertPA(
		const TA_Base_Bus::ITrainCorbaDef::TrainStateUpdatePA& message )
	{
		FUNCTION_ENTRY("convertPA");

		TA_IRS_App::TrainStatus::TrainInformation trainInformation;

		trainInformation.trainID =  message.trainID;
		trainInformation.PAAnnouncementID =  (unsigned char)message.PAAnnouncementID;
		trainInformation.sessionAllocation[Resource_PA] = message.sessionID;
		trainInformation.callAllocation[Resource_PA] = message.callID;
		trainInformation.syncState = syncPublicAddress;

		FUNCTION_EXIT;
		return trainInformation;
	}


	TA_Base_Bus::ITrainCorbaDef::TrainStateUpdatePEC convertPEC(
		const TrainStatus::TrainInformation& trainInformation)
	{
		FUNCTION_ENTRY("convertPEC");

		TA_Base_Bus::ITrainCorbaDef::PECPoint corbaPECPoint;
		TA_Base_Bus::ITrainCorbaDef::TrainStateUpdatePEC event;
		std::string sessionID;
		unsigned long callID = 0;
		bool find = false;

		event.trainID = trainInformation.trainID;
		event.activePECPoints.length(trainInformation.activePECPoints.size());
		for(unsigned int i = 0; i < trainInformation.activePECPoints.size(); ++i)
		{
			corbaPECPoint = convertPECPoint(trainInformation.activePECPoints[i]);
			event.activePECPoints[i] = corbaPECPoint;
		}
		find = findConnectedPECPoint(trainInformation.connectedPECPoint, trainInformation.activePECPoints, event.connectedPECPoint);
		if(find == true)
		{
			sessionID = getResourceSessionID(trainInformation, Resource_CCTV);
			event.sessionID = sessionID.c_str();
			callID = getResourceCallID(trainInformation, Resource_CCTV);
			event.callID = callID;
		}

		return event;
		FUNCTION_EXIT;
	}

	TA_IRS_App::TrainStatus::TrainInformation convertPEC(
		const TA_Base_Bus::ITrainCorbaDef::TrainStateUpdatePEC& message )
	{
		FUNCTION_ENTRY("convertPEC");

		TA_IRS_App::TrainStatus::TrainInformation trainInformation;
		PECPoint trainPECPoint;

		trainInformation.trainID =  message.trainID;
		trainInformation.activePECPoints.clear();
		for(unsigned int i = 0; i < message.activePECPoints.length(); i++)
		{
			trainPECPoint = convertPECPoint(message.activePECPoints[i]);
			trainInformation.activePECPoints.push_back(trainPECPoint);
		}
		if((message.activePECPoints.length() > 0) && (message.connectedPECPoint < message.activePECPoints.length()))
		{
			trainInformation.connectedPECPoint= convertPECPoint(message.activePECPoints[message.connectedPECPoint]);
		}
		trainInformation.sessionAllocation[Resource_PEC] = message.sessionID;
		trainInformation.callAllocation[Resource_PEC] = message.callID;
		trainInformation.syncState = syncPassengerEmergencyCall;

		return trainInformation;
		FUNCTION_EXIT;
	}

	TA_Base_Bus::ITrainCorbaDef::TrainStateUpdateTSI convertTSI(
		const TrainStatus::TrainInformation& trainInformation)
	{
		FUNCTION_ENTRY("convertTSI");

		TA_Base_Bus::ITrainCorbaDef::TrainStateUpdateTSI event;

		event.trainID = trainInformation.trainID;
		event.trainTSI[0] = trainInformation.mpuTSI[0].c_str();
		event.trainTSI[1] = trainInformation.mpuTSI[1].c_str();
		event.activeTrainTSI = trainInformation.activeMPU;

		FUNCTION_EXIT;
		return event;
	}

	TA_IRS_App::TrainStatus::TrainInformation convertTSI(
		const TA_Base_Bus::ITrainCorbaDef::TrainStateUpdateTSI& message )
	{
		FUNCTION_ENTRY("convertTSI");

		TA_IRS_App::TrainStatus::TrainInformation trainInformation;

		trainInformation.trainID =  message.trainID;
		trainInformation.mpuTSI[0] = message.trainTSI[0];
		trainInformation.mpuTSI[1] = message.trainTSI[1];
		trainInformation.activeMPU = message.activeTrainTSI;
		trainInformation.syncState = syncTSI;

		FUNCTION_EXIT;
		return trainInformation;
	}


	//
	// getResourceSessionID
	//
	std::string getResourceSessionID(const TrainStatus::TrainInformation& trainInformation, TrainStatus::ResourceCategory resource)
	{
		FUNCTION_ENTRY("getResourceSessionID");

		std::string sessionID;
		SessionAllocationMap::const_iterator session = trainInformation.sessionAllocation.find(resource);
		if(session != trainInformation.sessionAllocation.end())

	//
	// getResourceSessionID
	//
		{
			sessionID = session->second;
		}

		FUNCTION_EXIT;
		return sessionID;
	}

	//
	// getResourceCallID
	//
	unsigned long getResourceCallID(const TrainStatus::TrainInformation& trainInformation, TrainStatus::ResourceCategory resource)
	{
		FUNCTION_ENTRY("getResourceCallID");

		unsigned long callID = 0;
		CallAllocationMap::const_iterator call = trainInformation.callAllocation.find(resource);
		if(call != trainInformation.callAllocation.end())
		{
			callID = call->second;
		}

		FUNCTION_EXIT;
		return callID;
	}

	bool findConnectedPECPoint(const PECPoint& trainPECPoint,
		const PECPointCollection& activePECPoints, unsigned short& connectedPECPoint)
	{
		FUNCTION_ENTRY("findConnectedPECPoint");
		bool found = false;

		for(unsigned int i; i < activePECPoints.size(); ++i)
		{
			if((activePECPoints[i].car == trainPECPoint.car) && (activePECPoints[i].PEC == trainPECPoint.PEC))
			{
				connectedPECPoint = i;
				found = true;
			}
		}

		FUNCTION_EXIT;
		return found;
	}

	TA_Base_Bus::ITrainCorbaDef::ETrainStatus convertTrainStatus(
		const TrainTypes::TrainStatus& trainStatus)
	{
		FUNCTION_ENTRY("convertTrainStatus");
		TA_Base_Bus::ITrainCorbaDef::ETrainStatus corbaTrainStatus;

		switch(trainStatus)
		{
			case TrainTypes::IDLE:
			{
				corbaTrainStatus = TA_Base_Bus::ITrainCorbaDef::IDLE;
				break;
			}
			case TrainTypes::CAB_TO_CAB:
			{
				corbaTrainStatus = TA_Base_Bus::ITrainCorbaDef::IDLE;
				break;
			}
			case TrainTypes::DRIVER_PA:
			{
				corbaTrainStatus = TA_Base_Bus::ITrainCorbaDef::IDLE;
				break;
			}
			default:
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugError, "Uknown CallDrive state"); 
				break;
			}
		}

		FUNCTION_EXIT;
		return corbaTrainStatus;
	}

	TrainTypes::TrainStatus convertTrainStatus(
		const TA_Base_Bus::ITrainCorbaDef::ETrainStatus& corbaTrainStatus)
	{
		FUNCTION_ENTRY("convertTrainStatus");
		TrainTypes::TrainStatus trainStatus;

		switch(corbaTrainStatus)
		{
			case TA_Base_Bus::ITrainCorbaDef::IDLE:
			{
				trainStatus = TrainTypes::IDLE;
				break;
			}
			case TA_Base_Bus::ITrainCorbaDef::CAB_TO_CAB:
			{
				trainStatus = TrainTypes::CAB_TO_CAB;
				break;
			}
			case TA_Base_Bus::ITrainCorbaDef::DRIVER_PA:
			{
				trainStatus = TrainTypes::DRIVER_PA;
				break;
			}
			default:
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugWarn, "Uknown sychronisation CallDrive state"); 
				break;
			}
		}

		FUNCTION_EXIT;
		return trainStatus;
	}

	TA_Base_Bus::ITrainCorbaDef::CctvSwitchState convertCctvSwitchState(
		const TrainStatus::CctvSwitchState& cctvSwitchState)
	{
		FUNCTION_ENTRY("convertCctvSwitchState");
		TA_Base_Bus::ITrainCorbaDef::CctvSwitchState corbaCctvSwitchState;

		corbaCctvSwitchState.item1 = cctvSwitchState.item1;
		corbaCctvSwitchState.item2 = cctvSwitchState.item2;
		corbaCctvSwitchState.item3 = cctvSwitchState.item3;
		corbaCctvSwitchState.item4 = cctvSwitchState.item4;
		switch(cctvSwitchState.mode)
		{
			case TrainStatus::cam:
			{
				corbaCctvSwitchState.mode = TA_Base_Bus::ITrainCorbaDef::cam;
				break;
			}
			case TA_Base_Bus::ITrainCorbaDef::seq:
			{
				corbaCctvSwitchState.mode = TA_Base_Bus::ITrainCorbaDef::seq;
				break;
			}
			case TA_Base_Bus::ITrainCorbaDef::quad:
			{
				corbaCctvSwitchState.mode = TA_Base_Bus::ITrainCorbaDef::quad;
				break;
			}
			default:
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugError, "Uknown Cctv mode state"); 
				break;
			}
		}

		FUNCTION_EXIT;
		return corbaCctvSwitchState;
	}

	TrainStatus::CctvSwitchState convertCctvSwitchState(
		const TA_Base_Bus::ITrainCorbaDef::CctvSwitchState& corbaCctvSwitchState)
	{
		FUNCTION_ENTRY("convertCctvSwitchState");
		TrainStatus::CctvSwitchState cctvSwitchState;

		cctvSwitchState.item1 = corbaCctvSwitchState.item1;
		cctvSwitchState.item2 = corbaCctvSwitchState.item2;
		cctvSwitchState.item3 = corbaCctvSwitchState.item3;
		cctvSwitchState.item4 = corbaCctvSwitchState.item4;
		switch(corbaCctvSwitchState.mode)
		{
			case TA_Base_Bus::ITrainCorbaDef::cam:
			{
				cctvSwitchState.mode = TrainStatus::cam;
				break;
			}
			case TA_Base_Bus::ITrainCorbaDef::seq:
			{
				cctvSwitchState.mode = TrainStatus::seq;
				break;
			}
			case TA_Base_Bus::ITrainCorbaDef::quad:
			{
				cctvSwitchState.mode = TrainStatus::quad;
				break;
			}
			default:
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugError, "Uknown Cctv mode state"); 
				break;
			}
		}

		FUNCTION_EXIT;
		return cctvSwitchState;
	}

	TA_Base_Bus::ITrainCorbaDef::TrainStateUpdateAlarmIDs convertAlarmIDs(
		const TrainStatus::TrainInformation& trainInformation)
	{
		FUNCTION_ENTRY("convertAlarmIDs");
		TA_Base_Bus::ITrainCorbaDef::TrainStateUpdateAlarmIDs corbaTrainStateUpdateAlarmIDs;

		corbaTrainStateUpdateAlarmIDs.trainID = trainInformation.trainID;
		corbaTrainStateUpdateAlarmIDs.atcAlarmID = trainInformation.atcAlarmId.c_str();
		corbaTrainStateUpdateAlarmIDs.oa1AlarmID = trainInformation.oa1AlarmId.c_str();
        corbaTrainStateUpdateAlarmIDs.radioModeAlarmID = trainInformation.radioModeAlarmID.c_str();

		FUNCTION_EXIT;
		return corbaTrainStateUpdateAlarmIDs;
	}

	TrainStatus::TrainInformation convertAlarmIDs(
		const TA_Base_Bus::ITrainCorbaDef::TrainStateUpdateAlarmIDs& trainStateUpdateAlarmIDs)
	{
		FUNCTION_ENTRY("convertAlarmIDs");
		TrainStatus::TrainInformation trainInformation;

		trainInformation.trainID = trainStateUpdateAlarmIDs.trainID;
		trainInformation.atcAlarmId = trainStateUpdateAlarmIDs.atcAlarmID;
		trainInformation.oa1AlarmId = trainStateUpdateAlarmIDs.oa1AlarmID;
        trainInformation.radioModeAlarmID = trainStateUpdateAlarmIDs.radioModeAlarmID;
		trainInformation.syncState = syncAlarmIDs;

		FUNCTION_EXIT;
		return trainInformation;
	}

	TrainOperationMode convertOperationMode(
		const TA_Base_Bus::ITrainCorbaDef::TrainStateUpdateOperationMode& trainStateUpdateOperationMode)
	{
		FUNCTION_ENTRY("convertOperationMode");

		TrainOperationMode trainOperationMode;
		unsigned int index;

		trainOperationMode.doubleAtsFailure = trainStateUpdateOperationMode.doubleAtsFailure;
		trainOperationMode.fallbackMode = trainStateUpdateOperationMode.fallbackMode;
		trainOperationMode.groupOffline = trainStateUpdateOperationMode.groupOffline;
		trainOperationMode.localDuty = trainStateUpdateOperationMode.localDuty;

		for(index = 0; index < trainStateUpdateOperationMode.controlledLocations.length(); ++index)
		{
			trainOperationMode.controlledLocations.push_back(trainStateUpdateOperationMode.controlledLocations[index]);
		}

		FUNCTION_EXIT;
		return trainOperationMode;
	}

	TA_Base_Bus::ITrainCorbaDef::TrainStateUpdateOperationMode convertOperationMode(
		const TrainOperationMode& trainOperationMode)
	{
		FUNCTION_ENTRY("convertOperationMode");

		TA_Base_Bus::ITrainCorbaDef::TrainStateUpdateOperationMode trainStateUpdateOperationMode;
		unsigned int index;

		trainStateUpdateOperationMode.doubleAtsFailure = trainOperationMode.doubleAtsFailure;
		trainStateUpdateOperationMode.fallbackMode = trainOperationMode.fallbackMode;
		trainStateUpdateOperationMode.groupOffline = trainOperationMode.groupOffline;
		trainStateUpdateOperationMode.localDuty = trainOperationMode.localDuty;

		trainStateUpdateOperationMode.controlledLocations.length(trainOperationMode.controlledLocations.size());
		for(index = 0; index < trainOperationMode.controlledLocations.size(); ++index)
		{
			trainStateUpdateOperationMode.controlledLocations[index] = trainOperationMode.controlledLocations[index];
		}
		FUNCTION_EXIT;
		return trainStateUpdateOperationMode;
	}

}
