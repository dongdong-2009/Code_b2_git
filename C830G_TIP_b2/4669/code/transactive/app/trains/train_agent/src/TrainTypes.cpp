/**
  * The source code in this file is the property of
  * MI Services Group and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/trains/train_agent/src/TrainTypes.cpp $
  * @author:  Glen Kidd
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  *
  */

#include <ostream>
#include <time.h>

#include "app/trains/train_agent/src/TrainTypes.h" 
#include "app/trains/train_agent/TrainProtocolLibrary/src/TrainCommand.h"
#include "app/trains/train_agent/TrainProtocolLibrary/src/TrainEvent.h"

#include "core/utilities/src/TAAssert.h"


//static const char * TA_IRS_App::RPARAM_TrainTSIUpdateStrategy = "TrainTSIUpdateStrategy";


#if 0 // not using templates ;) too hard to manage vc++ and g++
#ifdef _WIN32
	template <>
#else
	template < typename TA_IRS_App::TrainTypes::MPUState, unsigned char >
#endif
TA_IRS_App::TrainTypes::MPUState TA_IRS_App::convertSDSData(unsigned char origin) 
{
	TA_IRS_App::TrainTypes::MPUState mpu;
	switch (origin)
	{
	case 0:
		mpu = TA_IRS_App::TrainTypes::MPU_Primary;
		break;
	case 1:
		mpu = TA_IRS_App::TrainTypes::MPU_Secondary;
		break;
	default:
		mpu = TA_IRS_App::TrainTypes::MPU_Unknown;
	}
	return mpu;
}

#ifdef _WIN32
	template <>
#else
	template < typename TA_IRS_App::TrainTypes::Track, unsigned char >
#endif
TA_IRS_App::TrainTypes::Track TA_IRS_App::convertSDSData(unsigned char track) 
{
	TA_ASSERT(false, "Not implemented");
	return TA_IRS_App::TrainTypes::Track_Unknown;
}
#endif

TA_IRS_App::TrainTypes::MPUState TA_IRS_App::convertSDSDataOrigin(unsigned char origin) 
{
	TA_IRS_App::TrainTypes::MPUState mpu;
	switch (origin)
	{
	case 0:
		mpu = TA_IRS_App::TrainTypes::MPU_Primary;
		break;
	case 1:
		mpu = TA_IRS_App::TrainTypes::MPU_Secondary;
		break;
	default:
		mpu = TA_IRS_App::TrainTypes::MPU_Unknown;
	}
	return mpu;
};



// Helpers

std::ostream & operator<< (std::ostream & os, const enum TA_IRS_App::TrainTypes::TrainAgentLocationType & x)
{
	os << "[AgentLocation=";
	switch (x) 
	{
	case TA_IRS_App::TrainTypes::LOC_OCC:
		os << "Occ";
		break;
	case TA_IRS_App::TrainTypes::LOC_DEPOT:
		os << "Depot";
		break;
	case TA_IRS_App::TrainTypes::LOC_STATION:
		os << "Station";
		break;
	default:
		os << "<InvalidEnumerationValue>"; // not possible, for completeness
		break;
	}
	os << "]";
	return os;
}

std::ostream & operator<< (std::ostream & os, const enum TA_IRS_App::TrainTypes::MPUState & x)
{
	os << "[TrainMPUState=";
	switch (x) 
	{
	case TA_IRS_App::TrainTypes::MPU_Primary:
		os << "Primary";
		break;
	case TA_IRS_App::TrainTypes::MPU_Secondary:
		os << "Secondary";
		break;
	case TA_IRS_App::TrainTypes::MPU_Unknown:
		os << "Unknown";
		break;
	default:
		os << "<InvalidEnumerationValue>"; // not possible, for completeness
		break;
	}
	os << "]";
	return os;
}

std::ostream & operator<< (std::ostream & os, const enum TA_IRS_App::TrainTypes::Track & x)
{
	os << "[TrainDirection=";
	switch (x) 
	{
	case TA_IRS_App::TrainTypes::Track_Unknown:
		os << "Unknown";
		break;
	case TA_IRS_App::TrainTypes::Track_Inbound:
		os << "Inbound";
		break;
	case TA_IRS_App::TrainTypes::Track_Outbound:
		os << "Outbound";
		break;
	default:
		os << "<InvalidEnumerationValue>"; // not possible, for completeness
		break;
	}
	os << "]";
	return os;
}

std::ostream & operator<< (std::ostream & os, const enum TA_IRS_App::TrainTypes::TTISStatus & x)
{
	os << "[TrainTISStatus=";
	switch (x) 
	{
	case TA_IRS_App::TrainTypes::TTIS_Unknown:
		os << "Unknown";
		break;
	case TA_IRS_App::TrainTypes::TTIS_Library:
		os << "Library";
		break;
	case TA_IRS_App::TrainTypes::TTIS_Schedule:
		os << "Schedule";
		break;
	default:
		os << "<InvalidEnumerationValue>"; // not possible, for completeness
		break;
	}
	os << "]";
	return os;
}

std::ostream & operator<< (std::ostream & os, const enum TA_IRS_App::TrainTypes::TrainStatus & x)
{
	os << "[TrainAudioStatus=";
	switch (x) 
	{
	case TA_IRS_App::TrainTypes::IDLE:
		os << "Idle";
		break;
	case TA_IRS_App::TrainTypes::CAB_TO_CAB:
		os << "CabToCab";
		break;
	case TA_IRS_App::TrainTypes::DRIVER_PA:
		os << "DriverPA";
		break;
	default:
		os << "<InvalidEnumerationValue>"; // not possible, for completeness
		break;
	}
	os << "]";
	return os;
}

