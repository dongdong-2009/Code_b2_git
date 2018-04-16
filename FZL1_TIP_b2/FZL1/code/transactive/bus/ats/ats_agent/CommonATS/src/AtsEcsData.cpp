/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/4669_Review_Branch/4669/transactive/bus/ats/ats_agent/CommonATS/src/AtsEcsData.cpp $
  * @author:   Darren Sampson
  * @version:  $Revision: #5 $
  *
  * Last modification: $DateTime: 2011/06/13 12:14:52 $
  * Last modified by:  $Author: bihui.luo $
  *
  * This represents the ATS ECS Data
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include <time.h>
#include <vector>

#include "core/utilities/src/TAAssert.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/types/ATSAgentComms_MessageTypes.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"

#include "bus/ats/ats_agent/CommonATS/src/AddressUtil.h"
#include "bus/ats/ats_agent/CommonATS/src/AtsEcsData.h"

namespace TA_IRS_Bus
{

	AtsEcsData::AtsEcsData(	TA_IRS_Bus::PhysicalTrainNumberType physicalTrainNumber, unsigned long entityKey, 
									unsigned long locationKey, TA_IRS_Core::IAtsFixedBlockMap& stationZones, 
									TA_IRS_Core::IAtsFixedBlockMap& ecsZones,
									std::vector<unsigned long>& stationsInThisSector)
	: 
	m_ecsInfoChanged (true),
	m_commsMessageSender (NULL),
	m_entityKey (entityKey), 
	m_locationKey (locationKey),
	m_stationZones (stationZones),
	m_ecsZones (ecsZones),
	m_recordImported ( false ),
	m_stalled ( false ),
	m_stationsInThisSector(stationsInThisSector)
	{
		// Train Identification
		m_atsEcsMessage.physicalTrainNumber = physicalTrainNumber;
		m_atsEcsMessage.atcTrainNumber = 0;

		m_atsEcsMessage.trainValid = false;

		// Localisation
		m_atsEcsMessage.localisation.valid = false;
		m_atsEcsMessage.localisation.serviceNumber = 0;
		m_atsEcsMessage.localisation.scheduleNumber = 0;
		m_atsEcsMessage.localisation.stationZoneId = 0;
		m_atsEcsMessage.localisation.lastStationZoneId = 0;
		m_atsEcsMessage.localisation.ecsZoneId = 0;
		m_atsEcsMessage.localisation.stallTimeInSeconds = 0;
			
		// OA1
		m_atsEcsMessage.oa1Data.valid = false;
		m_atsEcsMessage.oa1Data.isInEmergency = false;
		m_atsEcsMessage.oa1Data.maxExternalTemperatureInCelsius = 0;

		// get a handle to CommsMessageSender
		m_commsMessageSender =
			TA_Base_Core::MessagePublicationManager::getInstance().getCommsMessageSender ( TA_Base_Core::ATSAgentComms::Context );
	};

	AtsEcsData::~AtsEcsData()
	{
		// clean up comms message sender
		if ( m_commsMessageSender != NULL )
		{
			delete m_commsMessageSender;
			m_commsMessageSender = NULL;
		}
	}

	TA_IRS_Bus::IAtsEcsCorbaDef::AtsEcsMessageCorbaDef AtsEcsData::buildAtsEcsMessage()
	{	
		return m_atsEcsMessage;
	}

	void AtsEcsData::importAtsEcsMessage(const TA_IRS_Bus::IAtsEcsCorbaDef::AtsEcsMessageCorbaDef& ecsMessage)
	{
		updateAtcTrainNumber(ecsMessage.atcTrainNumber);

		updateTrainValidity(ecsMessage.trainValid);

		if (ecsMessage.localisation.valid)
		{
			updateServiceNumber(ecsMessage.localisation.serviceNumber);
			updateScheduleNumber(ecsMessage.localisation.scheduleNumber);
			updateZoneId(ecsMessage.localisation.stationZoneId,
						 ecsMessage.localisation.ecsZoneId);
			updateStallTime(ecsMessage.localisation.stallTimeInSeconds);

			if (inThisSector(ecsMessage.localisation.stationZoneId))
			{
				updateLocalisationValidity(true);
			}
			else
			{
				updateTrainValidity(false);
				updateLocalisationValidity(false);
			}
		}
		else
		{
			updateLocalisationValidity(false);
		}

		
		if(ecsMessage.oa1Data.valid)
		{
			updateEmergency(ecsMessage.oa1Data.isInEmergency);
			updateExternalTemperatureInCelsius(ecsMessage.oa1Data.maxExternalTemperatureInCelsius);
			updateOa1Validity(true);
		}
		else
		{
			updateOa1Validity(false);
		}
	}

