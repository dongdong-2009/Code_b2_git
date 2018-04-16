/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/PRIV_3001/3001/transactive/app/scada/PMSAgent/src/RTU.cpp $
  * @author:	HoaVu
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2018/03/14 13:51:20 $
  * Last modified by:	$Author: lim.cy $
  *
  * Description:
  *
  *	The RTU object manages a set of DataPoints that have been configured for it.
  *
  */

#include <sstream>

#include "app/scada/PMSAgent/src/RTU.h"
#include "app/scada/PMSAgent/src/RTUManager.h"
#include "app/scada/PMSAgent/src/RTUCommandWorker.h"
#include "app/scada/PMSAgent/src/RTUPollingWorker.h"
#include "app/scada/PMSAgent/src/RTUStatusWorker.h"
#include "app/scada/PMSAgent/src/RTUDpProcessor.h"

#include "bus/modbus_comms/src/DataBlock.h"
#include "bus/modbus_comms/src/ModbusException.h"
#include "bus/modbus_comms/src/ModbusTcpConnection.h"
#include "bus/modbus_comms/src/SteModbusTcpConnection.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"

#include "core/alarm/src/AlarmHelper.h"
#include "core/alarm/src/AlarmHelperManager.h"
#include "core/exceptions/src/GenericAgentException.h"
#include "core/message/src/AuditMessageSender.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/types/PMSAgentAlarms_MessageTypes.h"
#include "core/message/types/PMSAgentAudit_MessageTypes.h"
#include "core/utilities/src/RunParams.h"

using TA_Base_Core::DebugUtil;
using TA_Base_Core::GenericAgentException;

//TD15792, remove AvalancheHeaderDataPointTable processing codes
namespace TA_IRS_App
{
    RTU::RTU (RTUManager& rtuManager, TA_Base_Core::RTUEntityDataPtr rtuEntityData)
	    :
        m_rtuEntityData(rtuEntityData),
	    m_name(rtuEntityData->getName()),
	    m_address(rtuEntityData->getAddress()),
        m_primaryKey(rtuEntityData->getKey()),
        m_typeKey(rtuEntityData->getTypeKey()),
        m_locationKey(rtuEntityData->getLocation()),
        m_subsystemKey(rtuEntityData->getSubsystem()),
        m_inService(rtuEntityData->getInServiceState()),
        m_isInControlMode(false),
		m_agentInControlMode(false),
        m_pollingStarted(false),
        m_commsErrorAlarmGenerated(false),
		m_raiseAlarmWhenCommsError(rtuEntityData->isRaiseAlarmWhenCommError()),
        m_isStandardTcpModbusDevice(false),
        m_tcpNoDelay(rtuEntityData->getTcpNoDelay()),
		m_qualityStatus(TA_Base_Bus::QUALITY_BAD_NOT_CONNECTED),
	    m_isUploadingConfigFile(false),
        m_slaveID(rtuEntityData->getSlaveID()),
	    m_commsErrorRetries(rtuEntityData->getCommsErrorRetries()),
		m_pollTimeout( rtuEntityData->getPollTimeout() ),
		m_commandTimeout( rtuEntityData->getCommandTimeout() ),
        m_analogueDpUpdateInterval(0),
	    m_rtuCommand(NULL),
	    m_rtuDownloadWorker(*this),
	    m_rtuPolling(NULL),
        m_rtuStatus(NULL),
        m_rtuInternalDpProcessor(NULL),
        m_rtuManager(rtuManager),
	    //TD18093,jianghp, m_alarmHelper(TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper()),
	    m_auditMessageSender(NULL)
	{
        // if defined as "STE"
        std::string modbusDevice = TA_Base_Core::RunParams::getInstance().get("ModbusDevice");
        m_isStandardTcpModbusDevice = (0 != modbusDevice.compare("STE"));                

		// create RTUWorker to look after the RTU Polling table if addresses are valid
		if (false == getAreAddressesValid(m_rtuEntityData->getRTUPollingTableStartAddress(), m_rtuEntityData->getRTUPollingTableEndAddress()))
        {
            std::ostringstream desc;
            desc << "RTU " << m_name << " polling table address invalid;"
                << " rtuPollingTableStartAddress: " << m_rtuEntityData->getRTUPollingTableStartAddress() 
                << " rtuPollingTableEndAddress: " << m_rtuEntityData->getRTUPollingTableEndAddress();

            TA_THROW(GenericAgentException(GenericAgentException::CONFIGURATION_ERROR, desc.str().c_str()));        
        }

        // create RTUWorker to look after the RTU commands if addresses are valid
		if (false == getAreAddressesValid(m_rtuEntityData->getRTUCommandTableStartAddress(), m_rtuEntityData->getRTUCommandTableEndAddress()))
        {
            std::ostringstream desc;
            desc << "RTU " << m_name << " Command table address invalid;"
                << " rtuCommandTableStartAddress: " << m_rtuEntityData->getRTUCommandTableStartAddress() 
                << " rtuCommandTableEndAddress: " << m_rtuEntityData->getRTUCommandTableEndAddress();

			TA_THROW(GenericAgentException(GenericAgentException::CONFIGURATION_ERROR, desc.str().c_str()));        
        }

        m_auditMessageSender = TA_Base_Core::MessagePublicationManager::getInstance().getAuditMessageSender(TA_Base_Core::PMSAgentAudit::Context);
	}

