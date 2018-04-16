/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/PRIV_3001/3001/transactive/app/ats/AtsAgent/src/AtsCctvData.cpp $
  * @author:   Darren Sampson
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last mofified by:  $Author: lim.cy $
  *
  * This represents the ATS Train Record
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include "app/ats/AtsAgent/src/AddressUtil.h"
#include "app/ats/AtsAgent/src/AtsCctvData.h"
#include "app/ats/AtsAgent/src/TrainCctvCamera.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/types/ATSAgentComms_MessageTypes.h"
#include "core/utilities/src/TAAssert.h"

#include <time.h>

#include <sstream>
#include <vector>


namespace TA_IRS_App
{
	const std::string AtsCctvData::OA1_UNDER_FRAME_FIRE				= "UnderFrameFire";
	const std::string AtsCctvData::OA1_PECU_ACTIVATED				= "PecuActivated";
	const std::string AtsCctvData::OA1_EXTINGUISHER_REMOVED			= "ExtinguisherRemoved";
	const std::string AtsCctvData::OA1_EXT_SMOKE_DETECTED			= "ExteriorSmokeDetected";
	const std::string AtsCctvData::OA1_INT_SMOKE_DETECTED			= "InteriorSmokeDetected";
	const std::string AtsCctvData::OA1_UNDER_SEAT_FIRE				= "UnderSeatFire";
	const std::string AtsCctvData::OA1_EHS_ACTIVATED				= "EHSActivated";
	const std::string AtsCctvData::OA1_DD_COVER_OPEN				= "DDCoverOpen";
	const std::string AtsCctvData::OA1_DD_NOT_LOCKED_OR_NOT_CLOSED	= "DDNotLockedOrNotClosed";

	const std::string AtsCctvData::OA2_DRIVER_CONSOLE_COVER_OPEN	= "DriverConsoleCoverOpen";
	const std::string AtsCctvData::OA2_DOOR_OBSTRUCTION				= "DoorObstruction";
	const std::string AtsCctvData::OA2_DOOR_UNKNOWN                 = "DoorUnknown";


	AtsCctvData::AtsCctvData(TA_IRS_Bus::PhysicalTrainNumberType physicalTrainNumber, 
							 unsigned long location,
							 TA_IRS_Core::IAtsFixedBlockMap& stationZones)
		: m_location (location), m_stationZones (stationZones),
		  
		  m_cctvDataChanged (true),
		  m_trainValid (false)
    {
        FUNCTION_ENTRY( "AtsCctvData" );

		m_cctvData.physicalTrainNumber = physicalTrainNumber;
		m_cctvData.serviceNumber = 0;

		m_cctvData.localisation.valid = false;
		m_cctvData.localisation.stationId = 0;
		m_cctvData.localisation.lastStationId = 0;

		m_cctvData.oa1.valid = false;
		m_cctvData.oa2.valid = false;

        m_cameras.clear();

        initialiseOa1DataToUnknown();
        initialiseOa2DataToUnknown();

        FUNCTION_EXIT;
    }


    AtsCctvData::~AtsCctvData()
    {
        FUNCTION_ENTRY( "~AtsCctvData" );
        FUNCTION_EXIT;
    }


    TA_IRS_Bus::IAtsCctvCorbaDef::AtsCctvMessageCorbaDef AtsCctvData::buildAtsCctvMessage()
    {
        FUNCTION_ENTRY( "buildAtsCctvMessage" );
        FUNCTION_EXIT;
        return m_cctvData;
    }


    void AtsCctvData::initialiseOa1DataToUnknown()
    {
        FUNCTION_ENTRY( "initialiseOa1DataToUnknown" );

        unsigned int car;
        unsigned int index;

		// Initialise arrays
		for (car = 0; car < 3; car++)
		{
			m_cctvData.oa1.underFrameFireStatus[car] = TA_IRS_Bus::IAtsCctvCorbaDef::FireSmokeDetectionUnknown;
			m_cctvData.oa1.fireExtinguisherStatus[car] = TA_IRS_Bus::IAtsCctvCorbaDef::ExtinguisherStatusUnknown;
			m_cctvData.oa1.exteriorSmoke[car] = TA_IRS_Bus::IAtsCctvCorbaDef::FireSmokeDetectionUnknown;
			m_cctvData.oa1.interiorSmoke[car] = TA_IRS_Bus::IAtsCctvCorbaDef::InteriorSmokeUnknown;
			m_cctvData.oa1.underSeatFire[car] = TA_IRS_Bus::IAtsCctvCorbaDef::FireSmokeDetectionUnknown;
			
			for (index = 0; index < 8; index++)
			{
				(m_cctvData.oa1.ehsStatus[car])[index] = TA_IRS_Bus::IAtsCctvCorbaDef::EhsStatusUnknown;
            }
        }

        for (car = 0; car < 2; car++)
        {
            m_cctvData.oa1.detrainmentDoorStatus[car] = TA_IRS_Bus::IAtsCctvCorbaDef::DdUnknown;
        }

        FUNCTION_EXIT;
    }


