/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/4669_Review_Branch/4669/transactive/bus/ats/ats_agent/CommonATS/src/AtsPsdData.cpp $
  * @author:   Darren Sampson
  * @version:  $Revision: #3 $
  *
  * Last modification: $DateTime: 2010/03/30 11:17:43 $
  * Last modified by:  $Author: grace.koh $
  *
  * This represents the ATS PSD Data
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include <time.h>

#include "core/utilities/src/TAAssert.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/ats/src/AtsPSDData.h"
#include "core/exceptions/src/AtsAgentException.h"
#include "core/exceptions/src/FatalConfigurationException.h"
#include "core/message/types/AtsAgentAlarms_MessageTypes.h"

#include "bus/scada/datapoint_library/src/DataPoint.h"

#include "bus/ats/ats_agent/CommonATS/src/AddressUtil.h"
#include "bus/ats/ats_agent/CommonATS/src/DataPointHelper.h"
#include "bus/ats/ats_agent/CommonATS/src/AtsPsdData.h"

using TA_Base_Core::AtsAgentAlarms::AtsPsdDataSend;

namespace TA_IRS_Bus
{
	const std::string ATS_TRAIN_PSD_CLOSED_AND_LOCKED	= "PsdClosedAndLocked";
	const std::string ISCS_PLATFORM_CLOSE_REQUEST		= "CloseRequest";
	const std::string ISCS_PLATFORM_OPEN_REQUEST		= "OpenRequest";

	AtsPsdData::AtsPsdData(	TA_IRS_Bus::PlatformIdType platformId, unsigned long stationId,
							unsigned long entityKey, unsigned long locationKey,
							TA_Base_Core::AtsAgentEntityData::EAtsLocationType locationType,
							DataPointHelper& dpHelper)
	:
	m_psdClosePreWarningChanged (false),
	m_entityKey (entityKey), 
	m_locationKey (locationKey),
	m_locationType (locationType),
	m_psdPreCloseWarningDp (0),
	m_dpHelper(dpHelper)
	{
		m_atsPsdMessage.platformId = platformId;
		m_atsPsdMessage.psdClosePreWarning = false;
		m_atsPsdMessage.psdClosedAndLocked = false;

		for (unsigned int i = 0; i < 12; i++)
		{
			m_atsPsdMessage.psdDoorInhibitionStatus[i] = false;
		}

		m_dataPoints.clear();

		m_changedPsdInhibitStatus.clear();
		m_psdDoorInhibitDp.clear();		
	};

	AtsPsdData::~AtsPsdData()
	{
		m_psdDoorInhibitDp.clear();
	}

	bool AtsPsdData::attachDataPoint(TA_Base_Bus::DataPoint* dataPoint)
	{
		std::string address = dataPoint->getDataPointAddress();

		bool found = checkAttribute(address);
		
		if (found)
		{
			m_dataPoints.push_back(dataPoint);
			updateDataPoint(dataPoint);
		}

		return found;
	}

	bool AtsPsdData::checkAttribute(std::string address)
	{
		std::string attribute = AddressUtil::getTableRecordAttribute(address);

		// PSD Closed and Locked
		if (AddressUtil::caseInsensitiveCompare(attribute, ATS_TRAIN_PSD_CLOSED_AND_LOCKED) ||
			AddressUtil::caseInsensitiveCompare(attribute, ISCS_PLATFORM_CLOSE_REQUEST) ||
			AddressUtil::caseInsensitiveCompare(attribute, ISCS_PLATFORM_OPEN_REQUEST))
		{
			std::vector<std::string> addressParts = AddressUtil::splitColonSeparatedString(address);

			// Parts are table, key, attribute
			if (addressParts.size() == 3)
			{
				return true;
			}
			else
			{
				// LOG
				return false;
			}
		}

		return false;
	}