    void RTU::createRTUWorkers()
    {
        // if polling thread already exist, must be called more than once
        TA_ASSERT(NULL == m_rtuPolling, "createRTUWorkers can be only called once");

        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Creating polling worker for RTU [%s]...", getName().c_str());
        // create rtu polling worker
        m_rtuPolling = new RTUPollingWorker (*this,
            m_rtuEntityData->getLocation(),
            m_rtuEntityData->getRTUPollingTableServiceNumber(),
            m_rtuEntityData->getRTUPollingTableStartAddress(),
            m_rtuEntityData->getRTUPollingTableEndAddress(),
            m_rtuEntityData->getRTUStatusTableStartAddress(),
            m_rtuEntityData->getRTUStatusTableEndAddress(),
            m_rtuEntityData->getRTUStatusOffsetAddress(),
            m_rtuEntityData->getScanTimeMSecs(),
            m_slaveID,
            m_commsErrorRetries,
            m_isStandardTcpModbusDevice);
        
        TA_ASSERT(NULL == m_rtuStatus, "createRTUWorkers can be only called once");

        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Creating status worker for RTU [%s]...", getName().c_str());
        // create rtu status worker to process RTU Status data
        // NOTE: RTU Status table range is 0-32 words, but the DataPoint entities
        // associated with the RTU status info is within [0 + statusOffsetAddress, 32 + statusOffsetAddress]
        m_rtuStatus = new RTUStatusWorker(m_rtuEntityData->getRTUStatusOffsetAddress(),
            m_rtuEntityData->getRTUStatusTableStartAddress() + m_rtuEntityData->getRTUStatusOffsetAddress(),
            m_rtuEntityData->getRTUStatusTableEndAddress() + m_rtuEntityData->getRTUStatusOffsetAddress(),
            m_slaveID);
        
        TA_ASSERT(NULL == m_rtuCommand, "createRTUWorkers can be only called once");
        
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Creating output writer for RTU [%s] internal datapoints processing...", getName().c_str());
        //create output writer for command worker
        StationSystemOutputWriter* writer = new StationSystemOutputWriter(*this, 
            m_name,
            m_rtuEntityData->getRTUCommandTableServiceNumber(),
            m_rtuEntityData->getRTUCommandTableStartAddress() + m_rtuEntityData->getRTUStatusOffsetAddress(),
            m_rtuEntityData->getRTUCommandTableEndAddress() + m_rtuEntityData->getRTUStatusOffsetAddress(),
            m_slaveID,
            m_rtuEntityData->getRTUStatusOffsetAddress());
        
        m_outputWriters.push_back(writer);
        
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Creating command worker for RTU [%s]...", getName().c_str());
        // create output writer
        m_rtuCommand = new RTUCommandWorker (*this, *writer);

        TA_ASSERT(NULL == m_rtuInternalDpProcessor, "createRTUWorkers can be only called once");

        // create station system worker for RTU status datapoint processing
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Creating station system worker for RTU [%s] internal datapoints processing...", getName().c_str());
        std::vector<std::pair<unsigned long, unsigned long> > statusBlockAddress;

		std::pair<unsigned long, unsigned long> statusAddressPair;
		statusAddressPair.first = m_rtuEntityData->getRTUStatusTableStartAddress() + m_rtuEntityData->getRTUStatusOffsetAddress();
		statusAddressPair.second = m_rtuEntityData->getRTUStatusTableEndAddress() + m_rtuEntityData->getRTUStatusOffsetAddress();
		statusBlockAddress.push_back(statusAddressPair);
        
        m_rtuInternalDpProcessor = new RTUDpProcessor(
            m_rtuManager.getStationName(),
            m_name,
            m_rtuEntityData->getRTUPollingTableStartAddress() + m_rtuEntityData->getRTUStatusOffsetAddress(),
            m_rtuEntityData->getRTUPollingTableEndAddress() + m_rtuEntityData->getRTUStatusOffsetAddress(),
            m_rtuEntityData->getRTUStatusOffsetAddress(),
            statusBlockAddress,
            m_slaveID,
			m_slaveID,	// as stationsystem status word number.
            m_analogueDpUpdateInterval
            );
        
    }
    
