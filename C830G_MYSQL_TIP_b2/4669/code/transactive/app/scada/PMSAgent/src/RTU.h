/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/4669_T01271350/4669/transactive/app/scada/PMSAgent/src/RTU.h $
  * @author:	Raymond Pau
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2008/11/28 16:26:01 $
  * Last modified by:	$Author: builder $
  *
  * Description:
  *
  *	The RTU object manages a set of DataPoints that have been configured for it.
  *
  */

#ifndef RTU_H
#define RTU_H


#include <string>
#include <vector>


#include "app/scada/PMSAgent/src/RTUDownloadWorker.h"

#include "bus/generic_agent/src/IEntity.h"
#include "bus/modbus_comms/src/DataBlock.h"

#include "core/data_access_interface/entity_access/src/StationSystemEntityData.h"
#include "core/data_access_interface/entity_access/src/RTUEntityData.h"
#include "core/message/src/NameValuePair.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

namespace TA_Base_Bus
{
	class DataPoint;
	class DataPointWriteRequest;
    class IModbus;
}

namespace TA_Base_Core
{
    class AuditMessageSender;
    class AlarmHelper;
    class MessageType;
}

namespace TA_IRS_App
{
	class RTUManager;
    class RTUCommandWorker;
	class RTUPollingWorker;
    class RTUStatusWorker;
    class StationSystemOutputWriter;
    typedef std::vector< StationSystemOutputWriter* > StationSystemOutputWriters;
    class RTUDpProcessor;

	enum ModbusConnectionType
    {
        CONNTYPE_POLL,
        CONNTYPE_COMMAND
    };


    class RTU : public TA_Base_Bus::IEntity
	{
	public:

		RTU ( RTUManager& rtuManager, TA_Base_Core::RTUEntityDataPtr rtuEntityData );

		virtual ~RTU();

	// operations
	public:

        virtual unsigned long adjustAddress( unsigned long registerAddress );
		virtual void updateCommsAlarm( bool raiseAlarm, const std::string & servicePortNumber, const std::string & additionalComment );
		virtual void submitAuditMessage( const TA_Base_Core::DescriptionParameters & description,
                                         const TA_Base_Core::MessageType & messageType );

        virtual TA_Base_Bus::IModbus * createModbusConnection( const std::string & rtuServicePortNumber, int slaveID, ModbusConnectionType connectionType );
        virtual void destroyModbusConnection( TA_Base_Bus::IModbus * & modbusConnection );

        void createRTUWorkers();

        //
		// operations required by IEntity
		//

		/**
		  * isValid
		  *
		  * This is a pure vitual method that must be implemented by 
		  * all enitity classes. It will return true if the entity is 
		  * valid. Otherwise it will return false.
		  *
		  * @return TRUE if the entity is valid, FALSE otherwise 
		  */
		virtual bool isValid();

		/**
		  * start
		  *
		  * This is a pure vitual method that must be implemented by 
		  * all enitity classes. It will start any object specific tasks,
		  * such as creating its own thread.
		  */
		virtual void start();

		/**
		  * stop
		  *
		  * This is a pure vitual method that must be implemented by all
		  * enitity classes. It will stop any object specific tasks, such 
		  * as stopping its own thread. 
		  */
		virtual void stop();

		/**
		  * update
		  *
		  * This method is called by the agent controlling the entity to ask 
		  * the entity to update its configuration. Called after an update 
		  * configuration event is received.
		  */
		virtual void update(const TA_Base_Core::ConfigUpdateDetails& updateEvent);

		/**
		  * remove
		  *
		  * When a configuration update event is received to delete an entity 
		  * from an agent, the agent can call this method to tell the entity 
		  * to commence any clean up opperations.
		  * 
		  * The agent then still has to remove the object from its vector of entities.
		  */
		virtual void remove();