    void AtsCctvData::initialiseOa2DataToUnknown()
    {
        FUNCTION_ENTRY( "initialiseOa2DataToUnknown" );

        unsigned int car;
        unsigned int index;

        // Initialise arrays
        for (car = 0; car < 3; car++)
        {
            for (index = 0; index < 8; index++)
            {
                (m_cctvData.oa2.trainDoorData[car])[index] = TA_IRS_Bus::IAtsCctvCorbaDef::TrainDoorStatusUnknown;
            }
        }

        for (car = 0; car < 2; car++)
        {
            m_cctvData.oa2.driverConsoleCoverStatus[car] = TA_IRS_Bus::IAtsCctvCorbaDef::ConsoleCoverStatusUnknown;
        }

        FUNCTION_EXIT;
    }


    void AtsCctvData::importAtsCctvMessage(const TA_IRS_Bus::IAtsCctvCorbaDef::AtsCctvMessageCorbaDef& cctvUpdate)
    {
        FUNCTION_ENTRY( "importAtsCctvMessage" );

        if (cctvUpdate.localisation.valid)
        {
            updateZoneId(cctvUpdate.localisation.stationId);
			updateLocalisationValidity(true);
		}
		else
		{
			updateLocalisationValidity(false);
		}

		updateServiceNumber(cctvUpdate.serviceNumber);

		if (cctvUpdate.oa1.valid)
		{
	  		unsigned int car;
			
			for (car = 0; car < 3; car++)
			{
				updateUnderFrameFireStatus(car, cctvUpdate.oa1.underFrameFireStatus[car]);
				updateFireExtinguisherStatus(car, cctvUpdate.oa1.fireExtinguisherStatus[car]);
				updateExteriorSmokeDetectionStatus(car, cctvUpdate.oa1.exteriorSmoke[car]);
				updateInteriorSmokeDetectionStatus(car, cctvUpdate.oa1.interiorSmoke[car]);
				updateUnderSeatFireStatus(car, cctvUpdate.oa1.underSeatFire[car]);

				int door;
				for (door = 0; door < 8; door++)
				{
					updateEhsStatus(car, door, cctvUpdate.oa1.ehsStatus[car][door]);
				}
			}

			for (car = 0; car < 2; car++)
			{
				updateDetrainmentDoorStatus(car, cctvUpdate.oa1.detrainmentDoorStatus[car]);
			}

			updateOa1Validity(true);
		}
		else
		{
			updateOa1Validity(false);
		}

		if (cctvUpdate.oa2.valid)
		{
			unsigned int car;
			
			for (car = 0; car < 3; car++)
			{
				int door;
				for (door = 0; door < 8; door++)
				{
					updateTrainDoorStatus(car, door, cctvUpdate.oa2.trainDoorData[car][door]);
				}
			}

			for (car = 0; car < 2; car++)
			{
				updateDriverConsoleStatus(car, cctvUpdate.oa2.driverConsoleCoverStatus[car]);
			}

			updateOa2Validity(true);
		}
		else
		{
            updateOa2Validity(false);
        }

        FUNCTION_EXIT;
    }


    void AtsCctvData::clearImportFlag()
    {
        FUNCTION_ENTRY( "clearImportFlag" );

        m_recordImported = false;

        FUNCTION_EXIT;
    }


    void AtsCctvData::setTrainValidity()
    {
        FUNCTION_ENTRY( "setTrainValidity" );

        if (!m_trainValid)
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                "setTrainValidity() - train %d validity, was invalid now valid",
                m_cctvData.physicalTrainNumber);