	RTU::~RTU()
	{
        delete m_rtuInternalDpProcessor;
        m_rtuInternalDpProcessor = NULL;
        
        delete m_rtuPolling;
        m_rtuPolling = NULL;

        delete m_rtuStatus;
        m_rtuStatus = NULL;
    
        // clean up the rtu command worker if available
        delete m_rtuCommand;
        m_rtuCommand = NULL;

		// clean up other output workers
		for (StationSystemOutputWriters::iterator itr = m_outputWriters.begin();
              itr != m_outputWriters.end();
              ++itr)
		{
			(*itr)->setInServiceState(false);
			delete (*itr);
			*itr = NULL;
		}
		
        m_outputWriters.clear();

		TA_Base_Core::AlarmHelperManager::getInstance().cleanUp();
        
        delete m_auditMessageSender;
        m_auditMessageSender = NULL;
	}


	const std::string & RTU::getName() const
	{
		return m_name;
	}


	const std::string & RTU::getAddress() const
	{
		return m_address;
	}


	unsigned long RTU::getPKey() const
	{
		return m_primaryKey;
	}


	unsigned long RTU::getTypeKey() const
	{
		return m_typeKey;
	}


	unsigned long RTU::getLocationKey() const
	{
		return m_locationKey;
	}


	unsigned long RTU::getSubsystemKey() const
	{
		return m_subsystemKey;
	}

    
    int RTU::getSlaveID() const
    {
        return m_slaveID;
    }
    
    
    unsigned long RTU::adjustAddress(unsigned long registerAddress)
    {
        // Standard Modbus device is 0-based address
		if (true == m_isStandardTcpModbusDevice)
		{
			return registerAddress - 1;
		}

        return registerAddress;
    }


	void RTU::updateCommsAlarm(bool raiseAlarm, const std::string & servicePortNumber, const std::string & additionalComment)
    {
		if ( false == m_raiseAlarmWhenCommsError ) 
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Comms error but suppress alarm");
			return;
		}
		
        TA_THREADGUARD(m_alarmGenereatedLock);

