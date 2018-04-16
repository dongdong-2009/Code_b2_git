/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/ecs/ECSAgent/src/EcsHelpers.cpp $
  * @author:  Ripple
  * @version: $Revision: #5 $
  *
  * Last modification: $DateTime: 2013/09/25 14:02:39 $
  * Last modified by:  $Author: huangjian $
  *
  */
#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290 4503)
#endif // defined _MSC_VER

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/exceptions/src/ECSAgentExceptions.h"
#include "app/ecs/ECSAgent/src/EcsHelpers.h"



std::string EcsHelpers::toUpper(const std::string& in)
{
    std::string out(in);
    std::transform(out.begin(), out.end(), out.begin(), toupper);
    return out;
}

std::string EcsHelpers::toUpper(const char* in)
{
    std::string out(in);
    std::transform(out.begin(), out.end(), out.begin(), toupper);
    return out;
}

std::string EcsHelpers::toString(unsigned long param)
{
    std::ostringstream oss;
    oss << param;
    return oss.str();

}

std::string EcsHelpers::toString(TA_IRS_Bus::EEcsExecutionState state)
{
    switch (state)
    {
    case TA_IRS_Bus::ECS_EXECUTION_READY:
        return "Ready";
    case TA_IRS_Bus::ECS_EXECUTION_CONTROL:
        return "Control";
    case TA_IRS_Bus::ECS_EXECUTION_IN_PROGRESS:
        return "In Progress";
    case TA_IRS_Bus::ECS_EXECUTION_COMPLETED:
        return "Completed";
    default: 
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unrecognised EEcsExecutionState enumeration");
        //TA_ASSERT(false, "Unrecognised EEcsExecutionState enumeration");
    }

    return "";
}

bool EcsHelpers::isTrue(const std::string& value)
{
    std::string ucValue = toUpper(value);

    return (
        ucValue == "T" ||
        ucValue == "TRUE" ||
        ucValue == "1" ||
        ucValue == "Y" ||
        ucValue == "YES" );
}

TA_IRS_Bus::PhysicalTrainNumberType EcsHelpers::getPhysicalTrainNumber(const TA_IRS_Bus::IAtsEcsCorbaDef::AtsEcsMessageCorbaDef& packet)
{
    return packet.physicalTrainNumber;
}

TA_IRS_Bus::AtcTrainNumberType EcsHelpers::getAtcTrainNumber(const TA_IRS_Bus::IAtsEcsCorbaDef::AtsEcsMessageCorbaDef& packet)
{
    return packet.atcTrainNumber;
}


TA_IRS_Bus::ServiceNumberType EcsHelpers::getServiceNumber(const TA_IRS_Bus::IAtsEcsCorbaDef::AtsEcsMessageCorbaDef& packet)
    // TA_THROW(InvalidATSPacketException("Invalid localisation data in ATS packet"));
{
//    if (!packet.localisation.valid)
 //   {
 //       TA_THROW(TA_Base_Core::InvalidATSPacketException("Invalid localisation data in ATS packet"));
 //   }
    return packet.localisation.serviceNumber;
}

TA_IRS_Bus::ScheduleNumberType EcsHelpers::getScheduleNumber(const TA_IRS_Bus::IAtsEcsCorbaDef::AtsEcsMessageCorbaDef& packet)
    // TA_THROW(InvalidATSPacketException("Invalid localisation data in ATS packet"));
{
//    if (!packet.localisation.valid)
//    {
 //       TA_THROW(TA_Base_Core::InvalidATSPacketException("Invalid localisation data in ATS packet"));
//    }
    return packet.localisation.scheduleNumber;
}

unsigned long EcsHelpers::getEcsZoneId(const TA_IRS_Bus::IAtsEcsCorbaDef::AtsEcsMessageCorbaDef& packet)
    // TA_THROW(InvalidATSPacketException("Invalid localisation data in ATS packet"));
{
//    if (!packet.localisation.valid)
//    {
//        TA_THROW(TA_Base_Core::InvalidATSPacketException("Invalid localisation data in ATS packet"));
//    }
    return packet.localisation.ecsZoneId;
}

unsigned long EcsHelpers::getStallTimeInSeconds(const TA_IRS_Bus::IAtsEcsCorbaDef::AtsEcsMessageCorbaDef& packet)
    // TA_THROW(InvalidATSPacketException("Invalid localisation data in ATS packet"));
{
//    if (!packet.localisation.valid)
//    {
//        TA_THROW(TA_Base_Core::InvalidATSPacketException("Invalid localisation data in ATS packet"));
//    }
    return packet.localisation.stallTimeInSeconds;
}

double EcsHelpers::getMaxExternalTemperatureInCelsius(const TA_IRS_Bus::IAtsEcsCorbaDef::AtsEcsMessageCorbaDef& packet)
    // TA_THROW(InvalidATSPacketException("Invalid OA1 data in ATS packet"));
{
//    if (!packet.oa1Data.valid)
//    {
//        TA_THROW(TA_Base_Core::InvalidATSPacketException("Invalid OA1 data in ATS packet"));
//    }
    return packet.oa1Data.maxExternalTemperatureInCelsius;
}

