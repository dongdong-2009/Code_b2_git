/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/PRIV_3001/3001/transactive/app/ats/AtsAgent/src/AtsTrainData.h $
  * @author:   Darren Sampson
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last mofified by:  $Author: lim.cy $
  *
  * This represents the ATS Train Table
  */
#ifndef AtsTrainData_H
#define AtsTrainData_H

#include "core/data_access_interface/entity_access/src/AtsAgentEntityData.h"
#include "core/message/src/CommsMessageSender.h"
#include "core/data_access_interface/ats/src/IAtsFixedBlockMap.h"

#include "bus/ats/ats_agent/IDL/src/IAtsCorbaDef.h"

#include "app/ats/AtsAgent/src/RawTable.h"
#include "app/ats/AtsAgent/src/AtsTrainDataTimer.h"

#include "app/ats/AtsAgent/AtsTCPCommunication/src/AtsTcpServer.h"

namespace TA_IRS_App
{
	class AtsEcsData;

	const int OA1_TABLE_SIZE = 24;
	const int ATC_STATUS_REPORT_SIZE = 22;
	const int OA1_DATA_HEADER_LENGTH = 5; //Lucky 20-April-2011 7:52PM
	

	//Use for AtsMethod
	typedef std::vector<unsigned char> CriticalAlarmEvent;

    class AtsTrainData : public TA_Base_Core::ReEntrantThreadLockable,
						 public IAtsTrainDataTimerCallback
    {
    public:

		/**
		  * Constructor
		  *
		  */
		//td 13947
		//zhou yuan++
		//start the AtsTrainDataTimer for ATC table
		
		AtsTrainData(TA_IRS_Bus::PhysicalTrainNumberType physicalTrainNumber, unsigned long entityKey,
					 unsigned long locationKey, TA_IRS_Core::IAtsFixedBlockMap& stationZones,
					 TA_IRS_Core::IAtsFixedBlockMap& cctvZones,
					 std::vector<unsigned long>& stationsInThisSector,
					 AtsTrainDataTimer* atsTrainDataTimer);
		//++zhou yuan
		virtual ~AtsTrainData();

		TA_IRS_Bus::IAtsTrainCorbaDef::AtsTrainMessageCorbaDef buildAtsTrainMessage();
		void importAtsTrainMessage(const TA_IRS_Bus::IAtsTrainCorbaDef::AtsTrainMessageCorbaDef& trainMessage);

		void clearImportFlag();
		void setTrainValidity();
		void checkIfTrainNotImported();

		void updateLocalisationValidity(bool valid);
		void updateServiceNumber(TA_IRS_Bus::ServiceNumberType serviceNumber);
		void updateZoneId(unsigned long fixedBlocksBranchId,
						  unsigned long fbAbscissaInTheBranch,
						  unsigned long	headCarAbscissaOnFb);
		void updateMute(bool mute);

		void updateOa1Table(const TA_IRS_Bus::IAtsTrainCorbaDef::OctetSequence& oa1);
		void updateAtcTrainStatusReport(const TA_IRS_Bus::IAtsTrainCorbaDef::OctetSequence& atcTrainStatus);

		bool isUpdated();

		bool isValid();
		bool isValidForExport(unsigned long locationKey,
							  TA_Base_Core::AtsAgentEntityData::EAtsLocationType locationType);
		unsigned long getStation();

		bool isOa1Valid();
		unsigned char getOa1Data(unsigned int index);

		bool isAtcTrainStatusValid();
		unsigned char getAtcTrainStatusCodeHeader();
		unsigned char getAtcStatusReport(unsigned int index);

		void AtsTrainData::sendUpdates();
		
		//td 13947
		//zhou yuan++
		//start the AtsTrainDataTimer for ATC table		
		virtual void handleTimeout(ETimerType timerType); 
		//++zhou yuan

        void invalidateTrain();

		void updateCarNum1 ( unsigned long num ) ;
       void updateCarNum2 ( unsigned long num ) ;
	   void updateCarNum3  ( unsigned long num ) ;

	   //Use for the AtsMethod
		void updateAtsMethodTrainStatusReport(const TA_IRS_Bus::IAtsTrainCorbaDef::OctetSequence& atcTrainStatus);
		void updateAtsMethodOa1Table(const TA_IRS_Bus::IAtsTrainCorbaDef::OctetSequence& oa1);
		void startAtsServer(TA_IRS_App::AtsTcpServer* server);

	protected:

		void updateZoneId(const unsigned long radioZone,
						  const double cctvZone,
						  const TA_IRS_Bus::ETrack track);

		void updatePhysicalTrainService(const std::string physicalTrainService);

		void updateTrainValidity(bool valid);

		bool inThisSector(unsigned long stationId);

	
    private:
        // Disable default constructor, copy constructor and assignment operator
		AtsTrainData();
        AtsTrainData( const AtsTrainData& trainRecord);
        AtsTrainData& operator=(const AtsTrainData &);
		
		TA_IRS_App::AtsTcpServer* m_server;
	protected:

		// AtsTrainMessage
		TA_IRS_Bus::IAtsTrainCorbaDef::AtsTrainMessageCorbaDef
											m_atsTrainMessage;
		
		bool								m_recordImported;

		TA_IRS_Bus::ServiceNumberType			m_serviceNumber;			// <Service number>

		TA_IRS_Core::IAtsFixedBlockMap&		m_stationZones;
		TA_IRS_Core::IAtsFixedBlockMap&		m_cctvZones;

		bool								m_trainInfoChanged;

		TA_Base_Core::CommsMessageSender*		m_commsMessageSender;
		unsigned long						m_entityKey;
		unsigned long						m_locationKey;

		//
		// Validity of the OA1 train data:
		// -	Value to 0: data are relevant,
		// -	Value different from 0: data are not relevant
		//
		bool								m_oa1Valid;				// <Validity field>

		//
		// OA1 table from PV (Tetra)
		//
		unsigned char						m_oa1Table[OA1_TABLE_SIZE];	

		//
		// Atc Data Table
		//
		unsigned char						m_atcTrainStatusCodeHeader;
		unsigned char						m_atcStatusReport[ATC_STATUS_REPORT_SIZE];	// <ATC train borne message>

		bool								m_atcTrainStatusValid;

		std::vector<unsigned long>			m_stationsInThisSector;
		
		//td 13947
		//zhou yuan++
		//start the AtsTrainDataTimer for ATC table
		long m_oa1TimerId;
		long m_atcTimerId;
		AtsTrainDataTimer* m_atsTrainDataTimer;
		unsigned long m_carNum1 , m_carNum2, m_carNum3 ;
		//++zhou yuan
    };
}

#endif // !defined(TRAINRECORD_H)
