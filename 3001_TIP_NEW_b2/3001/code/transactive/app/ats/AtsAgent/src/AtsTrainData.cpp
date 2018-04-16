/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/3001_TIP_NEW/3001/transactive/app/ats/AtsAgent/src/AtsTrainData.cpp $
  * @author:   Darren Sampson
  * @version:  $Revision: #4 $
  *
  * Last modification: $DateTime: 2017/12/08 17:42:23 $
  * Last mofified by:  $Author: Ouyang $
  *
  * This represents the ATS Train Record
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include <sstream>
#include <vector>
#include <string>

#include "core/utilities/src/TAAssert.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/types/ATSAgentComms_MessageTypes.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/data_access_interface/ats/src/AtsFbMapRecord.h"

#include "app/ats/AtsAgent/src/AddressUtil.h"
#include "app/ats/AtsAgent/src/AtsEcsData.h"
#include "app/ats/AtsAgent/src/AtsTrainData.h"

#include "app/ats/AtsAgent/AtsTCPCommunication/src/AtsTcpServer.h"
#include "app/ats/AtsAgent/AtsMethod/src/AtsMethodTrainStatusReport.h"
#include "app/ats/AtsAgent/AtsMethod/src/AtsMethodOA1Table.h"

#define ATS_TRAIN_RECORD_EXCEPTIONS_EXTRA_DETAIL

using namespace TA_IRS_App;

static std::string num2str(unsigned long num)
{
#ifdef ATS_TRAIN_RECORD_EXCEPTIONS_EXTRA_DETAIL
	char str[16] = {0};
    sprintf(str,"%u",num);

    // this declaration and assignment merely for the purposes of a return value
    // may seem redundant when the compiler will do it for free but making the
    // conversion explicit seems to reduce Solaris compiler warnings.
    std::string str_=str;

    return str_;
#else
    return "";
#endif // #ifdef ATS_TRAIN_RECORD_EXCEPTIONS_EXTRA_DETAIL
}

static std::string makeFbString(TA_IRS_Bus::PhysicalTrainNumberType physicalTrainNumber, unsigned long fixedBlocksBranchId, unsigned long fbAbscissaInTheBranch, unsigned long headCarAbscissaOnFb)
{
    std::string fbString = "";
#ifdef ATS_TRAIN_RECORD_EXCEPTIONS_EXTRA_DETAIL
    fbString = " train# ";
    fbString.append(num2str(physicalTrainNumber));
    fbString.append(" fb_Abscissa=");
    fbString.append(num2str(fixedBlocksBranchId));
    fbString.append(" fb_Abscissa=");
    fbString.append(num2str(fbAbscissaInTheBranch));
    fbString.append(" hc_Abscissa=");
    fbString.append(num2str(headCarAbscissaOnFb));
#endif // #ifdef ATS_TRAIN_RECORD_EXCEPTIONS_EXTRA_DETAIL

    return fbString;
}

namespace
{
	const long INVALID_TIMER_ID(-1);
}


namespace TA_IRS_App
{