	bool AtsEcsData::inThisSector(unsigned long stationId)
	{
		std::vector<unsigned long>::iterator it;

		for (it = m_stationsInThisSector.begin(); it != m_stationsInThisSector.end(); it++)
		{
			if (stationId == (*it))
			{ 
				return true;
			}
		}

		return false;
	}

	void AtsEcsData::updateAtcTrainNumber(TA_IRS_Bus::AtcTrainNumberType atcTrainNumber)
	{
		if (atcTrainNumber != m_atsEcsMessage.atcTrainNumber)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                "updateAtcTrainNumber() - train %d number, was %d now %d", 
				m_atsEcsMessage.physicalTrainNumber, m_atsEcsMessage.atcTrainNumber, atcTrainNumber);

			m_atsEcsMessage.atcTrainNumber = atcTrainNumber;

			m_ecsInfoChanged = true;
		}
	}

	void AtsEcsData::updateLocalisationValidity(bool valid)
	{	
		if (m_atsEcsMessage.localisation.valid != valid)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                "updateLocalisationValidity() - train %d validity, was %s now %s", 
				m_atsEcsMessage.physicalTrainNumber, (m_atsEcsMessage.localisation.valid?"valid":"invalid"), (valid?"valid":"invalid"));

			m_atsEcsMessage.localisation.valid = valid;

			m_ecsInfoChanged = true;
		}
	}

	void AtsEcsData::updateServiceNumber(TA_IRS_Bus::ServiceNumberType serviceNumber)
	{
		if (serviceNumber != m_atsEcsMessage.localisation.serviceNumber)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                "updateServiceNumber() - train %d number, was %d now %d", 
				m_atsEcsMessage.physicalTrainNumber, m_atsEcsMessage.localisation.serviceNumber, serviceNumber);

			m_atsEcsMessage.localisation.serviceNumber = serviceNumber;

			m_ecsInfoChanged = true;
		}
	}

	void AtsEcsData::updateScheduleNumber(TA_IRS_Bus::ScheduleNumberType scheduleNumber)
	{
		if (scheduleNumber != m_atsEcsMessage.localisation.scheduleNumber)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                "updateScheduleNumber() - train %d number, was %d now %d", 
				m_atsEcsMessage.physicalTrainNumber, m_atsEcsMessage.localisation.scheduleNumber, scheduleNumber);

			m_atsEcsMessage.localisation.scheduleNumber = scheduleNumber;

			m_ecsInfoChanged = true;
		}
	}

	void AtsEcsData::updateZoneId(unsigned long fixedBlocksBranchId,
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
			if (!m_atsEcsMessage.localisation.valid || m_atsEcsMessage.localisation.stationZoneId != 0)
			{
				std::string text = "Station Zone ID DatabaseException - " + std::string(e.what());
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
							"updateZoneId() - train %d - %s", 
							m_atsEcsMessage.physicalTrainNumber, text.c_str());
			}
			stationZoneId = 0;
		}
		catch (TA_Base_Core::DataException& e)
		{
			if (!m_atsEcsMessage.localisation.valid || m_atsEcsMessage.localisation.stationZoneId != 0)
			{
				std::string text = "Station Zone ID DataException -" + std::string(e.what());
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
							"updateZoneId() - train %d - %s", 
							m_atsEcsMessage.physicalTrainNumber, text.c_str());
			}
			stationZoneId = 0;
		}

		unsigned long ecsZoneId = 0;

		try
		{
			ecsZoneId = (unsigned long)m_ecsZones.getZoneId(fixedBlocksBranchId, fbAbscissaInTheBranch, headCarAbscissaOnFb);
		}
		catch (TA_Base_Core::DatabaseException& e)
		{
			if (!m_atsEcsMessage.localisation.valid || m_atsEcsMessage.localisation.ecsZoneId != 0)
			{
				std::string text = "ECS Zone ID DatabaseException -" + std::string(e.what());
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
							"updateZoneId() - train %d - %s", 
							m_atsEcsMessage.physicalTrainNumber, text.c_str());
			}
			ecsZoneId = 0;
		}
		catch (TA_Base_Core::DataException& e)
		{
			if (!m_atsEcsMessage.localisation.valid || m_atsEcsMessage.localisation.ecsZoneId != 0)
			{
				std::string text = "ECS Zone ID DataException -" + std::string(e.what());
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
							"updateZoneId() - train %d - %s", 
							m_atsEcsMessage.physicalTrainNumber, text.c_str());
			}
			ecsZoneId = 0;
		}

		updateZoneId(stationZoneId, ecsZoneId);
	}

	void AtsEcsData::updateZoneId(unsigned long stationZoneId, unsigned long ecsZoneId)
	{
		if (stationZoneId != m_atsEcsMessage.localisation.stationZoneId)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                "updateZoneId() - train %d currentLocation, was %d now %d", 
				m_atsEcsMessage.physicalTrainNumber, m_atsEcsMessage.localisation.stationZoneId, stationZoneId);

			m_atsEcsMessage.localisation.lastStationZoneId = m_atsEcsMessage.localisation.stationZoneId;
			m_atsEcsMessage.localisation.stationZoneId = stationZoneId;
			m_ecsInfoChanged = true;
		}

		if (ecsZoneId != m_atsEcsMessage.localisation.ecsZoneId)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                "updateZoneId() - train %d ECS zone, was %d now %d", 
				m_atsEcsMessage.physicalTrainNumber, m_atsEcsMessage.localisation.ecsZoneId, ecsZoneId);

			m_atsEcsMessage.localisation.ecsZoneId = ecsZoneId;
			m_ecsInfoChanged = true;
		}
	}

	void AtsEcsData::updateStalled(bool stalled)
	{
		if (m_stalled != stalled)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                "updateStalled() - train %d stall, was %s now %s", 
				m_atsEcsMessage.physicalTrainNumber, m_stalled?"stalled":"not stalled", (!m_stalled)?"stalled":"not stalled");

			m_stalled = stalled;

			if (m_stalled)
			{
				time(&m_stallTime);
			}
			else
			{
				m_stallTime = 0;
			}

			m_atsEcsMessage.localisation.stallTimeInSeconds = 0;
			m_ecsInfoChanged = true;
		}
	}

	void AtsEcsData::updateOa1Validity(bool valid)
	{	
		if (m_atsEcsMessage.oa1Data.valid != valid)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                "updateOa1Validity() - train %d validity, was %s now %s", 
				m_atsEcsMessage.physicalTrainNumber, (m_atsEcsMessage.oa1Data.valid?"valid":"invalid"), (valid?"valid":"invalid"));

			m_atsEcsMessage.oa1Data.valid = valid;

			m_ecsInfoChanged = true;
		}
	}

	void AtsEcsData::updateEmergency(bool emergency)
	{
		if (emergency != m_atsEcsMessage.oa1Data.isInEmergency)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                "updateEmergency() - train %d emergency, was %s now %s", 
				m_atsEcsMessage.physicalTrainNumber, (m_atsEcsMessage.oa1Data.isInEmergency?"emergency":"no emergency"), 
				(emergency?"emergency":"no emergency"));

			m_atsEcsMessage.oa1Data.isInEmergency = emergency;

			m_ecsInfoChanged = true;
		}
	};

	void AtsEcsData::updateExternalTemperatureInCelsius(unsigned long externalTemperature)
	{
		if (externalTemperature != m_atsEcsMessage.oa1Data.maxExternalTemperatureInCelsius)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                "updateExternalTemperatureInCelsius() - train %d temperature, was %d now %d", 
				m_atsEcsMessage.physicalTrainNumber, m_atsEcsMessage.oa1Data.maxExternalTemperatureInCelsius, externalTemperature);

			m_atsEcsMessage.oa1Data.maxExternalTemperatureInCelsius = externalTemperature;

			m_ecsInfoChanged = true;
		}
	};

	
	// The stallTimeInSeconds attribute will be set based on the 
	// <Stalled in inter-station> field.  The ATS Agent will be responsible
	// for keeping track of stalled trains and how long they are stalled
	// for.
	void AtsEcsData::updateStallTime()
	{
		m_atsEcsMessage.localisation.stallTimeInSeconds = 0;

		if (m_stalled)
		{
			time_t currentTime = time(NULL);

			if (m_atsEcsMessage.localisation.stallTimeInSeconds != (unsigned long)(currentTime - m_stallTime))
			{
				m_atsEcsMessage.localisation.stallTimeInSeconds = (unsigned long)(currentTime - m_stallTime);
				m_ecsInfoChanged = true;
			}
		}
	};

	void AtsEcsData::updateStallTime(unsigned long stallTime)
	{
		if (m_stalled)
		{
			time_t currentTime = time(NULL);

			m_stallTime = currentTime - stallTime;

			if (m_atsEcsMessage.localisation.stallTimeInSeconds != stallTime)
			{
				m_atsEcsMessage.localisation.stallTimeInSeconds = stallTime;
				m_ecsInfoChanged = true;
			}
		}
	};

	void AtsEcsData::clearImportFlag()
	{
		m_recordImported = false;
	}

	void AtsEcsData::setTrainValidity()
	{	
		if (!m_atsEcsMessage.trainValid)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                "setTrainValidity() - train %d validity, was invalid now valid", 
				m_atsEcsMessage.physicalTrainNumber);

			m_atsEcsMessage.trainValid = true;
			m_ecsInfoChanged = true;
		}

		m_recordImported = true;
	}

	void AtsEcsData::updateTrainValidity(bool valid)
	{	
		if (m_atsEcsMessage.trainValid != valid)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                "updateTrainValidity() - train %d validity, was %s now %s", 
				m_atsEcsMessage.physicalTrainNumber, (m_atsEcsMessage.trainValid?"valid":"invalid"), (valid?"valid":"invalid"));

			m_atsEcsMessage.trainValid = valid;
			m_ecsInfoChanged = true;

			if (!valid)
			{
				// Invalidate localisation and OA1 as well
				updateLocalisationValidity(false);
				updateOa1Validity(false);
			}
		}
	}

	void AtsEcsData::checkIfTrainNotImported()
	{
		if (!m_recordImported && m_atsEcsMessage.trainValid)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                "checkIfTrainNotImported() - not updated, train %d validity, was valid now invalid", 
				m_atsEcsMessage.physicalTrainNumber);

			m_atsEcsMessage.trainValid = false;
		
			// Invalidate localisation and OA1 as well
			updateLocalisationValidity(false);
			updateOa1Validity(false);
		}
	}

	bool AtsEcsData::isUpdated()
	{
		updateStallTime();

		return m_ecsInfoChanged;
	} 

	bool AtsEcsData::isValid()
	{
		return m_atsEcsMessage.trainValid;
	}

	void AtsEcsData::sendUpdates()
	{
		if (isUpdated())
		{
			//TA_IRS_Bus::IAtsEcsCorbaDef::AtsEcsMessageCorbaDef* ecsMessage 
			//	= new TA_IRS_Bus::IAtsEcsCorbaDef::AtsEcsMessageCorbaDef(buildAtsEcsMessage());

			TA_IRS_Bus::IAtsEcsCorbaDef::AtsEcsMessageCorbaDef ecsMessage(buildAtsEcsMessage());

			CORBA::Any data;
				data <<= ecsMessage;

			m_commsMessageSender->sendCommsMessage ( TA_Base_Core::ATSAgentComms::ECSUpdate,
													m_entityKey,
													data,
													0,
													m_locationKey);

			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
					"sendUpdates() - Info: AtsEcsMessage sent - train=%d, entity=%d, location=%d, localisation=%s, zone=%d, oa1=%s, emergency=%s",
					m_atsEcsMessage.physicalTrainNumber, m_entityKey, m_locationKey,
					(m_atsEcsMessage.localisation.valid?"VALID":"INVALID"), 
					m_atsEcsMessage.localisation.ecsZoneId,
					(m_atsEcsMessage.oa1Data.valid?"VALID":"INVALID"),
					(m_atsEcsMessage.oa1Data.isInEmergency?"YES":"NO"));

			m_ecsInfoChanged = false;
		}
	}
}