            m_trainValid = true;
        }

        m_recordImported = true;

        FUNCTION_EXIT;
    }


    void AtsCctvData::checkIfTrainNotImported()
    {
        FUNCTION_ENTRY( "checkIfTrainNotImported" );

        if (!m_recordImported && m_trainValid)
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                "checkIfTrainNotImportd() - not updated, train %d validity, was valid now invalid",
                m_cctvData.physicalTrainNumber);

            m_trainValid = false;

            // Invalidate localisation and OA1 as well
            updateLocalisationValidity(false);
            updateOa1Validity(false);
            updateOa2Validity(false);

            // the train record has gone, clear any old data
            initialiseOa1DataToUnknown();
            initialiseOa2DataToUnknown();
        }

        FUNCTION_EXIT;
    }


    void AtsCctvData::updateLocalisationValidity(bool valid)
    {
        FUNCTION_ENTRY( "updateLocalisationValidity" );

        if (m_cctvData.localisation.valid != valid)
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                "updateLocalisationValidity() - train %d validity, was %s now %s",
                m_cctvData.physicalTrainNumber, (m_cctvData.localisation.valid?"valid":"invalid"), (valid?"valid":"invalid"));

            m_cctvData.localisation.valid = valid;
            m_cctvDataChanged = true;
        }

        FUNCTION_EXIT;
    }


    void AtsCctvData::updateZoneId(unsigned long fixedBlocksBranchId,
                                     unsigned long fbAbscissaInTheBranch,
                                   unsigned long headCarAbscissaOnFb)
    {
        FUNCTION_ENTRY( "updateZoneId" );

		unsigned long stationZoneId = 0;

		try
		{
			stationZoneId = (unsigned long)m_stationZones.getZoneId(fixedBlocksBranchId, fbAbscissaInTheBranch, headCarAbscissaOnFb);
		}
		catch (TA_Base_Core::DatabaseException& e)
		{
			if (!m_cctvData.localisation.valid || m_cctvData.localisation.stationId != 0)
			{
				std::string text = "Station Zone ID DatabaseException - " + std::string(e.what());
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
							"updateZoneId() - train %d - %s", 
							m_cctvData.physicalTrainNumber, text.c_str());
			}
			stationZoneId = 0;
		}
		catch (TA_Base_Core::DataException& e)
		{
			if (!m_cctvData.localisation.valid || m_cctvData.localisation.stationId != 0)
			{
				std::string text = "Station Zone ID DataException - " + std::string(e.what());
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
							"updateZoneId() - train %d - %s", 
							m_cctvData.physicalTrainNumber, text.c_str());
			}
			stationZoneId = 0;
		}

        updateZoneId(stationZoneId);

        FUNCTION_EXIT;
    }


	void AtsCctvData::updateZoneId(unsigned long stationZoneId)
    {
        FUNCTION_ENTRY( "updateZoneId" );

        if (m_cctvData.localisation.stationId != stationZoneId)
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                "updateLocalisation() - train %d was at location %d now at location %d",
                m_cctvData.physicalTrainNumber, m_cctvData.localisation.stationId,
                stationZoneId);

            m_cctvData.localisation.lastStationId = m_cctvData.localisation.stationId;
            m_cctvData.localisation.stationId = stationZoneId;
            m_cctvDataChanged = true;
        }

        FUNCTION_EXIT;
    }


    bool AtsCctvData::isAtStation(unsigned long station)
    {
        FUNCTION_ENTRY( "isAtStation" );
        FUNCTION_EXIT;
		return ((m_cctvData.localisation.valid && (m_cctvData.localisation.stationId == station)) ||
				!m_cctvData.localisation.valid);
	}


    void AtsCctvData::updateOa1Validity(bool valid)
    {
        FUNCTION_ENTRY( "updateOa1Validity" );

        if (valid != m_cctvData.oa1.valid)
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                "updateOa1Validity() - train %d validity, was %s now %s",
                m_cctvData.physicalTrainNumber, (m_cctvData.oa1.valid?"valid":"invalid"), (valid?"valid":"invalid"));

            m_cctvData.oa1.valid = valid;
            m_cctvDataChanged = true;
        }

        FUNCTION_EXIT;
    }


    void AtsCctvData::updateFireExtinguisherStatus(unsigned int car,
                                                   TA_IRS_Bus::IAtsCctvCorbaDef::EFireExtinguisherStatus fireExtinguisherStatus)
    {
        FUNCTION_ENTRY( "updateFireExtinguisherStatus" );

        if (fireExtinguisherStatus != m_cctvData.oa1.fireExtinguisherStatus[car])
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                "updateFireExtinguisherStatus() - train %d car %d status, was %d now %d",
                m_cctvData.physicalTrainNumber, car+1,
                m_cctvData.oa1.fireExtinguisherStatus[car],
                fireExtinguisherStatus);

            m_cctvData.oa1.fireExtinguisherStatus[car] = fireExtinguisherStatus;
            m_cctvDataChanged = true;
        }

        FUNCTION_EXIT;
    }


    void AtsCctvData::updateEhsStatus(unsigned int car, unsigned int door,
									  TA_IRS_Bus::IAtsCctvCorbaDef::EEhsStatus ehsStatus)
    {
        FUNCTION_ENTRY( "updateEhsStatus" );

        if (ehsStatus != m_cctvData.oa1.ehsStatus[car][door])
		{
			std::ostringstream doorStr;
			if (door < 4)
			{
				doorStr << 'A' << (door+1);
			}
			else
			{
				doorStr << 'B' << (door-3);
			}

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                "updateEhsStatus() - train %d car %d door %s status, was %d now %d", 
				m_cctvData.physicalTrainNumber, car+1, doorStr.str().c_str(), 
				m_cctvData.oa1.ehsStatus[car][door], ehsStatus);

			m_cctvData.oa1.ehsStatus[car][door] = ehsStatus;
			m_cctvDataChanged = true;
        }

        FUNCTION_EXIT;
    }


    void AtsCctvData::updateDetrainmentDoorStatus(unsigned int car,
												  TA_IRS_Bus::IAtsCctvCorbaDef::EDdStatus detrainmentDoorStatus)
	{
        FUNCTION_ENTRY( "updateDetrainmentDoorStatus" );

		if (detrainmentDoorStatus != m_cctvData.oa1.detrainmentDoorStatus[car])
		{
			int displayCar = (car==0)?1:3;

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                "updateDetrainmentDoorStatus() - train %d car %d status, was %d now %d", 
				m_cctvData.physicalTrainNumber, displayCar, m_cctvData.oa1.detrainmentDoorStatus[car], detrainmentDoorStatus);

			m_cctvData.oa1.detrainmentDoorStatus[car] = detrainmentDoorStatus;
			m_cctvDataChanged = true;
		}

        FUNCTION_EXIT;
    }


	void AtsCctvData::updateUnderSeatFireStatus(unsigned int car, 
												TA_IRS_Bus::IAtsCctvCorbaDef::EFireSmokeDetectionStatus underSeatFireStatus)
    {
        FUNCTION_ENTRY( "updateUnderSeatFireStatus" );

        if (underSeatFireStatus != m_cctvData.oa1.underSeatFire[car])
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                "updateUnderSeatFireStatus() - train %d car %d status, was %d now %d",
                m_cctvData.physicalTrainNumber, car+1, m_cctvData.oa1.underSeatFire[car], underSeatFireStatus);

            m_cctvData.oa1.underSeatFire[car] = underSeatFireStatus;
            m_cctvDataChanged = true;
        }

        FUNCTION_EXIT;
    }


    void AtsCctvData::updateUnderFrameFireStatus(unsigned int car,
												 TA_IRS_Bus::IAtsCctvCorbaDef::EFireSmokeDetectionStatus underFrameFireStatus)
    {
        FUNCTION_ENTRY( "updateUnderFrameFireStatus" );

        if (underFrameFireStatus != m_cctvData.oa1.underFrameFireStatus[car])
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                "updateUnderFrameFireStatus() - train %d car %d status, was %d now %d",
                m_cctvData.physicalTrainNumber, car+1, m_cctvData.oa1.underFrameFireStatus[car], underFrameFireStatus);

            m_cctvData.oa1.underFrameFireStatus[car] = underFrameFireStatus;
            m_cctvDataChanged = true;
        }

        FUNCTION_EXIT;
    }


	void AtsCctvData::updateExteriorSmokeDetectionStatus(unsigned int car, 
														 TA_IRS_Bus::IAtsCctvCorbaDef::EFireSmokeDetectionStatus exteriorSmokeDetectionStatus)
    {
        FUNCTION_ENTRY( "updateExteriorSmokeDetectionStatus" );

        if (exteriorSmokeDetectionStatus != m_cctvData.oa1.exteriorSmoke[car])
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                "updateExteriorSmokeDetectionStatus() - train %d car %d status, was %d now %d",
                m_cctvData.physicalTrainNumber, car+1, m_cctvData.oa1.exteriorSmoke[car], exteriorSmokeDetectionStatus);

            m_cctvData.oa1.exteriorSmoke[car] = exteriorSmokeDetectionStatus;
            m_cctvDataChanged = true;
        }

        FUNCTION_EXIT;
    }


	void AtsCctvData::updateInteriorSmokeDetectionStatus(unsigned int car, 
														 TA_IRS_Bus::IAtsCctvCorbaDef::EInteriorSmokeDetectionStatus interiorSmokeDetectionStatus)
	{
        FUNCTION_ENTRY( "updateInteriorSmokeDetectionStatus" );

		if (interiorSmokeDetectionStatus != m_cctvData.oa1.interiorSmoke[car])
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                "updateInteriorSmokeDetectionStatus() - train %d car %d status, was %d now %d", 
				m_cctvData.physicalTrainNumber, car+1, m_cctvData.oa1.interiorSmoke[car], interiorSmokeDetectionStatus);

			m_cctvData.oa1.interiorSmoke[car] = interiorSmokeDetectionStatus;
			m_cctvDataChanged = true;
        }

        FUNCTION_EXIT;
    }


    void AtsCctvData::updateOa2Validity(bool valid)
    {
        FUNCTION_ENTRY( "updateOa2Validity" );

		if (valid != m_cctvData.oa2.valid)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                "updateOa2Validity() - train %d validity, was %s now %s", 
				m_cctvData.physicalTrainNumber, (m_cctvData.oa2.valid?"valid":"invalid"), (valid?"valid":"invalid"));

			m_cctvData.oa2.valid = valid;
			m_cctvDataChanged = true;
		}

        FUNCTION_EXIT;
    }


	void AtsCctvData::updateDriverConsoleStatus(unsigned int car, 
												TA_IRS_Bus::IAtsCctvCorbaDef::EDriverConsoleCoverStatus driverConsoleCoverStatus)
    {
        FUNCTION_ENTRY( "updateDriverConsoleStatus" );

		if (driverConsoleCoverStatus != m_cctvData.oa2.driverConsoleCoverStatus[car])
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                "updateDriverConsoleStatus() - train %d car %d status, was %d now %d", 
				m_cctvData.physicalTrainNumber, car+1, m_cctvData.oa2.driverConsoleCoverStatus[car], driverConsoleCoverStatus);

			m_cctvData.oa2.driverConsoleCoverStatus[car] = driverConsoleCoverStatus;
			m_cctvDataChanged = true;
        }

        FUNCTION_EXIT;
    }


    void AtsCctvData::updateTrainDoorStatus(unsigned int car, unsigned int door,
                                            TA_IRS_Bus::IAtsCctvCorbaDef::ETrainDoorStatus trainDoorStatus)
    {
        FUNCTION_ENTRY( "updateTrainDoorStatus" );

        if (trainDoorStatus != m_cctvData.oa2.trainDoorData[car][door])
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                "updateTrainDoorStatus() - train %d car %d door %d status, was %d now %d",
                m_cctvData.physicalTrainNumber, car+1, door, m_cctvData.oa2.trainDoorData[car][door], trainDoorStatus);

            m_cctvData.oa2.trainDoorData[car][door] = trainDoorStatus;
            m_cctvDataChanged = true;
        }

        FUNCTION_EXIT;
    }


    void AtsCctvData::updateServiceNumber(TA_IRS_Bus::ServiceNumberType serviceNum)
    {
        FUNCTION_ENTRY( "updateServiceNumber" );

        if (m_cctvData.serviceNumber != serviceNum)
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                "updateServiceNumber(): update service number to %d for trainId %d", serviceNum    , m_cctvData.physicalTrainNumber);

            m_cctvData.serviceNumber = serviceNum;
            m_cctvDataChanged = true;
        }

        FUNCTION_EXIT;
    }


    TA_IRS_Bus::ServiceNumberType AtsCctvData::getServiceNumber()
    {
        FUNCTION_ENTRY( "getServiceNumber" );
        FUNCTION_EXIT;
        return m_cctvData.serviceNumber;
    }


    bool AtsCctvData::isUpdated()
    {
        FUNCTION_ENTRY( "isUpdated" );
        FUNCTION_EXIT;
        return m_cctvDataChanged;
    }


    void AtsCctvData::sendUpdates()
    {
        FUNCTION_ENTRY( "sendUpdates" );

		if (m_cctvDataChanged)
		{
			//
			// Update Camera Entities
			//

			std::vector<TrainCctvCamera*>::iterator it;
			for (it = m_cameras.begin(); it != m_cameras.end(); it++)
			{	
				(*it)->updateCamera();
			}
		}

        m_cctvDataChanged = false;

        FUNCTION_EXIT;
    }


    void AtsCctvData::attachCamera(TrainCctvCamera* camera, bool isAlarmAgentAvailable)
    {
        FUNCTION_ENTRY( "attachCamera" );

        m_cameras.push_back(camera);

        camera->clearCameraAlarmState(isAlarmAgentAvailable);
        camera->addExpressionData(this);
        camera->updateCamera(isAlarmAgentAvailable);

        FUNCTION_EXIT;
    }


    bool AtsCctvData::getBooleanValue ( const std::string & variableName ) const
    {
        FUNCTION_ENTRY( "getBooleanValue" );

		std::vector<std::string> variableParts = AddressUtil::splitColonSeparatedString(variableName);

		if (variableParts.size() < 2)
		{
			std::ostringstream message;
			message << "Train " << m_cctvData.physicalTrainNumber << " attribute ";
			message << variableName << " - Insufficient parameters";
			TA_THROW(TA_Base_Core::MathematicalEvaluationException(message.str()));
		}

		// OA1 Data
		if (AddressUtil::caseInsensitiveCompare(variableParts[0], OA1_UNDER_FRAME_FIRE) ||
			AddressUtil::caseInsensitiveCompare(variableParts[0], OA1_PECU_ACTIVATED) ||
			AddressUtil::caseInsensitiveCompare(variableParts[0], OA1_EXTINGUISHER_REMOVED) ||
			AddressUtil::caseInsensitiveCompare(variableParts[0], OA1_EXT_SMOKE_DETECTED) ||
			AddressUtil::caseInsensitiveCompare(variableParts[0], OA1_INT_SMOKE_DETECTED) ||
			AddressUtil::caseInsensitiveCompare(variableParts[0], OA1_UNDER_SEAT_FIRE) ||
			AddressUtil::caseInsensitiveCompare(variableParts[0], OA1_EHS_ACTIVATED) ||
			AddressUtil::caseInsensitiveCompare(variableParts[0], OA1_DD_COVER_OPEN) ||
			AddressUtil::caseInsensitiveCompare(variableParts[0], OA1_DD_NOT_LOCKED_OR_NOT_CLOSED))
		{
			if (!m_cctvData.oa1.valid)
			{
				std::ostringstream message;
				message << "Train " << m_cctvData.physicalTrainNumber << " attribute ";
				message << variableName << " - OA1 Data not valid";
				TA_THROW(TA_Base_Core::MathematicalEvaluationException(message.str()));
			}
            else
            {
                FUNCTION_EXIT;
                return getOa1BooleanValue(variableParts);
            }
		}

		// OA2 Data
		if (AddressUtil::caseInsensitiveCompare(variableParts[0], OA2_DRIVER_CONSOLE_COVER_OPEN) ||
			AddressUtil::caseInsensitiveCompare(variableParts[0], OA2_DOOR_OBSTRUCTION) ||
		    AddressUtil::caseInsensitiveCompare(variableParts[0], OA2_DOOR_UNKNOWN) )
		{
		    // if OA1 is valid, but OA2 is not,
		    // still use the last known OA2 value to allow the expression to be evaluated.
			if (!m_cctvData.oa2.valid && !m_cctvData.oa1.valid)
			{
				std::ostringstream message;
				message << "Train " << m_cctvData.physicalTrainNumber << " attribute ";
				message << variableName << " - OA2 Data not valid";
				TA_THROW(TA_Base_Core::MathematicalEvaluationException(message.str()));
			}
			else
            {
                FUNCTION_EXIT;
                return getOa2BooleanValue(variableParts);
            }
        }

		std::ostringstream message;
		message << "Train " << m_cctvData.physicalTrainNumber << " attribute ";
		message << variableName << " - Not defined";
		TA_THROW(TA_Base_Core::MathematicalEvaluationException(message.str()));
	}


    bool AtsCctvData::getOa1BooleanValue(const std::vector<std::string>& variableParts) const
    {
        FUNCTION_ENTRY( "getOa1BooleanValue" );

        std::istringstream params (variableParts[1]);
		unsigned int car = 0;
		params >> car;

		// Under frame fire detected
		if (AddressUtil::caseInsensitiveCompare(variableParts[0], OA1_UNDER_FRAME_FIRE))
		{
			// Parts are attribute, car
			if (variableParts.size() != 2 || car < 1 || car > 4)
			{
				std::ostringstream message;
				message << "Train " << m_cctvData.physicalTrainNumber << " attribute ";
				message << variableParts[0] << " - Bad Parameters";
				TA_THROW(TA_Base_Core::MathematicalEvaluationException(message.str()));
            }

            FUNCTION_EXIT;
            return (m_cctvData.oa1.underFrameFireStatus[car-1] == TA_IRS_Bus::IAtsCctvCorbaDef::FireSmokeDetected
					|| m_cctvData.oa1.underFrameFireStatus[car-1] == TA_IRS_Bus::IAtsCctvCorbaDef::FireSmokeDetectionUnknown);
		}

		// Fire extinguisher removed
		if (AddressUtil::caseInsensitiveCompare(variableParts[0], OA1_EXTINGUISHER_REMOVED))
		{
			// Parts are attribute, car
			if (variableParts.size() != 2 || car < 1 || car > 4)
			{
				std::ostringstream message;
				message << "Train " << m_cctvData.physicalTrainNumber << " attribute ";
				message << variableParts[0] << " - Bad Parameters";
				TA_THROW(TA_Base_Core::MathematicalEvaluationException(message.str()));
            }

            FUNCTION_EXIT;
            return (m_cctvData.oa1.fireExtinguisherStatus[car - 1] == TA_IRS_Bus::IAtsCctvCorbaDef::ExtinguisherStatusAtLeastOneRemoved);
		}

		// Exterior Smoke Detected
		if (AddressUtil::caseInsensitiveCompare(variableParts[0], OA1_EXT_SMOKE_DETECTED))
		{
			// Parts are attribute, car
			if (variableParts.size() != 2 || car < 1 || car > 4)
			{
				std::ostringstream message;
				message << "Train " << m_cctvData.physicalTrainNumber << " attribute ";
				message << variableParts[0] << " - Bad Parameters";
				TA_THROW(TA_Base_Core::MathematicalEvaluationException(message.str()));
            }

            FUNCTION_EXIT;
            return (m_cctvData.oa1.exteriorSmoke[car-1] == TA_IRS_Bus::IAtsCctvCorbaDef::FireSmokeDetected
					|| m_cctvData.oa1.exteriorSmoke[car-1] == TA_IRS_Bus::IAtsCctvCorbaDef::FireSmokeDetectionUnknown);
		}
		
		// Interior Smoke Detected
		if (AddressUtil::caseInsensitiveCompare(variableParts[0], OA1_INT_SMOKE_DETECTED))
		{
			// Parts are attribute, car
			if (variableParts.size() != 2 || car < 1 || car > 4)
			{
				std::ostringstream message;
				message << "Train " << m_cctvData.physicalTrainNumber << " attribute ";
				message << variableParts[0] << " - Bad Parameters";
				TA_THROW(TA_Base_Core::MathematicalEvaluationException(message.str()));
            }

            FUNCTION_EXIT;
            return (m_cctvData.oa1.interiorSmoke[car-1] == TA_IRS_Bus::IAtsCctvCorbaDef::InteriorSmokeUnknown
					|| m_cctvData.oa1.interiorSmoke[car-1] == TA_IRS_Bus::IAtsCctvCorbaDef::InteriorSmokeAlarm);
		}

		// Under Seat Fire
		if (AddressUtil::caseInsensitiveCompare(variableParts[0], OA1_UNDER_SEAT_FIRE))
		{
			// Parts are attribute, car
			if (variableParts.size() != 2 || car < 1 || car > 4)
			{
				std::ostringstream message;
				message << "Train " << m_cctvData.physicalTrainNumber << " attribute ";
				message << variableParts[0] << " - Bad Parameters";
				TA_THROW(TA_Base_Core::MathematicalEvaluationException(message.str()));
            }

            FUNCTION_EXIT;
            return (m_cctvData.oa1.underSeatFire[car-1] == TA_IRS_Bus::IAtsCctvCorbaDef::FireSmokeDetected
					|| m_cctvData.oa1.underSeatFire[car-1] == TA_IRS_Bus::IAtsCctvCorbaDef::FireSmokeDetectionUnknown);
		}

		// Saloon door EHS open
		if (AddressUtil::caseInsensitiveCompare(variableParts[0], OA1_EHS_ACTIVATED))
		{
			// Parts are attribute, car, door
			if (variableParts.size() != 3 || car < 1 || car > 4)
			{
				std::ostringstream message;
				message << "Train " << m_cctvData.physicalTrainNumber << " attribute ";
				message << variableParts[0] << " - Bad Parameters";
				TA_THROW(TA_Base_Core::MathematicalEvaluationException(message.str()));
			}

			const std::string& door = variableParts[2];

			if (door.length() != 2 || (door[0] != 'A' && door[0] != 'B'))
			{
				std::ostringstream message;
				message << "Train " << m_cctvData.physicalTrainNumber << " attribute ";
				message << variableParts[0] << " - Bad Parameters";
				TA_THROW(TA_Base_Core::MathematicalEvaluationException(message.str()));
			}

			int doorIndex = (int)(door[1] - '0') - 1;

			if (doorIndex < 0 || doorIndex > 3)
			{
				std::ostringstream message;
				message << "Train " << m_cctvData.physicalTrainNumber << " attribute ";
				message << variableParts[0] << " - Bad Parameters";
				TA_THROW(TA_Base_Core::MathematicalEvaluationException(message.str()));
			}

			if (door[0] == 'B')
			{
				doorIndex += 4;
            }

            FUNCTION_EXIT;
            return (m_cctvData.oa1.ehsStatus[car - 1][doorIndex] == TA_IRS_Bus::IAtsCctvCorbaDef::EhsStatusActivated);
		}

		// Detrainment door Cover Open
		if (AddressUtil::caseInsensitiveCompare(variableParts[0], OA1_DD_COVER_OPEN))
		{
			// Parts are attribute, car
			if (variableParts.size() != 2 || (car != 1 && car != 3))
			{
				std::ostringstream message;
				message << "Train " << m_cctvData.physicalTrainNumber << " attribute ";
				message << variableParts[0] << " - Bad Parameters";
				TA_THROW(TA_Base_Core::MathematicalEvaluationException(message.str()));
            }

            FUNCTION_EXIT;
			return ( (m_cctvData.oa1.detrainmentDoorStatus[(car == 1)?0:1] == TA_IRS_Bus::IAtsCctvCorbaDef::DdCoverOpen) ||
			         (m_cctvData.oa1.detrainmentDoorStatus[(car == 1)?0:1] == TA_IRS_Bus::IAtsCctvCorbaDef::DdEvacuationRequired) );
		}

		// Detrainment door Not closed nor locked
		if (AddressUtil::caseInsensitiveCompare(variableParts[0], OA1_DD_NOT_LOCKED_OR_NOT_CLOSED))
		{
			// Parts are attribute, car
			if (variableParts.size() != 2 || (car != 1 && car != 3))
			{
				std::ostringstream message;
				message << "Train " << m_cctvData.physicalTrainNumber << " attribute ";
				message << variableParts[0] << " - Bad Parameters";
				TA_THROW(TA_Base_Core::MathematicalEvaluationException(message.str()));
			}
			
            // ss++ TD16046
            TA_IRS_Bus::IAtsCctvCorbaDef::EDdStatus ddStatus = m_cctvData.oa1.detrainmentDoorStatus[(car == 1)?0:1];

            FUNCTION_EXIT;
			return (ddStatus == TA_IRS_Bus::IAtsCctvCorbaDef::DdNotLockedOrNotClosed);
            // ++ss
		}

		std::ostringstream message;
		message << "Train " << m_cctvData.physicalTrainNumber << " attribute ";
		message << variableParts[0] << " - Not defined";
		TA_THROW(TA_Base_Core::MathematicalEvaluationException(message.str()));
	}


    bool AtsCctvData::getOa2BooleanValue(const std::vector<std::string>& variableParts) const
    {
        FUNCTION_ENTRY( "getOa2BooleanValue" );

        std::istringstream params (variableParts[1]);
		unsigned int car = 0;
		params >> car;

		// Console cover open
		if (AddressUtil::caseInsensitiveCompare(variableParts[0], OA2_DRIVER_CONSOLE_COVER_OPEN))
		{
			// Parts are attribute, car
			if (variableParts.size() != 2 || (car != 1 && car != 3))
			{
				std::ostringstream message;
				message << "Train " << m_cctvData.physicalTrainNumber << " attribute ";
				message << variableParts[0] << " - Bad Parameters";
				TA_THROW(TA_Base_Core::MathematicalEvaluationException(message.str()));
			}

            FUNCTION_EXIT;
			return (m_cctvData.oa2.driverConsoleCoverStatus[(car==1)?0:1] == TA_IRS_Bus::IAtsCctvCorbaDef::ConsoleCoverStatusOpen);
		}

		// Train door obstruction
		if (AddressUtil::caseInsensitiveCompare(variableParts[0], OA2_DOOR_OBSTRUCTION))
		{
			// Parts are attribute, car, door
			if (variableParts.size() != 3 || car < 1 || car > 4)
			{
				std::ostringstream message;
				message << "Train " << m_cctvData.physicalTrainNumber << " attribute ";
				message << variableParts[0] << " - Bad Parameters";
				TA_THROW(TA_Base_Core::MathematicalEvaluationException(message.str()));
			}

			const std::string& door = variableParts[2];

			if (door.length() != 2 || (door[0] != 'A' && door[0] != 'B'))
			{
				std::ostringstream message;
				message << "Train " << m_cctvData.physicalTrainNumber << " attribute ";
				message << variableParts[0] << " - Bad Parameters";
				TA_THROW(TA_Base_Core::MathematicalEvaluationException(message.str()));
			}

			int doorIndex = (int)(door[1] - '0') - 1;

			if (doorIndex < 0 || doorIndex > 3)
			{
				std::ostringstream message;
				message << "Train " << m_cctvData.physicalTrainNumber << " attribute ";
				message << variableParts[0] << " - Bad Parameters";
				TA_THROW(TA_Base_Core::MathematicalEvaluationException(message.str()));
			}

			if (door[0] == 'B')
			{
				doorIndex += 4;
            }

            FUNCTION_EXIT;
			return (m_cctvData.oa2.trainDoorData[car-1][doorIndex] == TA_IRS_Bus::IAtsCctvCorbaDef::TrainDoorStatusObstacleDetection);
		}

		// Train door unknown 
		if (AddressUtil::caseInsensitiveCompare(variableParts[0], OA2_DOOR_UNKNOWN))
		{
			// Parts are attribute, car, door
			if (variableParts.size() != 3 || car < 1 || car > 4)
			{
				std::ostringstream message;
				message << "Train " << m_cctvData.physicalTrainNumber << " attribute ";
				message << variableParts[0] << " - Bad Parameters";
				TA_THROW(TA_Base_Core::MathematicalEvaluationException(message.str()));
			}

			const std::string& door = variableParts[2];

			if (door.length() != 2 || (door[0] != 'A' && door[0] != 'B'))
			{
				std::ostringstream message;
				message << "Train " << m_cctvData.physicalTrainNumber << " attribute ";
				message << variableParts[0] << " - Bad Parameters";
				TA_THROW(TA_Base_Core::MathematicalEvaluationException(message.str()));
			}

			int doorIndex = (int)(door[1] - '0') - 1;

			if (doorIndex < 0 || doorIndex > 3)
			{
				std::ostringstream message;
				message << "Train " << m_cctvData.physicalTrainNumber << " attribute ";
				message << variableParts[0] << " - Bad Parameters";
				TA_THROW(TA_Base_Core::MathematicalEvaluationException(message.str()));
			}

			if (door[0] == 'B')
			{
				doorIndex += 4;
            }

            FUNCTION_EXIT;
			return (m_cctvData.oa2.trainDoorData[car-1][doorIndex] == TA_IRS_Bus::IAtsCctvCorbaDef::TrainDoorStatusUnknown);
		}

		std::ostringstream message;
		message << "Train " << m_cctvData.physicalTrainNumber << " attribute ";
		message << variableParts[0] << " - Not defined";
		TA_THROW(TA_Base_Core::MathematicalEvaluationException(message.str()));
	}


    bool AtsCctvData::isOaValid()
    {
        FUNCTION_ENTRY( "isOaValid" );
        FUNCTION_EXIT;
        return (m_cctvData.oa1.valid || m_cctvData.oa2.valid);
    }


    double AtsCctvData::getRealNumberValue ( const std::string & variableName ) const
    {
        FUNCTION_ENTRY( "getRealNumberValue" );

		std::ostringstream message;
		message << "Train " << m_cctvData.physicalTrainNumber << " attribute ";
		message << variableName << " - No real number values defined";
		TA_THROW(TA_Base_Core::MathematicalEvaluationException(message.str()));
	}


}
