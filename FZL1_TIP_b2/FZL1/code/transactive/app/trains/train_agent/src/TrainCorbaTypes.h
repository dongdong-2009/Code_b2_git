/**
  * The source code in this file is the property of
  * MI Services Group and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/trains/train_agent/src/TrainCorbaTypes.h $
  * @author:  Glen Kidd
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  *
  */

#if !defined TRAIN_CORBA_TYPES_H
#define TRAIN_CORBA_TYPES_H

#include "bus/ats/ats_agent/IDL/src/IAtsTrainCorbaDef.h"
#include "bus/trains/TrainAgentCorbaDef/idl/src/ITrainCorbaDef.h"
#include "bus/security/duty_agent/IDL/src/DutyAgentTypeCorbaDef.h"

#include "TrainTypes.h"
#include "TrainInformation.h"

namespace TA_IRS_App
{

	TrainTypes::Track convertATSTrack(TA_IRS_Bus::ETrack t);

	ATSTrainMessage convertATSEvent(TA_IRS_Bus::IAtsTrainCorbaDef::AtsTrainMessageCorbaDef* x);

    // ss++
    // TD12082
	DutyTrainMessage convertDutyEvent(TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemState* x);
    //DutyTrainMessage convertDutyEventIncomplete(TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemDuty* x);
    // ++ss

	TA_Base_Bus::ITrainCorbaDef::TrainStateUpdateCollection convertTrainStateUpdate(std::vector<TA_IRS_App::TrainStatus::TrainInformation> & updateList);
	std::vector<TrainStatus::TrainInformation>  convertTrainStateUpdate(const class TA_Base_Bus::ITrainCorbaDef::TrainStateUpdateCollection& x);

	PECPoint convertPECPoint(const TA_Base_Bus::ITrainCorbaDef::PECPoint& corbaPECPoint);
	TA_Base_Bus::ITrainCorbaDef::PECPoint convertPECPoint(const PECPoint& trainPECPoint);

	TA_Base_Bus::ITrainCorbaDef::ETrainStatus convertTrainStatus(
	const TrainTypes::TrainStatus& trainStatus);
	TrainTypes::TrainStatus convertTrainStatus(
		const TA_Base_Bus::ITrainCorbaDef::ETrainStatus& corbaTrainStatus);

	TA_Base_Bus::ITrainCorbaDef::CctvSwitchState convertCctvSwitchState(
		const TrainStatus::CctvSwitchState& cctvSwitchState);
	TrainStatus::CctvSwitchState convertCctvSwitchState(
		const TA_Base_Bus::ITrainCorbaDef::CctvSwitchState& corbaCctvSwitchState);

	TA_Base_Bus::ITrainCorbaDef::TrainStateUpdateCallDriver convertCallDriver(
		const TrainStatus::TrainInformation& trainInformation);
	TA_Base_Bus::ITrainCorbaDef::TrainStateUpdateCctv convertCctv(
		const TrainStatus::TrainInformation& trainInformation);
	TA_Base_Bus::ITrainCorbaDef::TrainStateUpdateLibVers convertLibVers(
		const TrainStatus::TrainInformation& trainInformation);
	TA_Base_Bus::ITrainCorbaDef::TrainStateUpdatePA convertPA(
		const TrainStatus::TrainInformation& trainInformation);
	TA_Base_Bus::ITrainCorbaDef::TrainStateUpdatePEC convertPEC(
		const TrainStatus::TrainInformation& trainInformation);
	TA_Base_Bus::ITrainCorbaDef::TrainStateUpdateTSI convertTSI(
		const TrainStatus::TrainInformation& trainInformation);
	TA_Base_Bus::ITrainCorbaDef::TrainStateUpdateAlarmIDs convertAlarmIDs(
		const TrainStatus::TrainInformation& trainInformation);

	TA_IRS_App::TrainStatus::TrainInformation convertCallDriver(
		 const TA_Base_Bus::ITrainCorbaDef::TrainStateUpdateCallDriver& message);
	TA_IRS_App::TrainStatus::TrainInformation convertCctv(
		const TA_Base_Bus::ITrainCorbaDef::TrainStateUpdateCctv& message );
	TA_IRS_App::TrainStatus::TrainInformation convertLibVers(
		const TA_Base_Bus::ITrainCorbaDef::TrainStateUpdateLibVers& message );
	TA_IRS_App::TrainStatus::TrainInformation convertPA(
		const TA_Base_Bus::ITrainCorbaDef::TrainStateUpdatePA& message );
	TA_IRS_App::TrainStatus::TrainInformation convertPEC(
		const TA_Base_Bus::ITrainCorbaDef::TrainStateUpdatePEC& message );
	TA_IRS_App::TrainStatus::TrainInformation convertTSI(
		const TA_Base_Bus::ITrainCorbaDef::TrainStateUpdateTSI& message );
	TA_IRS_App::TrainStatus::TrainInformation convertAlarmIDs(
		const TA_Base_Bus::ITrainCorbaDef::TrainStateUpdateAlarmIDs& trainStateUpdateAlarmIDs);

	TrainOperationMode convertOperationMode(
		const TA_Base_Bus::ITrainCorbaDef::TrainStateUpdateOperationMode& trainStateUpdateOperationMode);
	TA_Base_Bus::ITrainCorbaDef::TrainStateUpdateOperationMode convertOperationMode(
		const TrainOperationMode& trainOperationMode);

	std::string getResourceSessionID(const TrainStatus::TrainInformation& trainInformation, TrainStatus::ResourceCategory resource);
	unsigned long getResourceCallID(const TrainStatus::TrainInformation& trainInformation, TrainStatus::ResourceCategory resource);
	bool findConnectedPECPoint(const PECPoint& trainPECPoint,
		const PECPointCollection& activePECPoints, unsigned short& connectedPECPoint);

};  // Namespace TA_IRS_App

// Helpers

//std::ostream & operator<< (std::ostream & os, const enum TA_IRS_App::TrainTypes::TrainAgentLocationType & x);

#endif