	const std::string ATS_TRAIN_DRIVER_CONSOLE_STATUS			= "DriverConsoleStatus";
	const std::string ATS_TRAIN_DETRAINMENT_DOOR_COVER_STATUS	= "DetrainmentDoorCoverStatus";
	const std::string ATS_TRAIN_DETRAINMENT_DOOR_OPEN_STATUS	= "DetrainmentDoorOpenStatus";
	const std::string ATS_TRAIN_FIRE_EXTINGUISHER_STATUS		= "FireExtinguisherStatus";
	const std::string ATS_TRAIN_INTERNAL_SMOKE_DETECTED			= "InternalSmokeDetected";
	const std::string ATS_TRAIN_EHS_STATUS						= "EhsStatus";
	const std::string ATS_TRAIN_DOOR_OBSTRUCTION_STATUS			= "DoorObstructionStatus";

	
	AtsTrainData::AtsTrainData(	TA_IRS_Bus::PhysicalTrainNumberType physicalTrainNumber, unsigned long entityKey, 
									unsigned long locationKey, TA_IRS_Core::IAtsFixedBlockMap&	stationZones,
									TA_IRS_Core::IAtsFixedBlockMap& cctvZones,
									std::vector<unsigned long>& stationsInThisSector,
									AtsTrainDataTimer* atsTrainDataTimer)
	:
	m_serviceNumber (9999),
	m_trainInfoChanged (true),
	m_stationZones (stationZones),
	m_cctvZones (cctvZones),
	m_commsMessageSender (NULL),
	m_entityKey (entityKey), 
	m_locationKey (locationKey),
	m_recordImported (false),
	m_stationsInThisSector(stationsInThisSector),
    m_oa1Valid(false),
    m_atcTrainStatusValid(false),
	m_oa1TimerId(INVALID_TIMER_ID),
	m_atcTimerId(INVALID_TIMER_ID),
	m_atsTrainDataTimer(atsTrainDataTimer),
	m_carNum1(0) ,
	m_carNum2(0),
	m_carNum3(0),
	m_server(NULL)
	{
		m_atsTrainMessage.physicalTrainNumber = physicalTrainNumber;
		m_atsTrainMessage.trainValid = false;

		m_atsTrainMessage.localisation.localisationValid = false;
		m_atsTrainMessage.localisation.currentLocation = 0;
		m_atsTrainMessage.localisation.lastLocation = 0;
		m_atsTrainMessage.localisation.currentCctvZone = 0;
		m_atsTrainMessage.localisation.physicalTrainService = CORBA::string_dup("");
		std::ostringstream physicalTrainService;
		physicalTrainService << (int)physicalTrainNumber << "/_";
		m_atsTrainMessage.localisation.physicalTrainService = physicalTrainService.str().c_str();

		m_atsTrainMessage.localisation.isMute = false;
		m_atsTrainMessage.localisation.track = TA_IRS_Bus::TrackUnknown;

		int i;

		for (i = 0; i < OA1_TABLE_SIZE; ++i)
		{
			m_oa1Table[i] = 0;
		}

		for (i = 0; i < ATC_STATUS_REPORT_SIZE; ++i)
		{
			m_atcStatusReport[i] = 0;
		}
		m_atcTrainStatusCodeHeader = 0;

		// get a handle to CommsMessageSender
		m_commsMessageSender =
			TA_Base_Core::MessagePublicationManager::getInstance().getCommsMessageSender ( TA_Base_Core::ATSAgentComms::Context );

	};

	AtsTrainData::~AtsTrainData()
	{
		// clean up comms message sender
		if ( m_commsMessageSender != NULL )
		{
			delete m_commsMessageSender;
			m_commsMessageSender = NULL;
		}
	}

	TA_IRS_Bus::IAtsTrainCorbaDef::AtsTrainMessageCorbaDef AtsTrainData::buildAtsTrainMessage()
	{
		return m_atsTrainMessage;
	}

	void AtsTrainData::importAtsTrainMessage(const TA_IRS_Bus::IAtsTrainCorbaDef::AtsTrainMessageCorbaDef& trainMessage)
	{
		updateTrainValidity(trainMessage.trainValid);

		if (trainMessage.localisation.localisationValid)
		{
			updatePhysicalTrainService((const char*)trainMessage.localisation.physicalTrainService);
			
			updateZoneId(trainMessage.localisation.currentLocation,
						 trainMessage.localisation.currentCctvZone,
						 trainMessage.localisation.track);

			/*
			if (inThisSector(trainMessage.localisation.currentLocation))
			{
				updateLocalisationValidity(true);
			}
			else
			{
				updateTrainValidity(false);
				updateLocalisationValidity(false);
			}*/


			// upDirection currently not used
			updateMute(trainMessage.localisation.isMute);
			
		}
		else
		{
			updateLocalisationValidity(false);
		}
	}

