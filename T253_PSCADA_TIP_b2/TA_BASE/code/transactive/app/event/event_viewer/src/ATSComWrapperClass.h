#pragma once
#include "app/event/ATSBridgeUtil/src/ATSBridgeUtil_i.h"
#include "boost/shared_ptr.hpp"
#include <string>
#include <atlcomcli.h>
#include <atlconv.h>

class AtsDateTimeWrap
{
public:
	AtsDateTimeWrap(CComPtr<IATSDateTime> pDateTime)
	{
		pDateTime->get_Year(&Year);
		pDateTime->get_Month(&Month);
		pDateTime->get_Day(&Day);
		pDateTime->get_Hour(&Hour);
		pDateTime->get_Minute(&Minute);
		pDateTime->get_Second(&Second);
		pDateTime->get_Nanosecond(&Nanosecond);
	}

	short Year;
	short Month;
	short Day;
	short Hour;
	short Minute;
	short Second;
	long Nanosecond;

private:
	AtsDateTimeWrap() {}; // hide defualt constructor
};

class AtsEventWrap
{
public:
	AtsEventWrap(CComPtr<IATSEvent> objEvent)
	{
		USES_CONVERSION;

		CComBSTR tempBstr;
		// Event ID
		objEvent->get_EventID(&tempBstr);
		EventID = OLE2A(tempBstr);
		tempBstr.Empty();

		//EventType
		objEvent->get_EventType(&tempBstr);
		EventType = OLE2A(tempBstr);
		tempBstr.Empty();

		CComPtr<IATSDateTime> pDateTime;
		objEvent->GetTimeStamp(&pDateTime);
		TimeStamp.reset(new AtsDateTimeWrap(pDateTime));

		// Asset;
		objEvent->get_Asset(&tempBstr);
		Asset = OLE2A(tempBstr);
		tempBstr.Empty();

		// Description;
		objEvent->get_Description(&tempBstr);
		Description = OLE2A(tempBstr);
		tempBstr.Empty();

		// Operator;
		objEvent->get_Operator(&tempBstr);
		Operator = OLE2A(tempBstr);
		tempBstr.Empty();

		// Location;
		objEvent->get_Location(&tempBstr);
		Location = OLE2A(tempBstr);
		tempBstr.Empty();

		// Value;
		objEvent->get_Value(&tempBstr);
		Value = OLE2A(tempBstr);
		tempBstr.Empty();

		// SystemName;
		objEvent->get_SystemName(&tempBstr);
		SystemName = OLE2A(tempBstr);
		tempBstr.Empty();

		// SubsystemName;
		objEvent->get_SubsystemName(&tempBstr);
		SubsystemName = OLE2A(tempBstr);
		tempBstr.Empty();

		// AlarmID;
		objEvent->get_AlarmID(&tempBstr);
		AlarmID = OLE2A(tempBstr);
		tempBstr.Empty();

		// AlarmType;
		objEvent->get_AlarmType(&tempBstr);
		AlarmType = OLE2A(tempBstr);
		tempBstr.Empty();

		// AlarmMMSState;
		objEvent->get_AlarmMMSState(&AlarmMMSState);

		// AlarmAvalancheType;
		objEvent->get_AlarmAvalancheType(&AlarmAvalancheType);
		
		// AlarmSeverity;
		objEvent->get_AlarmSeverity(&AlarmSeverity);
		
		// OperationalMaintenanceType;
		objEvent->get_OperationalMaintenanceType(&OperationalMaintenanceType);

		// AlarmState;
		objEvent->get_AlarmState(&AlarmState);
	}

	std::string EventID;
	std::string EventType;
	boost::shared_ptr<AtsDateTimeWrap> TimeStamp;
	std::string Asset;
	std::string Description;
	std::string Operator;
	std::string Location;
	std::string Value;
	std::string SystemName;
	std::string SubsystemName;
	std::string AlarmID;
	std::string AlarmType;
	short AlarmMMSState;
	short AlarmAvalancheType;
	short AlarmSeverity;
	short OperationalMaintenanceType;
	short AlarmState;

private:
	AtsEventWrap() {}; // Hide default constructor
};