bool EcsHelpers::getIsInEmergency(const TA_IRS_Bus::IAtsEcsCorbaDef::AtsEcsMessageCorbaDef& packet)
    // TA_THROW(InvalidATSPacketException("Invalid OA1 data in ATS packet"));
{
//    if (!packet.oa1Data.valid)
//    {
//        TA_THROW(TA_Base_Core::InvalidATSPacketException("Invalid OA1 data in ATS packet"));
//    }
    return packet.oa1Data.isInEmergency;
}

bool EcsHelpers::getTrainValid(const TA_IRS_Bus::IAtsEcsCorbaDef::AtsEcsMessageCorbaDef& packet)
{
	return packet.trainValid;
}

bool EcsHelpers::getLocalisationValid(const TA_IRS_Bus::IAtsEcsCorbaDef::AtsEcsMessageCorbaDef& packet)
{
    return packet.localisation.valid;
}

bool EcsHelpers::getOa1DataValid(const TA_IRS_Bus::IAtsEcsCorbaDef::AtsEcsMessageCorbaDef& packet)
{
    return packet.oa1Data.valid;
}






void EcsHelpers::handleCachedMappingNotFoundException(const TA_Base_Core::CachedMappingNotFoundException& e, const std::string& description)
{
    if (description == "")
    {
        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugError, e.what());
    }
    else
    {
        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugError, description.c_str());
    }

    // ToDo: submit audit message or Alarm
}



bool EcsHelpers::isNotEqual(
    const TA_IRS_Bus::IStationEcsAgentCorbaDef::EcsStationModeExecutionUpdateData& a,
    const TA_IRS_Bus::IStationEcsAgentCorbaDef::EcsStationModeExecutionUpdateData& b)
{
    return 
       ((a.fromStationMode  != b.fromStationMode)         ||
        (a.toStationMode    != b.toStationMode)           ||
        (a.forMasterMode    != b.forMasterMode)           ||
        (a.locationKey      != b.locationKey)             ||
        (strcmp(a.sessionId, b.sessionId) !=0 )           ||
        (a.executionState   != b.executionState)          ||
        (a.executionError   != b.executionError)          ||
        (strcmp(a.additionalInfo, b.additionalInfo) !=0 ));   
}

bool EcsHelpers::isNotEqual(
    const TA_IRS_Bus::IMasterEcsAgentCorbaDef::EcsMasterModeExecutionUpdateData& a,
    const TA_IRS_Bus::IMasterEcsAgentCorbaDef::EcsMasterModeExecutionUpdateData& b)
{
    if ((a.fromMasterMode   != b.fromMasterMode)         ||
        (a.toMasterMode     != b.toMasterMode)           ||
        (strcmp(a.sessionId, b.sessionId) !=0 )          ||
        (a.executionState   != b.executionState)         ||
        (a.executionError   != b.executionError)         ||
        (strcmp(a.additionalInfo, b.additionalInfo) !=0 ))
    {
        return true;
    }

    // No difference so far
    // Length comparison of locations
    if (a.locations.length() != b.locations.length())
    {
        return true;
    }

    // No difference so far
    // Content comparison of locations
    int i=0;
    for (i=0; i<a.locations.length(); ++i)
    {
        if (a.locations[i] != b.locations[i])
        {
            return true;
        }
    }

    // No difference so far
    // Length comparison of stationUpdateDatas
    if (a.stationUpdateDatas.length() != b.stationUpdateDatas.length())
    {
        return true;
    }

    // No difference so far
    // Content comparison of stationUpdateDatas
    for (i=0; i<a.stationUpdateDatas.length(); ++i)
    {
        if (isNotEqual(a.stationUpdateDatas[i], b.stationUpdateDatas[i]))
        {
            return true;
        }
    }

    // No difference so far

    return false;

}

void EcsHelpers::proposedModeSetToCorba
    (const ProposedModeSet& from,  
     TA_IRS_Bus::IMasterEcsAgentCorbaDef::ProposedModes& to)
{
    to.length(from.size());

    ProposedModeSet::const_iterator it = from.begin();
    
    for (int index = 0; index< to.length(); ++index, ++it)
    {
        to[index] = *it;
    }
}


void EcsHelpers::corbaToProposedModeSet
    (const TA_IRS_Bus::IMasterEcsAgentCorbaDef::ProposedModes from, 
     ProposedModeSet& to)
{
    to.clear();

    //ProposedModeSet::iterator it = to.begin();
    
    for (int index = 0; index< from.length(); ++index)
    {
        to.insert(from[index]);
		//++it;
    }
}

std::string EcsHelpers::toCommaSeparatedList(const std::set<unsigned long>& numbers)
{
    if (numbers.size() == 0)
    {
        return "-";
    }

    std::ostringstream oss;
    std::set<unsigned long>::const_iterator it = numbers.begin();
    

    for (; it!=numbers.end(); ++it)
    {
        oss << *it << ",";
    }

    // return above minus the last comma
    return oss.str().substr(0, oss.str().size()-1);
}


// TD17328&TD17443++
bool EcsHelpers::isStationOverlapped( TA_IRS_Bus::EcStationModeRecords& lastapply, TA_IRS_Bus::EcStationModeRecords& goingtobe)
{
	bool flag = false;
	
	for ( int i = 0; i < lastapply.length(); ++i )
	{
		for ( int j = 0; j < goingtobe.length(); ++j )
		{
			if ( lastapply[i].locationKey == goingtobe[j].locationKey )
			{
				flag = true;
			}
		}
	}

	return flag;
}
// ++TD17328&TD17443