	bool AtsTrainData::inThisSector(unsigned long stationId)
	{

		return true ; //c955 no sector concept already.

		
	}

	void AtsTrainData::clearImportFlag()
	{
		m_recordImported = false;
	}

	void AtsTrainData::setTrainValidity()
	{	
		if (!m_atsTrainMessage.trainValid)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                "setTrainValidity() - train %d validity, was invalid now valid", 
				m_atsTrainMessage.physicalTrainNumber);

			m_atsTrainMessage.trainValid = true;
			m_trainInfoChanged = true;
		}

		m_recordImported = true;
	}

    void AtsTrainData::invalidateTrain()
    {
        updateTrainValidity(false);
    }

	void AtsTrainData::updateTrainValidity(bool valid)
	{	
		if (m_atsTrainMessage.trainValid != valid)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                "updateTrainValidity() - train %d validity, was %s now %s", 
				m_atsTrainMessage.physicalTrainNumber, (m_atsTrainMessage.trainValid?"valid":"invalid"), 
				(valid?"valid":"invalid"));

			m_atsTrainMessage.trainValid = valid;
			m_trainInfoChanged = true;

			if (!valid)
			{
				// Invalidate localisation as well
				updateLocalisationValidity(false);
			}
		}
	}

	void AtsTrainData::checkIfTrainNotImported()
	{
		if (!m_recordImported && m_atsTrainMessage.trainValid)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                "checkIfTrainNotImported() - not updated, train %d validity, was valid now invalid", 
				m_atsTrainMessage.physicalTrainNumber);

			m_atsTrainMessage.trainValid = false;
			m_trainInfoChanged = true;

			// Invalidate localisation as well
			updateLocalisationValidity(false);
		}
	}


	void AtsTrainData::updateLocalisationValidity(bool valid)
	{	
		if (m_atsTrainMessage.localisation.localisationValid != valid)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                "updateLocalisationValidity() - train %d validity, was %s now %s", 
				m_atsTrainMessage.physicalTrainNumber, 
				(m_atsTrainMessage.localisation.localisationValid?"valid":"invalid"), 
				(valid?"valid":"invalid"));

			m_atsTrainMessage.localisation.localisationValid = valid;

			m_trainInfoChanged = true;
		}
	}

	void AtsTrainData::updateServiceNumber(TA_IRS_Bus::ServiceNumberType serviceNumber)
	{
		if (serviceNumber != m_serviceNumber)
		{	
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                "updateServiceNumber() - train %d serviceNumber, was %d now %d", 
				m_atsTrainMessage.physicalTrainNumber, m_serviceNumber, serviceNumber);

			m_serviceNumber = serviceNumber;
			std::ostringstream physicalTrainService;
			physicalTrainService << (int)m_atsTrainMessage.physicalTrainNumber << "/";
			// TES447 : if service number is blank, it should be shown as "_"
			// 4669R22020009-83001_00_D310_CSY_0131_Train_numbering_1B.doc,
			// now the default value is 9999, so if here it's still 9999, that means
			// atsagent doesnt get a valid service number, then it will show _.
			if ( m_serviceNumber != 9999 )
			{
				physicalTrainService << (int)m_serviceNumber;
			}
			else
			{
				physicalTrainService << "_";
			}
			
			updatePhysicalTrainService(physicalTrainService.str());
		}
	}

	void AtsTrainData::updatePhysicalTrainService(const std::string physicalTrainService)
	{
		std::string currentPhysicalTrainService = (char *)m_atsTrainMessage.localisation.physicalTrainService;

		if (physicalTrainService != currentPhysicalTrainService)
		{	
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                "updatePhysicalTrainService() - Physical Train Serivce was %s now %s", 
				currentPhysicalTrainService.c_str(), physicalTrainService.c_str());

			m_atsTrainMessage.localisation.physicalTrainService = physicalTrainService.c_str();
			
			m_trainInfoChanged = true;
		}
	}

	void AtsTrainData::updateZoneId(unsigned long fixedBlocksBranchId,
							  	   unsigned long fbAbscissaInTheBranch,
								   unsigned long	headCarAbscissaOnFb)
	{
		unsigned long stationZoneId = 0;

		try
		{
			stationZoneId = (unsigned long)m_stationZones.getZoneId(fixedBlocksBranchId, fbAbscissaInTheBranch, headCarAbscissaOnFb);
		}
		catch (TA_Base_Core::DatabaseException& e)
		{
			if (!m_atsTrainMessage.localisation.localisationValid || m_atsTrainMessage.localisation.currentLocation != 0)
			{
				std::string text = "Station Zone ID DatabaseException - " + std::string(e.what());
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
							"updateZoneId() - train %d - %s", 
							m_atsTrainMessage.physicalTrainNumber, text.c_str());
			}
			stationZoneId = 0;
		}
		catch (TA_Base_Core::DataException& e)
		{
			if (!m_atsTrainMessage.localisation.localisationValid || m_atsTrainMessage.localisation.currentLocation != 0)
			{
				std::string text = "Station Zone ID DataException - " + std::string(e.what());
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
							"updateZoneId() - train %d - %s", 
							m_atsTrainMessage.physicalTrainNumber, text.c_str());
			}
			stationZoneId = 0;
		}


		/*

		double cctvZoneId = 0;

		try
		{
			cctvZoneId = m_cctvZones.getZoneId(fixedBlocksBranchId, fbAbscissaInTheBranch, headCarAbscissaOnFb);
		}
		catch (TA_Base_Core::DatabaseException& e)
		{
			if (!m_atsTrainMessage.localisation.localisationValid || m_atsTrainMessage.localisation.currentCctvZone != 0)
			{
				std::string text = "CCTV Zone ID DataException - " + std::string(e.what());
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
							"updateZoneId() - train %d - %s", 
							m_atsTrainMessage.physicalTrainNumber, text.c_str());
			}
			cctvZoneId = 0;
		}
		catch (TA_Base_Core::DataException& e)
		{
			if (!m_atsTrainMessage.localisation.localisationValid || m_atsTrainMessage.localisation.currentCctvZone != 0)
			{
				std::string text = "CCTV Zone ID DataException - " + std::string(e.what());
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
							"updateZoneId() - train %d - %s", 
							m_atsTrainMessage.physicalTrainNumber, text.c_str());
			}
			cctvZoneId = 0;
		}

		*/

		TA_IRS_Bus::ETrack track = TA_IRS_Bus::TrackUnknown;

		try
		{
			std::string trackString = m_stationZones.getTrack(fixedBlocksBranchId, fbAbscissaInTheBranch, headCarAbscissaOnFb);

			if (trackString == TA_IRS_Core::AtsFbMapRecord::TRACK_INNERBOUND)
			{
				track = TA_IRS_Bus::TrackInnerbound;
			}
			else if (trackString == TA_IRS_Core::AtsFbMapRecord::TRACK_CENTRALINNERBOUND)
			{
				track = TA_IRS_Bus::TrackCentralInnerbound;
			}
			else if (trackString == TA_IRS_Core::AtsFbMapRecord::TRACK_OUTERBOUND)
			{
				track = TA_IRS_Bus::TrackOuterbound;
			}
			else if (trackString == TA_IRS_Core::AtsFbMapRecord::TRACK_CENTRALOUTERBOUND)
			{
				track = TA_IRS_Bus::TrackCentralOuterbound;
			}
			else
			{
				track = TA_IRS_Bus::TrackUnknown;
			}
		}
		catch (TA_Base_Core::DatabaseException& e)
		{
			std::string text = "updateZoneId() - " + std::string(e.what())
                             /*+ makeFbString(physicalTrainNumber, fixedBlocksBranchId, fbAbscissaInTheBranch, headCarAbscissaOnFb)*/;
			LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", text);
			track = TA_IRS_Bus::TrackUnknown;
		}
		catch (TA_Base_Core::DataException& e)
		{
			std::string text = "updateZoneId() - " + std::string(e.what())
                             /*+ makeFbString(physicalTrainNumber, fixedBlocksBranchId, fbAbscissaInTheBranch, headCarAbscissaOnFb)*/;
			LOG_EXCEPTION_CATCH(SourceInfo, "DataException", text);
			track = TA_IRS_Bus::TrackUnknown;
		}

		updateZoneId(stationZoneId, 0, track); //hardcode the cctv zone id
	}

	void AtsTrainData::updateZoneId(const unsigned long stationZoneId,
							  	    const double cctvZoneId,
									const TA_IRS_Bus::ETrack track)
	{
		if (stationZoneId != m_atsTrainMessage.localisation.currentLocation)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                "updateZoneId() - train %d currentLocation, was %d now %d", 
				m_atsTrainMessage.physicalTrainNumber, 
				m_atsTrainMessage.localisation.currentLocation, stationZoneId);

			m_atsTrainMessage.localisation.lastLocation = m_atsTrainMessage.localisation.currentLocation;
			m_atsTrainMessage.localisation.currentLocation = stationZoneId;
	
			m_trainInfoChanged = true;
		}
		/*

		if (cctvZoneId != m_atsTrainMessage.localisation.currentCctvZone)
		{
			m_atsTrainMessage.localisation.currentCctvZone = cctvZoneId;
	
			m_trainInfoChanged = true;
		}*/


		if (track != m_atsTrainMessage.localisation.track)
		{
			m_atsTrainMessage.localisation.track = track;
	
			m_trainInfoChanged = true;
		}
	}

	void AtsTrainData::updateMute(bool mute)
	{
		if (m_atsTrainMessage.localisation.isMute != mute)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                "updateMute() - train %d mute, was %s now %s", 
				m_atsTrainMessage.physicalTrainNumber, 
				m_atsTrainMessage.localisation.isMute?"set":"not set", 
				mute?"set":"not set");

			m_atsTrainMessage.localisation.isMute = mute;

			m_trainInfoChanged = true;
		}
	}

	void AtsTrainData::updateOa1Table(const TA_IRS_Bus::IAtsTrainCorbaDef::OctetSequence& oa1)
	{
		// Check if the sequence is too small
		if (oa1.length() != OA1_TABLE_SIZE)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
					"sendOa1TableToAts() - OA1 Table received for train %d - incorrect size",
					m_atsTrainMessage.physicalTrainNumber);
			
			m_oa1Valid = false;
		}
		else
		{
			for (unsigned int i = 0; i < OA1_TABLE_SIZE; i++)
			{
				m_oa1Table[i] = oa1[i];
			}

			m_oa1Valid = true;
			//td 13947
			//zhou yuan++
			//start the AtsTrainDataTimer for OA1 table
			m_oa1TimerId = m_atsTrainDataTimer->startTimer(*this, Oa1Timer, m_oa1TimerId);
			//++zhou yuan
		}
	}

	void AtsTrainData::updateAtcTrainStatusReport(const TA_IRS_Bus::IAtsTrainCorbaDef::OctetSequence& atcTrainStatus)
	{
		// Check if the sequence is too small (record size plus header byte)
		if (atcTrainStatus.length() < ATC_STATUS_REPORT_SIZE + 1)
		{
			std::string message = "atcTrainStatus sequence too small";
			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
				"updateAtcTrainStatusReport() - %s",
				message.c_str());
			
			m_atcTrainStatusValid = false;
		}
		else
		{
			m_atcTrainStatusCodeHeader = atcTrainStatus[0];

			for (unsigned int i = 0; i < ATC_STATUS_REPORT_SIZE; i++)
			{
				m_atcStatusReport[i] = atcTrainStatus[i+1];
			}

			m_atcTrainStatusValid = true;
			//td 13947
			//zhou yuan++
			//start the AtsTrainDataTimer for ATC table
			m_atcTimerId = m_atsTrainDataTimer->startTimer(*this, AtcTimer, m_atcTimerId);
			//++zhou yuan
		}
	}

	bool AtsTrainData::isUpdated()
	{
		return m_trainInfoChanged;
	}

	bool AtsTrainData::isValid()
	{
		return m_atsTrainMessage.trainValid;
	}

	bool AtsTrainData::isValidForExport(unsigned long locationKey,
		TA_Base_Core::AtsAgentEntityData::EAtsLocationType locationType)
	{
		switch (locationType)
		{
		case TA_Base_Core::AtsAgentEntityData::Occ:
		case TA_Base_Core::AtsAgentEntityData::Depot:
			return isValid() || isOa1Valid() || isAtcTrainStatusValid();

		case TA_Base_Core::AtsAgentEntityData::SecondaryStation1:
		case TA_Base_Core::AtsAgentEntityData::SecondaryStation2:
		case TA_Base_Core::AtsAgentEntityData::MainStation:
			return (isValid() && getStation() == locationKey) || isOa1Valid() || isAtcTrainStatusValid();

		default:
			return false;
		}
	}

	unsigned long AtsTrainData::getStation()
	{
		if (m_atsTrainMessage.localisation.localisationValid)
		{
			return m_atsTrainMessage.localisation.currentLocation;
		}
		else
		{
			return 0;
		}
	}

	bool AtsTrainData::isOa1Valid()
	{
		return m_oa1Valid;
	}

	unsigned char AtsTrainData::getOa1Data(unsigned int index)
	{
		TA_ASSERT(index < OA1_TABLE_SIZE, "OA1 Data index out of bounds");

		return m_oa1Table[index];
	}

	bool AtsTrainData::isAtcTrainStatusValid()
	{
		return m_atcTrainStatusValid;
	}

	unsigned char AtsTrainData::getAtcTrainStatusCodeHeader()
	{
		return m_atcTrainStatusCodeHeader;
	}
	
	unsigned char AtsTrainData::getAtcStatusReport(unsigned int index)
	{
		TA_ASSERT(index < ATC_STATUS_REPORT_SIZE, "Index out of bounds");
		
		return m_atcStatusReport[index];
	}

	void AtsTrainData::sendUpdates()
	{
		if (isUpdated())
		{
			
// 			TA_IRS_Bus::IAtsTrainCorbaDef::AtsTrainMessageCorbaDef* trainMessage 
// 				= new TA_IRS_Bus::IAtsTrainCorbaDef::AtsTrainMessageCorbaDef (buildAtsTrainMessage());

			TA_IRS_Bus::IAtsTrainCorbaDef::AtsTrainMessageCorbaDef trainMessage(buildAtsTrainMessage()); 

			CORBA::Any data;
				data <<= trainMessage;

			m_commsMessageSender->sendCommsMessage ( TA_Base_Core::ATSAgentComms::AtsTrainUpdate,
													m_entityKey,
													data,
													0,
													m_locationKey);

			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
				"sendUpdates() - Info: AtsTrainMessage sent - train=%d, entity=%d, location=%d, train=%s, localisation=%s, trainlocation=%d, lastlocation=%d",
				m_atsTrainMessage.physicalTrainNumber, m_entityKey, m_locationKey,
				(m_atsTrainMessage.trainValid?"VALID":"INVALID"),
				(m_atsTrainMessage.localisation.localisationValid?"VALID":"INVALID"),
				m_atsTrainMessage.localisation.currentLocation,
				m_atsTrainMessage.localisation.lastLocation);

			m_trainInfoChanged = false;
		}
	}

	//td 13947
	//zhou yuan++
	//start the AtsTrainDataTimer for ATC table	
	void AtsTrainData::handleTimeout(ETimerType timerType)
	{
		FUNCTION_ENTRY("handleTimeout");

		switch(timerType) 
		{
		case AtcTimer:
			{			
				m_atcTrainStatusValid = false;
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
					"the AtcTimer is timeout");
				break;
			}
		case Oa1Timer:
			{
				m_oa1Valid = false;
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
					"the Oa1Timer is timeout");				
				break;
			}
		default:
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
				"the AtsTrainDataTimer type is invalid!");
		}

		FUNCTION_EXIT;
	}
	void AtsTrainData::updateCarNum1 ( unsigned long num ) 
	{
		m_carNum1 = num ;
	}
	void AtsTrainData::updateCarNum2 ( unsigned long num ) 
	{
		m_carNum2 = num ;
	}
	void AtsTrainData::updateCarNum3  ( unsigned long num ) 
	{
		m_carNum3 = num ;
	}
	//++zhou yuan
}	