	void AtsPsdData::addPsdData(TA_IRS_Core::IAtsPSDData* psdData)
	{
		TA_ASSERT(m_locationType == TA_Base_Core::AtsAgentEntityData::SecondaryStation1 || 
				  m_locationType == TA_Base_Core::AtsAgentEntityData::SecondaryStation2 ||
				  m_locationType == TA_Base_Core::AtsAgentEntityData::MainStation,
				  "Fatal: Can not add PSD to non-station Platform Record")
	
		if (psdData->getPSDDataPointNameOfType() == TA_IRS_Core::AtsPSDData::OUTPUT_DOOR_INHIBIT_STATE)
		{
			try
			{
				m_dpHelper.addOutputBooleanDataPoint(psdData->getPSDDataPointEntityKey(), TA_Base_Core::AtsAgentAlarms::AtsPsdDataSend);
			}
			catch (TA_Base_Core::AtsAgentException& e)
			{
				std::string text = "addPsdData() - " + std::string(e.what());
				LOG_EXCEPTION_CATCH(SourceInfo, "AtsAgentException", text);

				std::ostringstream message;
				message.clear();
				message << "addPsdData() - OUTPUT_DOOR_INHIBIT_STATE DataPoint with entity key ";
				message << psdData->getPSDDataPointEntityKey();
				message << " not available for \"";
				message << psdData->getPSDDataPointNameOfType();
				message << "\".";

				TA_THROW(TA_Base_Core::AtsAgentException(message.str()));
			}

			std::map<long, unsigned long>::iterator dpFnd = m_psdDoorInhibitDp.find(psdData->getPSDNumber());

			if(dpFnd != m_psdDoorInhibitDp.end())
			{
				TA_THROW(TA_Base_Core::FatalConfigurationException(
							"Configuration Error: PSD Door Inhibit DataPoint multiply defined"));
			}

			m_psdDoorInhibitDp[psdData->getPSDNumber()] = psdData->getPSDDataPointEntityKey();
		}
		else if (psdData->getPSDDataPointNameOfType() == TA_IRS_Core::AtsPSDData::OUTPUT_PRE_CLOSE_WARNING)
		{
			try
			{
				m_dpHelper.addOutputBooleanDataPoint(psdData->getPSDDataPointEntityKey(), TA_Base_Core::AtsAgentAlarms::AtsPsdDataSend);
			}
			catch (TA_Base_Core::AtsAgentException& e)
			{
				std::string text = "addPsdData() - " + std::string(e.what());
				LOG_EXCEPTION_CATCH(SourceInfo, "AtsAgentException", text);

				std::ostringstream message;
				message.clear();
				message << "addPsdData() - OUTPUT_DOOR_INHIBIT_STATE DataPoint with entity key ";
				message << psdData->getPSDDataPointEntityKey();
				message << " not available for \"";
				message << psdData->getPSDDataPointNameOfType();
				message << "\".";

				TA_THROW(TA_Base_Core::AtsAgentException(message.str()));
			}

			if(m_psdPreCloseWarningDp != 0)
			{
				TA_THROW(TA_Base_Core::FatalConfigurationException(
							"Configuration Error: PSD Pre Close Warning DataPoint multiply defined"));
			}

			m_psdPreCloseWarningDp = psdData->getPSDDataPointEntityKey();
		}
		else if (psdData->getPSDDataPointNameOfType() == TA_IRS_Core::AtsPSDData::INPUT_DOOR_STATUS)
		{
			try
			{
				unsigned long entityKey = psdData->getPSDDataPointEntityKey();

				if (!m_dpHelper.isDataPointAdded(entityKey))
				{
					m_dpHelper.addInputBooleanDataPoint(entityKey);
				}
				m_psdDoorStatusDp[psdData->getPSDNumber()] = entityKey;
				m_psdDoorNumToStatus[psdData->getPSDNumber()] = false; //TD15264
			}
			catch (TA_Base_Core::AtsAgentException&)
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
					"addPsdData() - Error: Adding INPUT_DOOR_STATUS DataPoint for entity with key %d",
					psdData->getPSDDataPointEntityKey());
			}
			catch (...)
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
					"addPsdData() - Error: Unknown error adding INPUT_DOOR_STATUS Datapoint for entity with key %d",
					psdData->getPSDDataPointEntityKey());
			}
		}
		else
		{
			TA_THROW(TA_Base_Core::FatalConfigurationException(
						"Configuration Error: Unknown PSD DataPoint"));
		}
	}

	bool AtsPsdData::configurationComplete()
	{
		bool configComplete = true;
		int i;

		for (i = 1; i <=12; i++)
		{
			std::map<long, unsigned long>::iterator it = m_psdDoorInhibitDp.find(i);

			if (it == m_psdDoorInhibitDp.end())
			{
				configComplete = false;
			}
		}

		for (i = 1; i <=12; i++)
		{
			std::map<long, unsigned long>::iterator it = m_psdDoorStatusDp.find(i);

			if (it == m_psdDoorStatusDp.end())
			{
				configComplete = false;
			}
		}

		if (m_psdPreCloseWarningDp == 0)
		{
			configComplete = false;
		}

		return configComplete;
	}

	void AtsPsdData::updatePsdClosePreWarning(bool closePreWarning)
	{
        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
                            "AtsPsdData::updatePsdClosePreWarning alarmTypeKey = %s - LICHAO", AtsPsdDataSend.getTypeKey().c_str());
         
		if (m_atsPsdMessage.psdClosePreWarning != closePreWarning)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
                "updatePsdClosePreWarning() - platform %d closePreWarning, was %s now %s", 
				m_atsPsdMessage.platformId, 
				m_atsPsdMessage.psdClosePreWarning?"set":"not set", 
				closePreWarning?"set":"not set");

			m_atsPsdMessage.psdClosePreWarning = closePreWarning;
			
			m_psdClosePreWarningChanged = true;
		}
	}

	void AtsPsdData::updatePsdClosedAndLocked(bool psdClosedAndLocked)
	{
		if (m_atsPsdMessage.psdClosedAndLocked != psdClosedAndLocked)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
                "updatePsdClosedAndLocked() - platform %d psdClosedAndLocked, was %s now %s", 
				m_atsPsdMessage.platformId, 
				m_atsPsdMessage.psdClosedAndLocked?"set":"not set", 
				psdClosedAndLocked?"set":"not set");

			m_atsPsdMessage.psdClosedAndLocked = psdClosedAndLocked;
		}
	}

	void AtsPsdData::updatePsdInhibitionStatus(unsigned int doorIndex, bool inhibit)
	{
		if (m_atsPsdMessage.psdDoorInhibitionStatus[doorIndex] != inhibit)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
                "updatePsdInhibitionStatus() - platform %d door %d inhibit, was %s now %s", 
				m_atsPsdMessage.platformId, doorIndex+1, 
				m_atsPsdMessage.psdDoorInhibitionStatus[doorIndex]?"set":"not set", 
				inhibit?"set":"not set");

			m_atsPsdMessage.psdDoorInhibitionStatus[doorIndex] = inhibit;

			m_changedPsdInhibitStatus.push_back(doorIndex);
            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
                "AtsPsdData::updatePsdInhibitionStatus() inhibit changed/ doorIndex = %ld, inhibit = %s - LICHAO", doorIndex, inhibit?"true":"false");
		}
	}

	TA_IRS_Bus::PlatformIdType AtsPsdData::getPlatformId()
	{
		return m_atsPsdMessage.platformId;
	}

	void AtsPsdData::processUpdates()
	{
		std::vector<TA_IRS_Bus::IAtsPsdCorbaDef::AtsPsdMessageCorbaDef> psdUpdates;

		//
		// Execute PSD DataPoint Controls
		//
		if (m_locationType == TA_Base_Core::AtsAgentEntityData::SecondaryStation1 || 
			m_locationType == TA_Base_Core::AtsAgentEntityData::SecondaryStation2 ||
			m_locationType == TA_Base_Core::AtsAgentEntityData::MainStation)
		{
			bool setAlarm = false;
			bool sendAlarm = false;

			// <PSD close pre-warning>
			if (m_psdClosePreWarningChanged)
			{
				setAlarm = true;

				// Start/stop the pre-close warning
				if (m_psdPreCloseWarningDp != 0)
				{
					try
					{
                        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
                            "AtsPsdData::processUpdates() m_psdPreCloseWarningDp - LICHAO");
						if (m_dpHelper.setBooleanDataPointValue(m_psdPreCloseWarningDp, m_atsPsdMessage.psdClosePreWarning))
						{
							LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
								 "processUpdates() - Platform %d Pre-Close Warning set to %s",
								 m_atsPsdMessage.platformId, (m_atsPsdMessage.psdClosePreWarning?"TRUE":"FALSE"));
						}
						else
						{
							LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
								 "processUpdates() - Error: Failed to set Platform %d Pre-Close Warning to %s",
								 m_atsPsdMessage.platformId, (m_atsPsdMessage.psdClosePreWarning?"TRUE":"FALSE"));
							
							sendAlarm = true;
						}
					}
					catch (TA_Base_Core::FatalConfigurationException &)
					{
						std::ostringstream text;
						text << "processUpdates() - Error: Bad Configuration for datapoint for platform ";
						text << (unsigned long)m_atsPsdMessage.platformId << " pre close warning";
						LOG_EXCEPTION_CATCH(SourceInfo, "FatalConfigurationException", text.str());

						sendAlarm = true;
					}
				}
				else
				{
					LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
						 "processUpdates() - Error: PSD Pre-Close Warning %s DataPoint not defined for Platform %d - DataPoint not written",
						 m_atsPsdMessage.psdClosePreWarning?"Start":"Stop", m_atsPsdMessage.platformId);

					sendAlarm = true;
				}

				m_psdClosePreWarningChanged = false;
			}

			std::vector<unsigned long>::iterator indexIt;

			for (indexIt = m_changedPsdInhibitStatus.begin(); indexIt != m_changedPsdInhibitStatus.end(); indexIt++)
			{
				unsigned long changedIndex = *indexIt;
				setAlarm = true;

				// Inhibit or uninhibit door
                std::map<long, unsigned long >::iterator dpFnd = m_psdDoorInhibitDp.find(changedIndex+1);
                if (dpFnd != m_psdDoorInhibitDp.end())
                {
					try
					{
                        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
                            "AtsPsdData::processUpdates() m_changedPsdInhibitStatus  psdNum = %ld , entityKey = %ld - LICHAO", (*dpFnd).first, (*dpFnd).second );
						if (m_dpHelper.setBooleanDataPointValue((*dpFnd).second, m_atsPsdMessage.psdDoorInhibitionStatus[changedIndex]))
						{
							LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
								 "processUpdates() - Platform %d, PSD %d Uninhibit set to %s",
								 m_atsPsdMessage.platformId, changedIndex+1, (m_atsPsdMessage.psdDoorInhibitionStatus[changedIndex]?"TRUE":"FALSE"));
						}
						else
						{
							LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
								 "processUpdates() - Error: Failed to set Platform %d, PSD %d Uninhibit to %s",
								 m_atsPsdMessage.platformId, changedIndex+1, (m_atsPsdMessage.psdDoorInhibitionStatus[changedIndex]?"TRUE":"FALSE"));
						}
					}
					catch (TA_Base_Core::FatalConfigurationException &)
					{
						std::ostringstream text;
						text << "processUpdates() - Error: Bad Configuration for datapoint for platform ";
						text << (unsigned long)m_atsPsdMessage.platformId << " door " << (changedIndex+1);
						LOG_EXCEPTION_CATCH(SourceInfo, "FatalConfigurationException", text.str());

					}
					catch (...)
					{

						LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception",
							"setBooleanDataPointValue() - Exception caught while set DataPoint Value");
					}
				}
				else
				{
					LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
						 "processUpdates() - Error: PSD Door Uninhibit DataPoint not defined for Platform %d, door %d - DataPoint not written",
						 m_atsPsdMessage.platformId, changedIndex+1);

				}
			}

			m_changedPsdInhibitStatus.clear();

