
#ifndef _ATSTCP_SERVER_WRAPPER_
#define _ATSTCP_SERVER_WRAPPER_

#include "app/ats/AtsAgent/src/AtsServerLinkStatusUpdateThread.h"
#include "app/ats/AtsAgent/AtsTCPCommunication/src/AtsTcpServer.h"
#include "app/ats/AtsAgent/AtsMethod/src/AtsITcpMethod.h"
#include "app/ats/AtsAgent/AtsMethod/src/AtsMethodOA1Table.h"
#include "app/ats/AtsAgent/AtsMethod/src/AtsMethodTrainStatusReport.h"
#include "app/ats/AtsAgent/AtsTCPCommunication/src/AtsBackgroundProcessor.h"
#include "core/data_access_interface/entity_access/src/EntityData.h"

namespace TA_IRS_App
{
	class AtsTcpServer;
	class AtsBackgroundProcessor;

	class AtsITcpMethod;
	class AtsMethodOA1Table;
	class AtsMethodTrainStatusReport;

	//event
	class AtsEventTrainStatusReport;
	class AtsEventOa1Table;

	class AtsTcpServerWrapper
	{
	public:
		AtsTcpServerWrapper(AtsServerLinkStatusUpdateThread* linkStatusThread);
		virtual ~AtsTcpServerWrapper();

		void setEntity(TA_Base_Core::EntityData& entityData);
	   
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
		void setParameters( const std::string & primaryAddresses,
                            const std::string & secondaryAddresses);


        void start();
		
		void setToControlMode(void);
		void setToMonitorMode(void);

		void onTerminate(bool peerAvailable);

		void sendMethod(TA_IRS_App::AtsITcpMethod &Method);
		
		/**
		* This method returns the m_active member value
		*/
		bool getIsActive(void);

		/**
		 * These are to get and set the agent's asset name for
		 * alarm submission
		 */
		std::string getAgentAssetName() { return m_agentAssetName; };
		void setAgentAssetName (std::string assetName) { m_agentAssetName = assetName; };

	private:

		AtsTcpServer* server;


		bool									m_active; // Indicates if its in control/monitor mode

		// Radio agent's asset name for alarm submission
		std::string								m_agentAssetName;
		AtsServerLinkStatusUpdateThread*		m_serverLinkStatusThread;

		AtsBackgroundProcessor* m_backgroundProcessor;
	};
};

#endif //end of _ATSTCP_SERVER_WRAPPER_