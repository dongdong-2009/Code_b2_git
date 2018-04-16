/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: \\depot\3001_TIP\3001\transactive\app\radio\radio_simulator\MotorolaProtocolLib\src\MotorolaSDSProtocolHelper.h $
  * @author:  Lucky Cuizon
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/10/08 04:05:25 $
  * Last modified by:  $Author: builder $
  *
  * Helper methods to encode/decode SDTS messages
  */

#ifndef _MOTOROLA_SDS_PROTOCOL_HELPER_H
#define _MOTOROLA_SDS_PROTOCOL_HELPER_H
#include "app/radio/radio_simulator/MotorolaProtocolLib/src/IProtocolHelper.h"
#include "app/radio/radio_simulator/MotorolaProtocolLib/src/CMTypes.h"
#include "app/radio/radio_simulator/MotorolaProtocolLib/src/RadioIdentity.h"
#include "app/radio/radio_simulator/TrainSimulatorLib/src/IRadioCommsObserver.h"
#include "app/radio/radio_simulator/MotorolaProtocolLib/src/CMData.h"
#include "app/radio/radio_simulator/CommandProcessorLib/src/ISDSMessageObserver.h"
#include "app/radio/radio_simulator/CommandProcessorLib/src/IPACMData.h"
#include <map>
#include <string>
#include "app/radio/radio_simulator/TrainSimulator/IModelLogger.h"

#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

#include "app/radio/radio_simulator/TrainSimulator/IRegStatus.h"


namespace TA_IRS_App
{
	class MotorolaSDSProtocolHelper : public IProtocolHelper, 
								      public IPACMData
									
	{
	public:

		/**
		 * Constructor
		 */
		MotorolaSDSProtocolHelper(IRegStatus* regStatus);

		/**
		 * Destructor
		 */
		virtual ~MotorolaSDSProtocolHelper();

		/**
		 * processIncomingSDS
		 *
		 * This method is used process the incoming SDS whether it is registration or a sds data
		 *
		 * @param : std::vector<unsigned char> pData
		 */
		void processIncomingSDS( std::vector<unsigned char>& pData );

		/**
		 * parseRegistrationRequest
		 *
		 * This method is used for Registration Request
		 *
		 * @param : const std::vector<unsigned char>& pData
		 * @param : RadioIdentity* id
		 */
		bool parseRegistrationRequest(const std::vector<unsigned char>& pData, RadioIdentity* id);
		CMRegistrationStatus processIdentity(RadioIdentity* id);
		void buildRegisterResponse(const std::vector<unsigned char>& pData, CMRegistrationStatus status);

		// For CM Broadcast 
		bool parseCMBroadcast(const std::vector<unsigned char>& pData);

		// For CM Data
		bool parseCMData(const std::vector<unsigned char>& pData);
		void buildTLReport();
		void buildCMReport();
		void buildISCSResponse(const std::vector<unsigned char>& pData);

		void registerSDSMessage( ISDSMessageObserver* sdsMessageObserver);

		void buildPAReadyResponse(const std::vector<unsigned char>& pData);

		void buildMotorolaHeaders(const std::string& pecEventMsg);
		
		void buildMotorolaHeadersTIMSCommand(const std::vector<unsigned char>& data,bool bIsUsePrimary);

		std::vector<unsigned char>* buildMotorolaHeadersHeartBeat(const std::vector<unsigned char>& heartBeatProtocol);

		void setLoggerView(IModelLogger* modelLogger);

		//Use for the PA Data
		CMData& getCMData();
		std::string& getTLHeader();

		//Use for the TTIS Data
		std::string& getTTISLibVersion();

		friend bool operator< (RadioIdentity a, RadioIdentity b);


		/**
		 * execAddISSI
		 *
		 * This method is used to add the ISSI
		 */
		bool execAddISSI(const std::string& cmdValue);

		/**
		 * execListISSIPassNum
		 *
		 * This method is used to list the ISSI Pass Number
		 */
		bool execListISSIPassNum();

		/**
		 * registerSocketSendData
		 */
		void registerSocketSendData( IRadioCommsObserver* radioComms);

		void logger(const std::vector<unsigned char>& data);

		bool checkDataControl(const std::vector<unsigned char>& data);


	

	private:
		/**
		 * Disable copy and assignment constructor
		 */
		MotorolaSDSProtocolHelper ( const MotorolaSDSProtocolHelper& );
		MotorolaSDSProtocolHelper& operator= ( const MotorolaSDSProtocolHelper& );

	
		typedef std::map<RadioIdentity, std::string> RadioMap;
		RadioMap radioIdMap;
		RadioMap::iterator radioId;

		IRadioCommsObserver* m_radioCommsObserver;
		ISDSMessageObserver* m_sdsMessageObserver;

		CMDeliveryStatus m_cmDeliveryStatus;
		CMData m_cmData;
		std::string m_tlHeader;
		std::string m_ttisLibVer;

		IModelLogger* m_logViewer;

		static unsigned char _commands[];
		TA_Base_Core::ReEntrantThreadLockable m_messageLock;

		IRegStatus* m_regStatus;

	};
}

#endif