        // if the current comms status is bad then raise a comms alarm and log
        // the a new event
        if (true == raiseAlarm)
        {
            // if no alarm and event have been generated
            if (false == m_commsErrorAlarmGenerated)
            {
                // construct the TA_Base_Core::DescriptionParameters list
                TA_Base_Core::DescriptionParameters descriptionParameters;
                TA_Base_Core::NameValuePair pair1("RTUName", m_name);
                TA_Base_Core::NameValuePair pair2("ServicePortNumber", servicePortNumber);
                TA_Base_Core::NameValuePair pair3("ModbusException", additionalComment);
                descriptionParameters.push_back(&pair1);
                descriptionParameters.push_back(&pair2);
                descriptionParameters.push_back(&pair3);

                try
                {
                    //submit an unique RTU comms alarm
                    TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().submitAlarmWithAsset(TA_Base_Core::PMSAgentAlarms::RTUCommsError,
                                                m_primaryKey,				// entity key
												m_typeKey,					// entity type key
                                                descriptionParameters,
                                                m_name,						// entity name
                                                m_locationKey,				// location key
                                                m_subsystemKey,				// subystem key
												m_rtuManager.getAgentAssetName(),
												TA_Base_Core::AlarmConstants::defaultTime);
                }
                catch (...)
                {
                }
                
                // set up the flags
                m_commsErrorAlarmGenerated = true;
            }
        }
        else
		{
            //
            // Else if the current RTU state is good then close the comms alarms
            // and log a new event
			//
			// if alarm and event have been generated
			if (true == m_commsErrorAlarmGenerated)
			{
				// close the unique RTU Comms alarm
				TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().closeAlarmAtLocation(TA_Base_Core::PMSAgentAlarms::RTUCommsError,
                                                     m_primaryKey,
                                                     m_locationKey);
                
                // set up the flags
				m_commsErrorAlarmGenerated = false;
			}
		}
    }


    void RTU::submitAuditMessage(const TA_Base_Core::DescriptionParameters & description,
                                  const TA_Base_Core::MessageType & messageType)
    {
        TA_ASSERT(NULL != m_auditMessageSender, "m_auditMessageSender is NULL in RTU::submitAuditMessage");
        
        TA_Base_Core::DescriptionParameters descriptionParameters(description);
        TA_Base_Core::NameValuePair nvpRTUName("RTUName", m_name);
        descriptionParameters.push_back (&nvpRTUName);

        try
		{
            m_auditMessageSender->sendAuditMessage(messageType,				// message type
                                                    m_primaryKey,				// entity key
												    descriptionParameters,		// description
												    "",							// details
												    TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID),	// session ID
                                                    "",							// alarm ID
                                                    "",							// incidentkey
                                                    "");						// parent event ID
		}
        catch(const TA_Base_Core::TransactiveException& te)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::TransactiveException", te.what());
        }
		catch (...)
		{
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown exception", "Caught unknown exception while sending audit message");
		}
    }


    TA_Base_Bus::IModbus * RTU::createModbusConnection(const std::string & rtuServicePortNumber, int slaveID, ModbusConnectionType connectionType)
    {
		TA_ASSERT( connectionType == CONNTYPE_COMMAND || 
			       connectionType == CONNTYPE_POLL, "Modbus connection type is invalid in RTU::createModbusConnection" );

		TA_Base_Bus::IModbus * modbusConnection = NULL;

		//
		// create and enable TCP Modbus Connection, either STE specific device or
		// standard TCP Modbus device, depending on runtime parameter
		//

		int timeout = 0;
		std::string connType;
		switch(connectionType)
		{
		case CONNTYPE_COMMAND:
			timeout = m_commandTimeout;
			break;

		case CONNTYPE_POLL:
		default:
			timeout = m_pollTimeout;
		}

        if (true == m_isStandardTcpModbusDevice)
		{
			modbusConnection = new TA_Base_Bus::ModbusTcpConnection(m_address,
                                                                     rtuServicePortNumber,
                                                                     timeout,
                                                                     m_tcpNoDelay);
		}
		else
		{
			modbusConnection = new TA_Base_Bus::SteModbusTcpConnection(m_address,
                                                                        rtuServicePortNumber,
                                                                        timeout,
                                                                        m_tcpNoDelay);
		}

		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "RTU::createModbusConnection - Timeout=%d",timeout);

		modbusConnection->setDeviceAddress(slaveID);
		modbusConnection->disable();

		return modbusConnection;
    }


    void RTU::destroyModbusConnection(TA_Base_Bus::IModbus * & modbusConnection)
    {
        TA_ASSERT(NULL != modbusConnection, "modbusConnection == NULL in RTU::destroyModbusConnection");

        modbusConnection->disable();
        
        delete modbusConnection;
        modbusConnection = NULL;
    }


    bool RTU::isInService() const
	{
		return m_inService;
	}


	void RTU::start()
	{
	}


	void RTU::stop()
	{
	}

	bool RTU::isValid()
	{
		return true;
	}


	void RTU::update(const TA_Base_Core::ConfigUpdateDetails& updateEvent)
	{
		// to be implemented
	}


	void RTU::remove()
	{
		// do nothing.  Destructor will clean up the data point
	}

	void RTU::setToControlMode()
	{
        // set the flag
		m_isInControlMode = true;

		// if the RTU is IN_SERVICE
		if (true == m_inService)
		{
			startProcessing();
		}

		m_rtuInternalDpProcessor->setOperationMode(TA_Base_Core::Control);
	}


	void RTU::setToMonitorMode()
	{
        // set the flag
		m_isInControlMode = false;

		stopProcessing();

		m_rtuInternalDpProcessor->setOperationMode(TA_Base_Core::Monitor);
	}

	void RTU::setOperationMode(TA_Base_Core::EOperationMode mode)
	{
		if (TA_Base_Core::Control == mode)
		{
			// set the flag
			m_isInControlMode = true;

			// if the RTU is IN_SERVICE
			if (true == m_inService)
			{
				startProcessing();
			}
		}
		else
		{
			// set the flag
			m_isInControlMode = false;

			stopProcessing();
		}

		m_rtuInternalDpProcessor->setOperationMode(mode);
	}

	bool RTU::getIsInControlMode()
	{
		return m_isInControlMode;
	}


	bool RTU::addDataPoint(TA_Base_Bus::DataPoint * dp)
	{
        TA_ASSERT(NULL != dp, "dp is NULL in RTU::addDataPoint");

        // only hold output datapoints
        bool isAdded = false;
        if (dp->isWriteable())
		{
			for (StationSystemOutputWriters::iterator itr = m_outputWriters.begin();
                  isAdded == false && itr != m_outputWriters.end();
                  ++itr)
			{
				isAdded =(*itr)->addDataPoint(dp);
			}
		}
        else
        {
            if (NULL != m_rtuInternalDpProcessor)
            {
                isAdded = m_rtuInternalDpProcessor->addDataPoint(dp);
            }
        }

        return isAdded;
	}


	void RTU::removeDataPoint (TA_Base_Bus::DataPoint * dp)
	{
        TA_ASSERT(NULL != dp, "dp is NULL in RTU::removeDataPoint");

		for (StationSystemOutputWriters::iterator itr = m_outputWriters.begin();
              itr != m_outputWriters.end();
              ++itr)
		{
			(*itr)->removeDataPoint(dp);
		}
	}


	void RTU::createOutputWriterForSystem(unsigned long pkey,
										   unsigned long typeKey,
										   const std::string & name,
										   const std::string & outputServiceNumber,
										   unsigned long outputStartAddress,
										   unsigned long outputEndAddress,
										   int slaveID)
	{
		// create RTUWorker to look after the RTU output table if addresses are valid
		if (true == getAreAddressesValid(outputStartAddress, outputEndAddress))
		{
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Creating output writer [%s] for RTU [%s]", name.c_str(), getName().c_str());

            StationSystemOutputWriter* writer = new StationSystemOutputWriter(*this, 
				name,
				outputServiceNumber,
				outputStartAddress,
				outputEndAddress,
				slaveID);
			
			m_outputWriters.push_back(writer);
		}
	}


	bool RTU::getIsPrimaryRTU() const
	{
        TA_ASSERT(NULL != m_rtuStatus, "m_rtuStatus is NULL in RTU::getIsPrimaryRTU");

        return m_rtuStatus->getIsPrimaryRTU();
	}


	bool RTU::getIsRTUSwitching() const
	{
        TA_ASSERT(NULL != m_rtuStatus, "m_rtuStatus is NULL in RTU::getIsRTUSwitching");

        return m_rtuStatus->getIsRTUSwitching();
	}


	//TD10586
	bool RTU::getIsDownloadRequired() const
	{
        TA_ASSERT(NULL != m_rtuStatus, "m_rtuStatus is NULL in RTU::getIsDownloadRequired");
		
        return m_rtuStatus->getIsDownloadRequired();
	}
	

	int RTU::getSWCWeightage() const
	{
        TA_ASSERT(NULL != m_rtuStatus, "m_rtuStatus is NULL in RTU::getSWCWeightage");

        return m_rtuStatus->getSWCWeightage();
	}


	bool RTU::resetRTU()
	{
        TA_ASSERT(NULL != m_rtuCommand, "m_rtuCommand is NULL in RTU::resetRTU");
        
        return m_rtuCommand->resetRTU();
	}


	bool RTU::switchPrimaryToStandby()
	{
        TA_ASSERT(NULL != m_rtuCommand, "m_rtuCommand is NULL in RTU::switchPrimaryToStandby");
        
        return m_rtuCommand->switchPrimaryToStandby();
	}


	bool RTU::switchStandbyToPrimary()
	{
        TA_ASSERT(NULL != m_rtuCommand, "m_rtuCommand is NULL in RTU::switchStandbyToPrimary");

        return m_rtuCommand->switchStandbyToPrimary();
	}


	bool RTU::setDownloadRequired()
	{
        TA_ASSERT(NULL != m_rtuCommand, "m_rtuCommand is NULL in RTU::setDownloadRequired");
        
        return m_rtuCommand->setDownloadRequired();
	}


	//TD10586
	bool RTU::setDownloadCompleted(bool isSucceeded)
	{
        TA_ASSERT(NULL != m_rtuCommand, "m_rtuCommand is NULL in RTU::setDownloadCompleted");

		if (isSucceeded == true)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Downloading config file succeeded, to send download completed and reset RTU.");
			m_rtuCommand->setDownloadCompleted();

			//if reseting rtu very soon after download completed, RTU will lost the config file, don't know why...
			//So wait max 5 second for rtu getting ready before reset.
			for (int i = 10; i > 0; -- i)
			{
				TA_Base_Core::Thread::sleep(500);
				
				if (getIsDownloadRequired() == false)
				{
					break;
				}
			}
			
			m_isUploadingConfigFile = false;

			return m_rtuCommand->resetRTU();
		}
		
		m_isUploadingConfigFile = false;

		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Downloading config file failed, need to check if the source file exists and rtu ftp server can be connected.");
		return isSucceeded;
	}

	//TD10586
	bool RTU::uploadConfigFile()
	{
		if (m_isUploadingConfigFile == true)
		{
			//if download is in processing, do nothing
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Download is in progress...");
			return true;
		}

		//to indicate download is in processing
		m_isUploadingConfigFile = true;
		
		//to start download process.
		m_rtuDownloadWorker.startDownload();

		return true;
	}
	
	//TD10586
	unsigned int RTU::getRTUFtpPort()
	{
		return m_rtuEntityData->getRTUFtpPort();
	}

	std::string RTU::getRTUFtpUsername()
	{
		return m_rtuEntityData->getRTUFtpUsername();
	}

	std::string RTU::getRTUFtpPassword()
	{
		return m_rtuEntityData->getRTUFtpPassword();
	}

	std::string RTU::getRTUConfigFilePath()
	{
		return m_rtuEntityData->getRTUConfigFilePath();
	}

	std::string RTU::getPMSConfigFilePath()
	{
		return m_rtuEntityData->getPMSConfigFilePath();
	}
	
	bool RTU::enablePOWER1Polling()
	{
        TA_ASSERT(NULL != m_rtuCommand, "m_rtuCommand is NULL in RTU::enablePOWER1Polling");

        return m_rtuCommand->enablePOWER1Polling();
	}


	bool RTU::enablePOWER2Polling()
	{
        TA_ASSERT(NULL != m_rtuCommand, "m_rtuCommand is NULL in RTU::enablePOWER2Polling");

        return m_rtuCommand->enablePOWER2Polling();
	}


	bool RTU::enablePOWER3Polling()
	{
        TA_ASSERT(NULL != m_rtuCommand, "m_rtuCommand is NULL in RTU::enablePOWER3Polling");
        
        return m_rtuCommand->enablePOWER3Polling();
	}


	bool RTU::enableCLOCKPolling()
	{
        TA_ASSERT(NULL != m_rtuCommand, "m_rtuCommand is NULL in RTU::enableCLOCKPolling");

        return m_rtuCommand->enableCLOCKPolling();
	}


	bool RTU::enableWILDPolling()
	{
        TA_ASSERT(NULL != m_rtuCommand, "m_rtuCommand is NULL in RTU::enableWILDPolling");
        
        return m_rtuCommand->enableWILDPolling();
	}


	bool RTU::enableECSPolling()
	{
        TA_ASSERT(NULL != m_rtuCommand, "m_rtuCommand is NULL in RTU::enableECSPolling");
        
        return m_rtuCommand->enableECSPolling();
	}


	bool RTU::enableFPSPolling()
	{
        TA_ASSERT(NULL != m_rtuCommand, "m_rtuCommand is NULL in RTU::enableFPSPolling");
        
        return m_rtuCommand->enableFPSPolling();
	}


	bool RTU::enableTWPPolling()
	{
        TA_ASSERT(NULL != m_rtuCommand, "m_rtuCommand is NULL in RTU::enableTWPPolling");
        
        return m_rtuCommand->enableTWPPolling();
	}


	bool RTU::enablePSD1Polling()
	{
        TA_ASSERT(NULL != m_rtuCommand, "m_rtuCommand is NULL in RTU::enablePSD1Polling");

        return m_rtuCommand->enablePSD1Polling();
	}
		

	bool RTU::enablePSD2Polling()
	{
        TA_ASSERT(NULL != m_rtuCommand, "m_rtuCommand is NULL in RTU::enablePSD2Polling");
       
        return m_rtuCommand->enablePSD2Polling();
	}


	bool RTU::enablePLC1Polling()
	{
        TA_ASSERT(NULL != m_rtuCommand, "m_rtuCommand is NULL in RTU::enablePLC1Polling");
       
        return m_rtuCommand->enablePLC1Polling();
	}


	bool RTU::enablePLC2Polling()
	{
        TA_ASSERT(NULL != m_rtuCommand, "m_rtuCommand is NULL in RTU::enablePLC2Polling");
       
        return m_rtuCommand->enablePLC2Polling();
	}


	bool RTU::enablePLC3Polling()
	{
        TA_ASSERT(NULL != m_rtuCommand, "m_rtuCommand is NULL in RTU::enablePLC3Polling");

        return m_rtuCommand->enablePLC3Polling();
	}


	bool RTU::enablePLC4Polling()
	{
        TA_ASSERT(NULL != m_rtuCommand, "m_rtuCommand is NULL in RTU::enablePLC4Polling");

        return m_rtuCommand->enablePLC4Polling();
	}


	bool RTU::inhibitPOWER1Polling()
	{
        TA_ASSERT(NULL != m_rtuCommand, "m_rtuCommand is NULL in RTU::inhibitPOWER1Polling");
	
		return m_rtuCommand->inhibitPOWER1Polling();
	}


	bool RTU::inhibitPOWER2Polling()
	{
        TA_ASSERT(NULL != m_rtuCommand, "m_rtuCommand is NULL in RTU::inhibitPOWER2Polling");

        return m_rtuCommand->inhibitPOWER2Polling();
	}


	bool RTU::inhibitPOWER3Polling()
	{
		TA_ASSERT(NULL != m_rtuCommand, "m_rtuCommand is NULL in RTU::inhibitPOWER3Polling");

        return m_rtuCommand->inhibitPOWER3Polling();
	}


	bool RTU::inhibitCLOCKPolling()
	{
		TA_ASSERT(NULL != m_rtuCommand, "m_rtuCommand is NULL in RTU::inhibitCLOCKPolling");

        return m_rtuCommand->inhibitCLOCKPolling();
	}


	bool RTU::inhibitWILDPolling()
	{
		TA_ASSERT(NULL != m_rtuCommand, "m_rtuCommand is NULL in RTU::inhibitWILDPolling");

        return m_rtuCommand->inhibitWILDPolling();
	}


	bool RTU::inhibitECSPolling()
	{
		TA_ASSERT(NULL != m_rtuCommand, "m_rtuCommand is NULL in RTU::inhibitECSPolling");

        return m_rtuCommand->inhibitECSPolling();
	}


	bool RTU::inhibitFPSPolling()
	{
		TA_ASSERT(NULL != m_rtuCommand, "m_rtuCommand is NULL in RTU::inhibitFPSPolling");

        return m_rtuCommand->inhibitFPSPolling();
	}


	bool RTU::inhibitTWPPolling()
	{
		TA_ASSERT(NULL != m_rtuCommand, "m_rtuCommand is NULL in RTU::inhibitTWPPolling");

        return m_rtuCommand->inhibitTWPPolling();
	}


	bool RTU::inhibitPSD1Polling()
	{
		TA_ASSERT(NULL != m_rtuCommand, "m_rtuCommand is NULL in RTU::inhibitPSD1Polling");

        return m_rtuCommand->inhibitPSD1Polling();
	}


	bool RTU::inhibitPSD2Polling()
	{
		TA_ASSERT(NULL != m_rtuCommand, "m_rtuCommand is NULL in RTU::inhibitPSD2Polling");

        return m_rtuCommand->inhibitPSD2Polling();
	}


	bool RTU::inhibitPLC1Polling()
	{
		TA_ASSERT(NULL != m_rtuCommand, "m_rtuCommand is NULL in RTU::inhibitPLC1Polling");

        return m_rtuCommand->inhibitPLC1Polling();
	}


	bool RTU::inhibitPLC2Polling()
	{
		TA_ASSERT(NULL != m_rtuCommand, "m_rtuCommand is NULL in RTU::inhibitPLC2Polling");

        return m_rtuCommand->inhibitPLC2Polling();
	}


	bool RTU::inhibitPLC3Polling()
	{
		TA_ASSERT(NULL != m_rtuCommand, "m_rtuCommand is NULL in RTU::inhibitPLC3Polling");

        return m_rtuCommand->inhibitPLC3Polling();
    }


	bool RTU::inhibitPLC4Polling()
	{
		TA_ASSERT(NULL != m_rtuCommand, "m_rtuCommand is NULL in RTU::inhibitPLC4Polling");
        
        return m_rtuCommand->inhibitPLC4Polling();
	}

	
	bool RTU::getAreAddressesValid (unsigned long startAddress, unsigned long endAddress)
	{
		if ((0 == startAddress) && (0 == endAddress) ||
			 (0 > startAddress) || (0 > endAddress) ||
			 (startAddress > endAddress))
		{
			return false;
		}

		return true;
	}


	bool RTU::getIsCommsOK() const
	{
		TA_ASSERT(NULL != m_rtuStatus, "m_rtuStatus is NULL in RTU::getIsCommsOK");

		return m_rtuStatus->getIsCommsOK();
	}


	void RTU::startProcessing()
	{
        startPollingRTUStatus();
	}


	void RTU::stopProcessing()
	{
        stopPollingRTUStatus();
    }


	//TD10586
	bool RTU::processWriteRequest(TA_Base_Bus::DataPointWriteRequest & newWrite)
	{
		bool isProcessed = false;

		for (StationSystemOutputWriters::iterator itr = m_outputWriters.begin();
			isProcessed == false && itr != m_outputWriters.end();
			itr++)
		{
            // if station system slaveId equil to rtu's slaveId, then should process write request
            // else only process write request when this rtu is primary
            if (getIsPrimaryRTU() || (*itr)->getSlaveID() == m_slaveID)
            {
                isProcessed = (*itr)->processWriteRequest(newWrite);
            }
		}

		return isProcessed;
	}

    void RTU::updateRTUConfiguration(const TA_Base_Core::ConfigUpdateDetails & updateEvent)
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "RTU [%s] received a config update event", getName().c_str());
        
        switch (updateEvent.getModifications())
        {
        // only if the RTU has been updated in the database
        case TA_Base_Core::Update:
            {
                // get the list of changed parameter from the config update
                const std::vector< std::string > * changes = &(updateEvent.getChangedParams());
                
                if ((0 != changes) && (false == changes->empty()))
                {
                    try
                    {
                        // force rtu entity data to be reloaded
                        m_rtuEntityData->invalidate();
                        
                        for (unsigned long i = 0; i < changes->size(); i++)
                        {
                            if (TA_Base_Core::RTUEntityData::INSERVICE == (*changes)[i])
                            {
                                LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Updating InService state of %s", getName().c_str());
                                updateInServiceParameter (m_rtuEntityData->getInServiceState());
                            }
                            else if (TA_Base_Core::RTUEntityData::SCANTIME == (*changes)[i])
                            {
                                LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Updating ScanTime of %s", getName().c_str());
                                updateScanTimeMSecs (m_rtuEntityData->getScanTimeMSecs());
                            }
                            else if (TA_Base_Core::RTUEntityData::COMMS_ERROR_RETRIES == (*changes)[i])
                            {
                                LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Updating CommsErrorRetries of %s", getName().c_str());
                                updateCommsErrorRetries (m_rtuEntityData->getCommsErrorRetries());
                            }
                            else if (TA_Base_Core::RTUEntityData::COMMAND_TIMEOUT == (*changes)[i])
                            {
                                LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Updating command timeout of %s", getName().c_str());
                                updateCommandTimeout(m_rtuEntityData->getCommandTimeout());
                            }
							else if (TA_Base_Core::RTUEntityData::POLL_TIMEOUT == (*changes)[i])
                            {
                                LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Updating poll timeout of %s", getName().c_str());
                                updatePollTimeout(m_rtuEntityData->getPollTimeout());
                            }
                        }
                    }
                    catch (...)
                    {
                        LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Exception caught when process rtu entity data");
                    }
                }
                
                break;
            }
            
        // don't care in other case
        default:
            {
                break;
            }
        }
    }
        
	void RTU::updateInServiceParameter(bool newState)
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Received config update for InService parameter...");

		// save the new state
		m_inService = newState;

		// if the RTU has been in control mode
		if (true == m_isInControlMode && true == m_inService)
		{
			startProcessing();
        }
		else
		{
			stopProcessing();
		}
	}


	void RTU::updateScanTimeMSecs(int scanTimeMSecs)
	{
        TA_ASSERT(NULL != m_rtuPolling, "m_rtuPolling is NULL in RTU::updateScanTimeMSecs");

        m_rtuPolling->updateScanTimeMSecs(scanTimeMSecs);
	}


	void RTU::updateCommsErrorRetries(int retries)
	{
        TA_ASSERT(NULL != m_rtuPolling, "m_rtuPolling is NULL in RTU::updateCommsErrorRetries");

        if (m_commsErrorRetries != retries)
        {
            m_commsErrorRetries = retries;
            m_rtuPolling->updateCommsErrorRetries(retries);
        }
	}


	void RTU::updateCommandTimeout( int commandTimeout )
	{
		TA_ASSERT( NULL != m_rtuCommand, "m_rtuCommand is NULL in RTU::updateCommandTimeout" );
        
        if ( m_commandTimeout != commandTimeout )
        {
            m_commandTimeout = commandTimeout;
        }
	}

	void RTU::updatePollTimeout( int pollTimeout )
	{
		TA_ASSERT( NULL != m_rtuPolling, "m_rtuPolling is NULL in RTU::updatePollTimeout" );
        
        if ( m_pollTimeout != pollTimeout )
        {
            m_pollTimeout = pollTimeout;
        }
	}

	int RTU::getMaxCommsRetries() const
	{
		return m_commsErrorRetries;
	}


	void RTU::startPollingRTUStatus()
	{
        TA_ASSERT(NULL != m_rtuPolling, "m_rtuPolling is NULL in RTU::startPollingRTUStatus");

        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Start the polling thread of RTU %s, IP address %s",
            m_name.c_str(), m_address.c_str());

        // only proceed if the RTU is in service
		if (false == m_inService)
		{
			m_pollingStarted = false;

            LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Cannot start polling for status since RTU %s, IP address %s is not IN SERVICE.",
                m_name.c_str(), m_address.c_str());
			return;
		}

        // enable modbus connection
        m_rtuPolling->setInServiceState(true);

		// enable output worker.
		for (StationSystemOutputWriters::iterator itr = m_outputWriters.begin();
              itr != m_outputWriters.end();
              ++itr)
		{
			(*itr)->setInServiceState(true);
		}

		m_pollingStarted = true;
	}


	void RTU::stopPollingRTUStatus()
	{
        TA_ASSERT(NULL != m_rtuPolling, "m_rtuPolling is NULL in RTU::stopPollingRTUStatus");

        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Terminate the polling thread of RTU %s, IP address %s",
            m_name.c_str(), m_address.c_str());
        
        m_rtuPolling->setInServiceState(false);

		// disable output worker.
		for (StationSystemOutputWriters::iterator itr = m_outputWriters.begin();
              itr != m_outputWriters.end();
              ++itr)
		{
			(*itr)->setInServiceState(false);
		}
		
		m_pollingStarted = false;
	}

    //TD16169: update analogue datapoint update interval
    void RTU::updateAnalogueDpUpdateInterval(int analogueDpUpdateInterval)
    {
        m_analogueDpUpdateInterval = analogueDpUpdateInterval;

        if (NULL != m_rtuInternalDpProcessor)
        {
            m_rtuInternalDpProcessor->updateAnalogueDpUpdateInterval(analogueDpUpdateInterval);
        }
    }
    
    
    // called by rtu polling thread and forward to rtu manager
    void RTU::processPollingData(const TA_Base_Bus::DataBlock< WORD > & addressBlock)
    {
        // process rtu status
		m_rtuStatus->processRTUDataBlock(addressBlock, m_rtuPolling->getIsCommsOK());

        // check if download configfile is required
        if (m_rtuStatus->getIsDownloadRequired())
        {
            uploadConfigFile();
        }
        
        // process rtu internal datapoints
        if (m_rtuPolling->getIsCommsOK())
        {
            m_qualityStatus = TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON;
        }
		else if (TA_Base_Bus::QUALITY_BAD_NOT_CONNECTED != m_qualityStatus)
		{
			m_qualityStatus = TA_Base_Bus::QUALITY_BAD_LAST_KNOWN_VALUE;
		}

        m_rtuInternalDpProcessor->processRTUDataBlock(addressBlock, m_qualityStatus);

        // notify status to holder
        m_rtuManager.processRTUStatusChanged(*this);

        // notify holder to process polling datablock
        m_rtuManager.processRTUDataBlock(*this, addressBlock);
    }
    
    bool RTU::getIsCommsWithStationSystemsValid (int systemID) const
    {
        return m_rtuStatus->getIsCommsWithStatusWordNumber(systemID);
    }   
}