void AtsTrainData::startAtsServer(TA_IRS_App::AtsTcpServer* server)
{
	FUNCTION_ENTRY("startAtsServer");
	m_server = server;
	FUNCTION_EXIT;
}

void AtsTrainData::updateAtsMethodTrainStatusReport(const TA_IRS_Bus::IAtsTrainCorbaDef::OctetSequence& atcTrainStatus)
{
	FUNCTION_ENTRY("updateAtsMethodTrainStatusReport");
	std::vector<unsigned char> criticalAlarmEvent;
	
	std::string hexText = "";
	std::string consoleTxt = "";


	if ( atcTrainStatus.length() < OA1_DATA_HEADER_LENGTH )
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
			"updateAtsMethodTrainStatusReport() - atcTrainStatus Table received for train %d - incorrect size",
			m_atsTrainMessage.physicalTrainNumber);

		m_oa1Valid = false;

	}
	else
	{
		for(unsigned int i=0; i<atcTrainStatus.length(); i++)
			criticalAlarmEvent.push_back(atcTrainStatus[i]);

		TA_IRS_App::AtsMethodTrainStatusReport methodTrainStatus(criticalAlarmEvent);
		methodTrainStatus.getLogText(hexText,consoleTxt);
		try
		{
			m_server->sendMethod(methodTrainStatus);
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
				"\n\n\t\t\t****** CRITICAL ALARM ******\n\n"
				"\t\t\tFROM : updateAtsMethodTrainStatusReport\n"
				"\t\t\tTrain ID      : %X\n"
				"\t\t\tData Received : %s\n\n",criticalAlarmEvent[2],hexText.c_str());

		}catch(...)
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"The transaction Timeout");
		}
		m_oa1Valid = true;
	}

	FUNCTION_EXIT;
}