		/**
		  * setToControlMode
		  *
		  * This method can be called by the agent responsible for the entity in
		  * the event of its mode of operation changing to control. This enables the
		  * entity to perform tasks such as connecting to any field devices it is
		  * responsible for communicating with.
		  */
		virtual void setToControlMode();

		/**
		  * setToMonitorMode
		  *
		  * This method can be called by the agent responsible for the entity in
		  * the event of its mode of operation changing to monitor. This enables the
		  * entity to perform tasks such as disconnecting from any field devices it was
		  * communicating with.
		  */
		virtual void setToMonitorMode();

		//
		// specific to RTU
		//

		/**
		  * getName
		  *
		  * Get the name of the RTU entity
		  *
		  */
		const std::string & getName() const;

		/**
		  * getAddress
		  *
		  * Get the address of the RTU entity
		  *
		  */
		const std::string & getAddress() const;

		/**
		  * getPKey
		  *
		  * Get the pkey of the RTU entity
		  *
		  */
		unsigned long getPKey() const;

		/**
		  * getTypeKey
		  *
		  * Get the type key of the RTU entity
		  *
		  */
		unsigned long getTypeKey() const;

		/**
		  * getLocationKey
		  *
		  * Get the location key of the RTU entity
		  *
		  */
		unsigned long getLocationKey() const;

		/**
		  * getSubsystemKey
		  *
		  * Get the sub system key of the RTU entity
		  *
		  */
		unsigned long getSubsystemKey() const;

        int getSlaveID() const;

        //TD10586
		unsigned int getRTUFtpPort();
		std::string getRTUFtpUsername();
		std::string getRTUFtpPassword();
		std::string getRTUConfigFilePath();
		std::string getPMSConfigFilePath();

		/**
		  * isInService
		  *
		  * Get the indication if the RTU entity is in service or not.
		  * Returns true if the RTU entity is in service.
		  *
		  */
        bool isInService() const;

		int getMaxCommsRetries() const;

		/**
		  * addDataPoint
		  *
		  * Add the specified DataPoint object to the list of data point
		  * controlled and monitored by the RTU entity
		  *
		  * @param theDataPoint	the DataPoint object that is monitored and controlled
		  *						by the RTU entity
		  */
		bool addDataPoint( TA_Base_Bus::DataPoint * dp );

		void removeDataPoint( TA_Base_Bus::DataPoint * dp );

		/**
		  * createOutputWorkerForSystem
		  *
		  * Create output worker thread associated with the StationSystem entity that is managed
		  * by the RTU entity
		  *
		  * @param pkey		the pkey of the StationSystem entity
		  * @param typeKey	the type key of the StationSystem entity
		  * @param name		the name of the StationSystem entity
		  * @param outputServiceNumber	the output service number of the StationSystem entity
		  * @param outputStartAddress	the output start address of the StationSystem entity
		  * @param outputEndAddress		the output end address of the StationSystem entity
		  * @param slaveID				the modbus slave ID associated with the StationSystem entity
		  *
		  */
		void createOutputWriterForSystem( unsigned long pkey,
										  unsigned long typeKey,
										  const std::string & name,
										  const std::string & outputServiceNumber,
										  unsigned long outputStartAddress,
										  unsigned long outputEndAddress,
                                          int slaveID );

        void updateRTUConfiguration(const TA_Base_Core::ConfigUpdateDetails & updateEvent);
        
        void updateInServiceParameter( bool newState );
		void updateScanTimeMSecs( int scanTimeMSecs );
		void updateCommsErrorRetries( int retries );
		void updatePollTimeout( int pollTimeout );  //wenching
		void updateCommandTimeout( int commandTimeout );  //wenching
		bool getIsInControlMode();
		void updateDataPointWithBadQuality( bool toBeUpdatedWithBadQuality );
		//TD10586
		bool processWriteRequest( TA_Base_Bus::DataPointWriteRequest & newWrite );
		void updateAnalogueDpUpdateInterval(int analogueDpUpdateInterval); //TD16169