std::ostream & operator<< (std::ostream & os, const enum TA_IRS_App::TrainTypes::PECSource & x)
{
	os << "[TrainPECSource=";
	switch (x) 
	{
	case TA_IRS_App::TrainTypes::PEC_UNASSIGNED:
		os << "Unassigned";
		break;
	case TA_IRS_App::TrainTypes::PEC_DRIVER:
		os << "Driver";
		break;
	case TA_IRS_App::TrainTypes::PEC_OPERATOR:
		os << "Operator";
		break;
	default:
		os << "<InvalidEnumerationValue>"; // not possible, for completeness
		break;
	}
	os << "]";
	return os;
}

std::ostream & operator<< (std::ostream & os, const enum TA_IRS_App::TrainTypes::AtsValid & x)
{
	os << "[AtsValid=";
	switch (x) 
	{
	case TA_IRS_App::TrainTypes::AtsValid_Unknown:
		os << "Unknown";
		break;
	case TA_IRS_App::TrainTypes::AtsValid_Invalid:
		os << "Invalid";
		break;
	case TA_IRS_App::TrainTypes::AtsValid_Valid:
		os << "Valid";
		break;
	default:
		os << "<InvalidEnumerationValue>"; // not possible, for completeness
		break;
	}
	os << "]";
	return os;
}

std::ostream & operator<< (std::ostream & os, const TA_IRS_App::PECPoint & x)
{
	// char buf[40]; // more than enough
/// @todo fix this...	strftime(buf, 80, "%Y-%m-%dT%H:%M:%S", localtime(reinterpret_cast<const long *>(&x.creationTime)));
	os << "[PECPoint=" 
		<< "[Car=" << static_cast<unsigned int>(x.car) << "]"
		<< "[PEC=" << static_cast<unsigned int>(x.PEC) << "]"
		<< "[source=" << x.source << "]"
//		<< "[creationTime=" << buf << "]"
		<< "[creationTime=" << x.creationTime << "]"
		<< "]";
	return os;
}

std::ostream & operator<< (std::ostream & os, const TA_IRS_App::ATSTrainMessage & x)
{
	os << "[ATSMessage="
		<< "[TrainID=" << static_cast<int>(x.trainID) << "]"
		<< "[ValidTrain=" << x.isValid << "]"
		<< "[LocationValid=" << x.isLocalisationValid << "]"
		<< "[CurrentLocationKey=" << x.currentLocation << "]"
		<< "[ServiceNumber=" << x.serviceNumber << "]"
		<< "[IsMute=" << x.isMute << "]"
		<< x.track
		<< "[CCTV Zone=" << x.currentCctvZone << "]"
		<< "]";
	return os;
}

std::ostream & operator<< (std::ostream & os, const TA_IRS_App::TrainOperationMode & x)
{
	os << "[DoubleAtsFailure="; x.doubleAtsFailure ? os << "true" : os << "false"; os << "] ";
	os << "[FallbackMode="; x.fallbackMode ? os << "true" : os << "false"; os << "] ";
	os << "[GroupOffline="; x.groupOffline ? os << "true" : os << "false"; os << "] ";
	os << "[LocalDuty="; x.localDuty ? os << "true" : os << "false"; os << "] ";
	os << "[Degraded locations=";
	for(TA_IRS_App::ControlledLocations::const_iterator iter = x.controlledLocations.begin();
		iter != x.controlledLocations.end(); ++iter)
	{
		os << *iter << " ";
	}
	os << "]" << std::ends;
	return os;
}

std::ostream & operator<< (std::ostream & os, const TA_IRS_App::TrainCommand & x)
{
	os << "[TrainCommand="
		<< "[Command=" << static_cast<unsigned int>(x.getCommand()) << "]"
		<< "[TrainID=" << static_cast<unsigned int>(x.getTrainID()) << "]"
		<< "[Origin=" << static_cast<unsigned int>(x.getOrigin()) << "]"
		<< "[ReplyEvent=";
//	if ( -1 == x.getReplyEventMessageNumber())
	std::vector<int> replyEvent = x.getReplyEventMessageNumber();
	if ( replyEvent.size() == 0)
	{
		os << "invalid"; // Must always have a reply event, this should never occur
	}
	else if ( *(replyEvent.begin()) == -1)
	{
		os << "nil";
	}
	else
	{
		std::vector<int>::iterator i = replyEvent.begin();
		os << static_cast<unsigned int>(*i);
		++i;
		for (; i != replyEvent.end(); ++i) 
		{
			os << "," << static_cast<unsigned int>(*i);
		}
	}
	os << "]"
		<< "]";
	return os;
}

std::ostream & operator<< (std::ostream & os, TA_IRS_App::TrainEventPtr & x)
{
	os << "[TrainEvent="
		<< "[Command=" << static_cast<unsigned int>(x->getMessageType()) << "]"
		<< "[TrainID=" << static_cast<unsigned int>(x->getTrainID()) << "]"
		<< "[Origin=" << static_cast<unsigned int>(x->getOrigin()) << "]"
		<< "[TSI=" << x->getTrainITSI() << "]"
		<< "]";
	return os;
}

std::ostream & operator<< (std::ostream & os, TA_IRS_App::TrainEvent & x)
{
	os << "[TrainEvent="
		<< "[Command=" << static_cast<unsigned int>(x.getMessageType()) << "]"
		<< "[TrainID=" << static_cast<unsigned int>(x.getTrainID()) << "]"
		<< "[Origin=" << static_cast<unsigned int>(x.getOrigin()) << "]"
		<< "[TSI=" << x.getTrainITSI() << "]"
		<< "]";
	return os;
}