void AtsTrainData::updateAtsMethodOa1Table(const TA_IRS_Bus::IAtsTrainCorbaDef::OctetSequence& oa1)
{
	FUNCTION_ENTRY("updatedAtsMethodOa1Table");
	std::vector<unsigned char> criticalAlarmEvent;
	
	std::string hexText = "";
	std::string consoleTxt = "";
	
	if ( oa1.length() < OA1_DATA_HEADER_LENGTH )
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
			"updateAtsMethodOa1Table() - OA1 Table received for train %d - incorrect size",
			m_atsTrainMessage.physicalTrainNumber);
		
		m_oa1Valid = false;

	}
	else
	{
		for(unsigned int i=0; i<oa1.length(); i++)
			criticalAlarmEvent.push_back(oa1[i]);
		
		TA_IRS_App::AtsMethodOA1Table atsMethodOA1Table(criticalAlarmEvent);
		atsMethodOA1Table.getLogText(hexText,consoleTxt);
		try
		{
			m_server->sendMethod(atsMethodOA1Table);
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
					"\n\n\t\t\t****** CRITICAL ALARM ******\n\n"
					"\t\t\tFROM : updateAtsMethodOa1Table\n"
					"\t\t\tTrain ID      : %X\n"
					"\t\t\tData Received : %s\n\n",criticalAlarmEvent[2],hexText.c_str());

		}catch(...)
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"The transaction Timeout");
		}
		m_oa1Valid = true;
	}

	FUNCTION_EXIT;
}
