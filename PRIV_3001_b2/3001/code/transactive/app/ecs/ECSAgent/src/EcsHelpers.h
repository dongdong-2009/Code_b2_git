/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/ecs/ECSAgent/src/EcsHelpers.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
// CachedConfig.h: interface for the CachedConfig class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(EcsHelpers_INCLUDED)
#define EcsHelpers_INCLUDED

#include "core/exceptions/src/ECSAgentExceptions.h"
#include "bus/ats/ats_agent/IDL/src/IAtsEcsCorbaDef.h"
#include "app/ecs/ECSAgent/src/EcsGlobals.h"


class EcsHelpers  
{

public:

    static bool EcsHelpers::isNotEqual(const TA_IRS_Bus::IStationEcsAgentCorbaDef::EcsStationModeExecutionUpdateData& a,
                                       const TA_IRS_Bus::IStationEcsAgentCorbaDef::EcsStationModeExecutionUpdateData& b);

    static bool EcsHelpers::isNotEqual(const TA_IRS_Bus::IMasterEcsAgentCorbaDef::EcsMasterModeExecutionUpdateData& a,
                                       const TA_IRS_Bus::IMasterEcsAgentCorbaDef::EcsMasterModeExecutionUpdateData& b);
 
    static std::string toUpper(const std::string& in);

    static std::string toUpper(const char* in);

    static std::string toString(unsigned long param);

    static std::string toString(TA_IRS_Bus::EEcsExecutionState state);

    static bool isTrue(const std::string& value);

    static void handleCachedMappingNotFoundException(const TA_Base_Core::CachedMappingNotFoundException& e, const std::string& description = "");

    static void proposedModeSetToCorba(const ProposedModeSet& from, TA_IRS_Bus::IMasterEcsAgentCorbaDef::ProposedModes& to);

    static void EcsHelpers::corbaToProposedModeSet(const TA_IRS_Bus::IMasterEcsAgentCorbaDef::ProposedModes from, ProposedModeSet& to);

    static TA_IRS_Bus::PhysicalTrainNumberType getPhysicalTrainNumber(const TA_IRS_Bus::IAtsEcsCorbaDef::AtsEcsMessageCorbaDef& packet);

    static TA_IRS_Bus::AtcTrainNumberType getAtcTrainNumber(const TA_IRS_Bus::IAtsEcsCorbaDef::AtsEcsMessageCorbaDef& packet);

    static TA_IRS_Bus::ServiceNumberType getServiceNumber(const TA_IRS_Bus::IAtsEcsCorbaDef::AtsEcsMessageCorbaDef& packet);

    static TA_IRS_Bus::ScheduleNumberType getScheduleNumber(const TA_IRS_Bus::IAtsEcsCorbaDef::AtsEcsMessageCorbaDef& packet);

    static unsigned long getEcsZoneId(const TA_IRS_Bus::IAtsEcsCorbaDef::AtsEcsMessageCorbaDef& packet);

    static unsigned long getStallTimeInSeconds(const TA_IRS_Bus::IAtsEcsCorbaDef::AtsEcsMessageCorbaDef& packet);

    static double getMaxExternalTemperatureInCelsius(const TA_IRS_Bus::IAtsEcsCorbaDef::AtsEcsMessageCorbaDef& packet);

    static bool getIsInEmergency(const TA_IRS_Bus::IAtsEcsCorbaDef::AtsEcsMessageCorbaDef& packet);

	static bool getTrainValid(const TA_IRS_Bus::IAtsEcsCorbaDef::AtsEcsMessageCorbaDef& packet);

	static bool getLocalisationValid(const TA_IRS_Bus::IAtsEcsCorbaDef::AtsEcsMessageCorbaDef& packet);

	static bool getOa1DataValid(const TA_IRS_Bus::IAtsEcsCorbaDef::AtsEcsMessageCorbaDef& packet);

    static std::string toCommaSeparatedList(const std::set<unsigned long>& numbers);

	static bool isStationOverlapped( TA_IRS_Bus::EcStationModeRecords& lastapply, TA_IRS_Bus::EcStationModeRecords& goingtobe); // TD17328&TD17443++

};




#endif // !defined(EcsHelpers_INCLUDED)
