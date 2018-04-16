/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP/3001/transactive/app/cctv/tvs_agent/src/TVSS.h $
  * @author:   Lucky Cuizon
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2011/01/04 10:43:52 $
  * Last modified by:  $Author:  $
  *
  */
#ifndef TVSS_H
#define TVSS_H

#include "app/cctv/tvss_agent/TCPCommunication/src/BaseTcpServer.h"
#include "app/cctv/tvss_agent/TVSSProtocolLibrary/src/TVSSMethod.h"
#include "app/cctv/tvss_agent/TVSSProtocolLibrary/src/TVSSClearImageMethod.h"
#include "app/cctv/tvss_agent/TVSSProtocolLibrary/src/TVSSQuadScreenMethod.h"
#include "app/cctv/tvss_agent/TVSSProtocolLibrary/src/TVSSSingleScreenMethod.h"
#include "app/cctv/tvss_agent/TVSSProtocolLibrary/src/TVSSAcknowledgeMethod.h"
#include "app/cctv/tvss_agent/TVSSProtocolLibrary/src/TVSSSequencingScreenMethod.h"
#include "app/cctv/tvss_agent/TVSSProtocolLibrary/src/TVSSEquipmentQueryMethod.h"
#include "app/cctv/tvss_agent/TVSSProtocolLibrary/src/TVSSEvent.h"
#include "app/cctv/tvss_agent/TVSSProtocolLibrary/src/TVSSCommandResponseEventReader.h"
#include "app/cctv/tvss_agent/TVSSProtocolLibrary/src/TVSSTrainborneEventReader.h"
#include "app/cctv/tvss_agent/TVSSProtocolLibrary/src/TVSSEquipmentResponseEventReader.h"

#include "app/cctv/tvss_agent/src/TVSSServerLinkStatusUpdateThread.h"
#include "app/cctv/tvss_agent/src/TVSSBackgroundProcessor.h"

#define COUNT_EQUIPMENT_ALARM 4

namespace TA_IRS_App
{
	class BaseTcpServer;
	class TVSSBackgroundProcessor;

	//Method
	class TVSSMethod;
	class TVSSClearImageMethod;
	class TVSSQuadScreenMethod;
	class TVSSSingleScreenMethod;
	class TVSSSequencingScreenMethod;
	class TVSSEquipmentQueryMethod;

	//Event
	class TVSSEvent;	
	class TVSSCommandEventReader;
	class TVSSTrainborneEventReader;
	class TVSSEquipmentResponseEventReader;
	class TVSSServant;
	class TVSS
	{
	public:

		TVSS(TVSSServant* servant,TVSSServerLinkStatusUpdateThread* linkStatusThread);
		virtual ~TVSS();

		void setEntity(TA_Base_Core::IEntityData& entityData);
	   
	   /**
		* This function enables the radio hardware IP addresses to be defined
        * as well as other necessary parameters.
        *
        * @param primaryAddresses the list of primary host addresses, in format
        *        "12.34.56.78:4000,21,43.65.87:4000,third.host.ip.address:port"
        * @param secondaryAddresses the list of secondary host addresses, same
        *        format as the primaries. There cannot be more secondary
        *        addresses than primary, so the list of secondary addresses is
        *        restricted to the number of primary. Each of the secondary 
        *        addresses is paired with the same TcpServer as the corresponding
        *        ipaddress in the primaryAddresses list, ie. the first secondary
        *        address will be paired with the first primary, etc.
		*/
		void setParameters( const std::string & primaryAddress,
                            const std::string & secondaryAddress,
							const std::string & primaryTrainborneAddress,
							const std::string & secondaryTrainborneAddress,
							unsigned long timeout);


        void start();
		
		void setToControlMode(void);
		void setToMonitorMode(void);

		void onTerminate(bool peerAvailable);

		void sendMethod(TA_IRS_App::TVSSMethod &Method);
		
		void sendEquipmentAlarmMethod(TA_IRS_App::TVSSMethod &Method,bool alarmStatus[]);

        void sendTrainbornHeartbeatMethod();
		/**
		* This method returns the m_active member value
		*/
		bool getIsActive(void);
		

	   /*****************		
		* Event Handlers
		*****************/

	   /**
		* TVSSComandEventReader Event handler
		*/
		void onTVSSComandEventReader(const TA_IRS_App::TVSSCommandEventReader& eventReader);

	   /**
		* TVSSTrainborneEventReader Event handler
		*/
		void onTVSSTrainborneEventReader(const TA_IRS_App::TVSSTrainborneEventReader& eventReader);

	   /**
		* TVSSEquipmentResponseEventReader Event handler
		*/
		void onTVSSEquipmentResponseEventReader(const TA_IRS_App::TVSSEquipmentResponseEventReader& eventReader);

		
		/**
		 * These are to get and set the agent's asset name for
		 * alarm submission
		 */
		std::string getAgentAssetName() { return m_agentAssetName; };
		void setAgentAssetName (std::string assetName) { m_agentAssetName = assetName; };

	private:

		BaseTcpServer* server;
		BaseTcpServer* trainborneServer;

		bool									m_active; // Indicates if its in control/monitor mode

		// Radio agent's asset name for alarm submission
		std::string								m_agentAssetName;
		TVSSServerLinkStatusUpdateThread*		m_serverLinkStatusThread;

		///
        /// The processor that deals with events that need to be processed off
        /// the main Radio thread
        TVSSBackgroundProcessor *              m_backgroundProcessor;
		TVSSServant*						   m_servant;
	};


}
#endif //end TVSS_MANAGER_H