        void processPollingData(const TA_Base_Bus::DataBlock< WORD > & addressBlock);
        bool getIsCommsWithStationSystemsValid ( int systemID ) const;
        
		//
		// operations to monitor RTU status
		//

		bool getIsCommsOK() const;
		bool getIsPrimaryRTU() const;
		bool getIsRTUSwitching() const;
		bool getIsDownloadRequired() const;//TD10586
		int getSWCWeightage() const;
		//
		// operations to command RTU
		//

		bool resetRTU();
		bool switchPrimaryToStandby();
		bool switchStandbyToPrimary();
		bool setDownloadRequired();
		bool setDownloadCompleted(bool isSucceeded); //TD10586
		bool uploadConfigFile(); //TD10586

		bool enablePOWER1Polling();
		bool enablePOWER2Polling();
		bool enablePOWER3Polling();
		bool enableCLOCKPolling();
		bool enableWILDPolling();
		bool enableECSPolling();
		bool enableFPSPolling();
		bool enableTWPPolling();
		bool enablePSD1Polling();
		bool enablePSD2Polling();
		bool enablePLC1Polling();
		bool enablePLC2Polling();
		bool enablePLC3Polling();
		bool enablePLC4Polling();

		bool inhibitPOWER1Polling();
		bool inhibitPOWER2Polling();
		bool inhibitPOWER3Polling();
		bool inhibitCLOCKPolling();
		bool inhibitWILDPolling();
		bool inhibitECSPolling();
		bool inhibitFPSPolling();
		bool inhibitTWPPolling();
		bool inhibitPSD1Polling();
		bool inhibitPSD2Polling();
		bool inhibitPLC1Polling();
		bool inhibitPLC2Polling();
		bool inhibitPLC3Polling();
		bool inhibitPLC4Polling();

	// operations
	private:

        bool getAreAddressesValid( unsigned long startAddress, unsigned long endAddress );
		void startProcessing();
		void stopProcessing();
        void startPollingRTUStatus();
        void stopPollingRTUStatus();
        
        // disabled contructors
		RTU();
		RTU( const RTU & obj );
		void operator= ( const RTU & rhs );

	// attributes
	private:
		
        // cache rtu entitydata
        TA_Base_Core::RTUEntityDataPtr m_rtuEntityData;
        
        std::string m_name;
		std::string m_address;
		unsigned long m_primaryKey;
		unsigned long m_typeKey;
		unsigned long m_locationKey;
		unsigned long m_subsystemKey;
		bool m_inService;
        bool m_isInControlMode;
		bool m_agentInControlMode;
		bool m_pollingStarted;
        bool m_commsErrorAlarmGenerated;
        bool m_isStandardTcpModbusDevice;
        bool m_tcpNoDelay;
		TA_Base_Bus::EQualityStatus m_qualityStatus;

		volatile bool m_isUploadingConfigFile; //TD10586

		int m_slaveID;
		int m_commsErrorRetries;
        int m_pollTimeout;   //wenching
		int m_commandTimeout;   //wenching

        int m_analogueDpUpdateInterval; // TD16169
        
		RTUCommandWorker *  m_rtuCommand;
		RTUDownloadWorker m_rtuDownloadWorker;
        StationSystemOutputWriters m_outputWriters;
		RTUPollingWorker *  m_rtuPolling;
        RTUStatusWorker * m_rtuStatus;
        RTUDpProcessor * m_rtuInternalDpProcessor;

        RTUManager& m_rtuManager;

		//TD18093,jianghp
        //TA_Base_Core::AlarmHelper &             m_alarmHelper;
		//TD18093,jianghp
        TA_Base_Core::AuditMessageSender *      m_auditMessageSender;
		
        TA_Base_Core::ReEntrantThreadLockable   m_alarmGenereatedLock;
	};
}
#endif
