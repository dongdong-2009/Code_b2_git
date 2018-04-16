/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/4669_Review_Branch/4669/transactive/bus/ats/ats_agent/CommonATS/src/AtsCctvData.h $
  * @author:   Darren Sampson
  * @version:  $Revision: #4 $
  *
  * Last modification: $DateTime: 2011/04/13 14:25:16 $
  * Last modified by:  $Author: bihui.luo $
  *
  * This represents the ATS Train Table
  */
#ifndef ATSCCTVDATA_H
#define ATSCCTVDATA_H

#include "bus/ats/ats_agent/IDL/src/IAtsCorbaDef.h"

#include "core/data_access_interface/ats/src/IAtsFixedBlockMap.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/data_access_interface/entity_access/src/AtsAgentEntityData.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

#include <vector>


namespace TA_IRS_Bus
{
	class TrainCctvCamera;

    class AtsCctvData : public TA_Base_Core::ReEntrantThreadLockable
    {

    public:

		/**
		  * Constructor
		  *
		  */
		AtsCctvData(TA_IRS_Bus::PhysicalTrainNumberType physicalTrainNumber, unsigned long location,  
					TA_IRS_Core::IAtsFixedBlockMap& stationZones, TA_Base_Core::AtsAgentEntityData::EAtsLocationType locationType);
		virtual ~AtsCctvData();

		TA_IRS_Bus::IAtsCctvCorbaDef::AtsCctvMessageCorbaDef buildAtsCctvMessage();
		void importAtsCctvMessage(const TA_IRS_Bus::IAtsCctvCorbaDef::AtsCctvMessageCorbaDef& cctvUpdate);

		// Train Validity
		void clearImportFlag();
		void setTrainValidity();
		void checkIfTrainNotImported();

		// Localisation
		void updateLocalisationValidity(bool valid);
		void updateZoneId(unsigned long fixedBlocksBranchId,
						  unsigned long fbAbscissaInTheBranch,
						  unsigned long headCarAbscissaOnFb);
		bool isAtStation(unsigned long station);
		bool isAtsDepot();
		// OA1 Data
		void updateOa1Validity(bool valid);

		void updateFireExtinguisherStatus(unsigned int car, 
										  TA_IRS_Bus::IAtsCctvCorbaDef::EFireExtinguisherStatus fireExtinguisherStatus);
		void updateEhsStatus(unsigned int car, unsigned int door, 
						     TA_IRS_Bus::IAtsCctvCorbaDef::EEhsStatus ehsStatus);
		void updateDetrainmentDoorStatus(unsigned int car, 
										 TA_IRS_Bus::IAtsCctvCorbaDef::EDdStatus detrainmentDoorStatus);
		void updateUnderSeatFireStatus(unsigned int car, 
									   TA_IRS_Bus::IAtsCctvCorbaDef::EFireSmokeDetectionStatus underSeatFireStatus);
		void updateUnderFrameFireStatus(unsigned int car, 
										TA_IRS_Bus::IAtsCctvCorbaDef::EFireSmokeDetectionStatus underFrameFireStatus);
		void updateExteriorSmokeDetectionStatus(unsigned int car, 
												TA_IRS_Bus::IAtsCctvCorbaDef::EFireSmokeDetectionStatus exteriorSmokeDetectionStatus);
		void updateInteriorSmokeDetectionStatus(unsigned int car, 
												TA_IRS_Bus::IAtsCctvCorbaDef::EInteriorSmokeDetectionStatus interiorSmokeDetectionStatus);
		
		// OA2 Data
		void updateOa2Validity(bool valid);
		void updateDriverConsoleStatus(unsigned int car, 
									   TA_IRS_Bus::IAtsCctvCorbaDef::EDriverConsoleCoverStatus driverConsoleCoverStatus);
		void updateTrainDoorStatus(unsigned int car, unsigned int door, 
								   TA_IRS_Bus::IAtsCctvCorbaDef::ETrainDoorStatus trainDoorStatus);
		bool isUpdated();
		void sendUpdates();

        void attachCamera(TrainCctvCamera* camera, bool isAlarmAgentAvailable=true);

		bool isOaValid();

		// For Mathematical Evaluation context
		bool getBooleanValue ( const std::string & variableName ) const;
		double getRealNumberValue ( const std::string & variableName ) const;

		void updateServiceNumber(TA_IRS_Bus::ServiceNumberType serviceNum);
		TA_IRS_Bus::ServiceNumberType getServiceNumber();

	protected:

		bool checkAttribute(std::string attribute);
		void updateCamera(TrainCctvCamera* camera);

		void updateZoneId(unsigned long stationZoneId);
		bool getOa1BooleanValue(const std::vector<std::string>& variableParts) const;
		bool getOa2BooleanValue(const std::vector<std::string>& variableParts) const;
		
		void initialiseOa1DataToUnknown();
		void initialiseOa2DataToUnknown();
		
    private:
        // Disable copy constructor and assignment operator
        AtsCctvData( const AtsCctvData& trainRecord);
        AtsCctvData& operator=(const AtsCctvData &);

	protected:
		
		bool								m_trainValid;
		bool								m_recordImported;

		TA_IRS_Bus::IAtsCctvCorbaDef::AtsCctvMessageCorbaDef
											m_cctvData;
		bool								m_cctvDataChanged;

		std::vector<TrainCctvCamera*>		m_cameras;

		unsigned long						m_location;

		TA_IRS_Core::IAtsFixedBlockMap&		m_stationZones;	

		TA_Base_Core::AtsAgentEntityData::EAtsLocationType m_locationType ;
		
		static const std::string OA1_UNDER_FRAME_FIRE;
		static const std::string OA1_PECU_ACTIVATED;
		static const std::string OA1_EXTINGUISHER_REMOVED;
		static const std::string OA1_EXT_SMOKE_DETECTED;
		static const std::string OA1_INT_SMOKE_DETECTED;
		static const std::string OA1_UNDER_SEAT_FIRE;
		static const std::string OA1_EHS_ACTIVATED;
		static const std::string OA1_DD_COVER_OPEN;
		static const std::string OA1_DD_NOT_LOCKED_OR_NOT_CLOSED;

		static const std::string OA2_DRIVER_CONSOLE_COVER_OPEN;
		static const std::string OA2_DOOR_OBSTRUCTION;
		static const std::string OA2_DOOR_UNKNOWN;
    };
}

#endif // !defined(ATSCCTVDATA_H)