// 			if (setAlarm)
// 			{
// 				m_dpHelper.setAlarmState(AtsPsdDataSend.getTypeKey(), sendAlarm);
// 			}
		}

		//
		// Update DataPoints
		//

		std::vector<TA_Base_Bus::DataPoint*>::iterator it;
		for (it = m_dataPoints.begin(); it != m_dataPoints.end(); it++)
		{	
			updateDataPoint(*it);
		}
	}


	TA_IRS_Bus::IAtsPsdCorbaDef::AtsPsdMessageCorbaDef AtsPsdData::buildAtsPsdMessage()
	{
		TA_IRS_Bus::IAtsPsdCorbaDef::AtsPsdMessageCorbaDef message = m_atsPsdMessage;

		return message;
	}

	void AtsPsdData::updateDataPoint(TA_Base_Bus::DataPoint* dataPoint)
	{
		bool configurationError = true;

		std::string address = dataPoint->getDataPointAddress();
		std::string attribute = AddressUtil::getTableRecordAttribute(address);

		// PSD Closed and Locked
		if (AddressUtil::caseInsensitiveCompare(attribute, ATS_TRAIN_PSD_CLOSED_AND_LOCKED) )
		{
			std::vector<std::string> addressParts = AddressUtil::splitColonSeparatedString(address);

			// Parts are table, key, attribute
			if (addressParts.size() == 3)
			{
				// Set the value of the datapoint
				TA_Base_Bus::DpValue dpValue(dataPoint->getDataPointDataType(),
											 dataPoint->getEnumLabelsMap(),
											 dataPoint->getBooleanLabels());
				dpValue.setBoolean ( m_atsPsdMessage.psdClosedAndLocked );
				
				TA_Base_Bus::EQualityStatus dpQuality = TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON;

				TA_Base_Bus::DataPointState newState ( dpValue, time( NULL ), dpQuality );
				
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
                "updateDataPoint() - datapoint %s closed and locked.", 
				dataPoint->getDataPointName().c_str()
				);   //TD15264

				dataPoint->updateFieldState( newState );

				configurationError = false;
			}
		}

		if (AddressUtil::caseInsensitiveCompare(attribute, ISCS_PLATFORM_CLOSE_REQUEST) ||
			AddressUtil::caseInsensitiveCompare(attribute, ISCS_PLATFORM_OPEN_REQUEST))
		{
			configurationError = false;
		}


		if (configurationError)
		{
			TA_Base_Bus::DpValue dpValue(dataPoint->getDataPointDataType(),
										 dataPoint->getEnumLabelsMap(),
										 dataPoint->getBooleanLabels());
			dpValue.setBoolean ( false );
			TA_Base_Bus::DataPointState newState ( dpValue, time( NULL ), TA_Base_Bus::QUALITY_BAD_CONFIGURATION_ERROR );

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
                "updateDataPoint() - datapoint %s quality bad due to configuration error.", 
				dataPoint->getDataPointName().c_str()
				);                //TD15264
			dataPoint->updateFieldState( newState );
		}
	}

	void AtsPsdData::processUpdatesWhenDoubleATSFail()
	{
		std::vector<TA_Base_Bus::DataPoint*>::iterator it;
		for (it = m_dataPoints.begin(); it != m_dataPoints.end(); it++)
		{	
			updateDataPointWhenDoubleATSFail(*it);
		}
	}

	void AtsPsdData::updateDataPointWhenDoubleATSFail(TA_Base_Bus::DataPoint* dataPoint)
	{
		bool configurationError = true;

		std::string address = dataPoint->getDataPointAddress();
		std::string attribute = AddressUtil::getTableRecordAttribute(address);

		// PSD Closed and Locked
		if (AddressUtil::caseInsensitiveCompare(attribute, ATS_TRAIN_PSD_CLOSED_AND_LOCKED) )
		{
			std::vector<std::string> addressParts = AddressUtil::splitColonSeparatedString(address);

			// Parts are table, key, attribute
			if (addressParts.size() == 3)
			{
				// Initialise the datapoint to not connected status
				TA_Base_Bus::DpValue dpValue = dataPoint->getCurrentValue();
    
				TA_Base_Bus::DataPointState newState ( dpValue, time( NULL ), TA_Base_Bus::QUALITY_BAD_NOT_CONNECTED );
				dataPoint->updateFieldState( newState );


				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
                "updateDataPoint() - datapoint %s closed and locked.", 
				dataPoint->getDataPointName().c_str());

				configurationError = false;
			}
		}

		if (AddressUtil::caseInsensitiveCompare(attribute, ISCS_PLATFORM_CLOSE_REQUEST) ||
			AddressUtil::caseInsensitiveCompare(attribute, ISCS_PLATFORM_OPEN_REQUEST))
		{
			configurationError = false;
		}


		if (configurationError)
		{
			TA_Base_Bus::DpValue dpValue(dataPoint->getDataPointDataType(),
										 dataPoint->getEnumLabelsMap(),
										 dataPoint->getBooleanLabels());
			dpValue.setBoolean ( false );
			TA_Base_Bus::DataPointState newState ( dpValue, time( NULL ), TA_Base_Bus::QUALITY_BAD_CONFIGURATION_ERROR );

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
                "updateDataPoint() - datapoint %s quality bad due to configuration error.", 
				dataPoint->getDataPointName().c_str()
				);                //TD15264
			dataPoint->updateFieldState( newState );
		}
	}

	bool AtsPsdData::getDoorStatus(unsigned int index)
	{
		bool doorStatus = false;

		std::map<long, unsigned long>::iterator it = m_psdDoorStatusDp.find(index+1);
		std::map<long, bool>::iterator it2 = m_psdDoorNumToStatus.find(index+1);

		if (( it != m_psdDoorStatusDp.end() ) && ( it2 != m_psdDoorNumToStatus.end()) ) //TD15264
		{
			try
			{
				if (m_dpHelper.isBooleanDataPointQualityGood((*it).second))
				{
					doorStatus = m_dpHelper.getBooleanDataPointValue((*it).second);
				}
			}
			catch (TA_Base_Core::FatalConfigurationException &)
			{
				std::ostringstream text;
				text << "getDoorStatus() - Error: Bad Configuration for datapoint for platform ";
				text << (unsigned long)m_atsPsdMessage.platformId << " door " << (index+1);
				LOG_EXCEPTION_CATCH(SourceInfo, "FatalConfigurationException", text.str());
			}
		}
		else
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
				"exportRecord() - Error: No datapoint for platform %d, door %d",
				m_atsPsdMessage.platformId, index+1);
		}
		
		if ( doorStatus != (*it2).second )                                    
		{
			// different with the previous value, log it.
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,                  //TD15264
									"status of Door %d changed from %s to %d.",
									(index+1),
									(*it2).second?"outofservice":"inservice",
									doorStatus?"outofservice":"inservice");
		}

		(*it2).second = doorStatus ;

		return doorStatus;
	}

	bool AtsPsdData::getPsdCloseRequest()
	{
		bool closeRequest = m_psdCloseRequest;
		m_psdCloseRequest = false;

		return closeRequest;
	}

	bool AtsPsdData::getPsdOpenRequest()
	{
		bool openRequest = m_psdOpenRequest;
		m_psdOpenRequest = false;

		return openRequest;
	}

	bool AtsPsdData::checkWriteQueue(TA_Base_Bus::DataPointWriteRequest * writeReq)
	{
		//test++
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "test++ checkWriteQueue");
		//++test

		bool ret = false;

		if ( 0 != writeReq )
		{
			// get the output data point from the write request
			TA_Base_Bus::DataPoint * theDataPoint = 0;
			theDataPoint = writeReq->getDataPoint();
			
			// if successfull to get the output data point
			if ( 0 != theDataPoint )
			{
				// check if the output data point is for this output worker
				if ( true == findDataPoint ( *theDataPoint ) )
				{
					ret = true;
					try
					{
						// make the field state of the output datapoint to be the same as the value
						// being sent ATS.
						TA_Base_Bus::DataPointState newState( writeReq->getValue(), time( NULL ),
							TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON );
						theDataPoint->updateFieldState ( newState );
						
						std::string address = theDataPoint->getDataPointAddress();
						
						std::string attribute = AddressUtil::getTableRecordAttribute(address);
						
						if (AddressUtil::caseInsensitiveCompare(attribute, ISCS_PLATFORM_CLOSE_REQUEST))
						{
							// <Close Request>
							if (newState.getValue().getBoolean() == true)
							{
								m_psdCloseRequest = true;
								LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "checkWriteQueue() - datapoint %s : Close Request.",
									theDataPoint->getDataPointName().c_str()); //TD15264
								theDataPoint->updateControlState( TA_Base_Bus::pending, true );//limin++, SystemSession should not access db
								theDataPoint->startABETimeout( writeReq->getSessionID() );
							}
						}
						else if (AddressUtil::caseInsensitiveCompare(attribute, ISCS_PLATFORM_OPEN_REQUEST))
						{
							// <Open Request>
							if (newState.getValue().getBoolean() == true)
							{
								m_psdOpenRequest = true;
								LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "checkWriteQueue() - datapoint %s : Open Request.",
									theDataPoint->getDataPointName().c_str()); //TD15264
								theDataPoint->updateControlState( TA_Base_Bus::pending, true );//limin++, SystemSession should not access db
								theDataPoint->startABETimeout( writeReq->getSessionID() );
							}
						}
						else
						{
							LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
								"checkWriteQueue() - datapoint %s has invalid address %s", theDataPoint->getDataPointName().c_str(),
								address.c_str());
							theDataPoint->updateControlState( TA_Base_Bus::comms_failed, true );//limin++, SystemSession should not access db
						}
					}
					catch ( ... )
					{
						LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", "checkWriteQueue() - Unknown exception updating datapoint from write queue");
					}
				}
			}
		}
		
		return ret;
	}

	bool AtsPsdData::findDataPoint ( const TA_Base_Bus::DataPoint & theDataPoint )
	{
		bool found = false;

		// for each of the data points assigned to this worker thread
		std::vector < TA_Base_Bus::DataPoint * >::iterator dpIter;
		for ( dpIter = m_dataPoints.begin();
			  dpIter != m_dataPoints.end();
			  dpIter++ )
		{
			// if the names are matched
			if ( (*dpIter)->getDataPointName() == theDataPoint.getDataPointName() )
			{
				// set the flag and exit loop
				found = true;
				break;
			}
		}

		return found;
	}
}
