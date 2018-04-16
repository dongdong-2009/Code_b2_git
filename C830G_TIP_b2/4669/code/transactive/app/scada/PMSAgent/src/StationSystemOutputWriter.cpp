/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/4669_T01271350/4669/transactive/app/scada/PMSAgent/src/StationSystemOutputWriter.cpp $
  * @author:	HoaVu
  * @version:	$Revision: #3 $
  *
  * Last modification:	$DateTime: 2009/05/04 18:23:53 $
  * Last modified by:	$Author: builder $
  *
  * Description:
  *
  *	The StationSystemOutputWriter object is responsible for processing write data commands to the PMS RTU
  *
  */

#include "app/scada/PMSAgent/src/StationSystemOutputWriter.h"
#include "app/scada/PMSAgent/src/RTU.h"

#include "bus/scada/datapoint_library/src/DataPoint.h"
#include "bus/modbus_comms/src/IModbus.h"
#include "bus/modbus_comms/src/ModbusException.h"
#include "bus/modbus_comms/src/ModbusTcpConnection.h"
#include "bus/modbus_comms/src/SteModbusTcpConnection.h"

#include "core/synchronisation/src/ThreadGuard.h"
#include "core/synchronisation/src/ThreadReadGuard.h"
#include "core/utilities/src/RunParams.h"
#include "core/data_access_interface/src/SessionAccessFactory.h"


using TA_Base_Core::DebugUtil;

namespace 
{
    bool isSystemSessionID( const std::string& sessionID )//limin++, SystemSession should not access db
    {
        bool ret = ( TA_Base_Core::SessionAccessFactory::getSuperSessionId() == sessionID ||
                     TA_Base_Bus::DPT_INTERNAL_SESSION_ID == sessionID ||
                     TA_Base_Bus::NotLoaded == sessionID );
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[sessionID=%s][return=%d]", sessionID.c_str(), ret );
        return ret;
    }
}


namespace TA_IRS_App
{
	StationSystemOutputWriter::StationSystemOutputWriter ( RTU& rtu,
                                                           const std::string & stationSystemName,
                                                           const std::string & rtuServicePortNumber,
                                                           unsigned long startIOAddress,
                                                           unsigned long endIOAddress,
                                                           int slaveID,
                                                           unsigned long offsetAddress )
	:
    m_rtu (rtu),
	m_stationSystemName ( stationSystemName ),
	m_offsetAddress ( offsetAddress ),
	m_rtuServicePortNumber ( rtuServicePortNumber ),
    m_addressBlock ( startIOAddress, endIOAddress  ),
    m_slaveID ( slaveID ),
	m_inService ( false ),
	m_currentModbusConnection ( NULL )
	{
	}


	StationSystemOutputWriter::~StationSystemOutputWriter()
	{
	}


	bool StationSystemOutputWriter::processWriteRequest ( TA_Base_Bus::DataPointWriteRequest & newWrite )
	{
		// get the data point from the new request
		TA_Base_Bus::DataPoint * dp = newWrite.getDataPoint();

		TA_ASSERT( 0 != dp, "StationSystemOutputWriter::processWriteRequest: newWrite contains a NULL datapoint." );

        RTUOutputDataPointMap::iterator itr = m_dataPoints.find( dp->getEntityKey() );

        if ( itr == m_dataPoints.end() )
        {
            return false;
        }

        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "%s, %s, slave ID %d: start writing process for %s",
            m_rtu.getName().c_str(),
            m_stationSystemName.c_str(),
            m_slaveID,
            dp->getDataPointName().c_str() );
        
        if ( dp->getIsDigitalDataPoint() )
        {
            writeDigitalDataPoint( newWrite );
        }
        else if ( dp->getIsTextDataPoint() )
        {
            // do nothing. To be implemented when required
        }
        else if ( dp->getIsAnalogueDataPoint() )
        {
            writeAnalogueDataPoint( newWrite );
        }
        else
        {
            // do nothing for the current design
        }
        
        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "%s, %s, slave ID %d: disable ModbusConnection after sending output command for %s",
            m_rtu.getName().c_str(),
            m_stationSystemName.c_str(),
            m_slaveID,
            dp->getDataPointName().c_str() );

        return true;
    }


	void StationSystemOutputWriter::writeDigitalDataPoint ( TA_Base_Bus::DataPointWriteRequest & newWrite )
	{
		TA_Base_Bus::DataPoint * point = 0;
		unsigned long registerAddress = 0;
		TA_Base_Bus::DataBlock < bool > coilsDataBlock;

        bool isSystemSession = isSystemSessionID( newWrite.getSessionID() );//limin++, SystemSession should not access db

		// get the data point from the new request
		point = newWrite.getDataPoint();

		// get the data point's register address
		registerAddress = point->getDataPointRegisterAddress();

		registerAddress = registerAddress - m_offsetAddress;

		// Standard Modbus device is 0-based address
		registerAddress = m_rtu.adjustAddress(registerAddress);

		// initialise coils data block, start at register address and length of 1 bit
		coilsDataBlock.setStartAndLength ( registerAddress, 1 );

		// set the value in data block
		coilsDataBlock.set( registerAddress, newWrite.getValue().getBoolean() );

		//
		// sending the command
		//

		// update control status of the data point to indicate that it
		// is waiting for confirmation from the field device
		point->updateControlState ( TA_Base_Bus::pending, isSystemSession ); //wenching++ (TD13999) //limin++, SystemSession should not access db

		// send command now
		if ( true == sendCommand( coilsDataBlock ) )
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
                "Successful to send command to %s, slave ID = %d. Start ABE timeout mechanism for %s",
                m_stationSystemName.c_str(),
                m_slaveID,
                point->getDataPointName().c_str() );

			// activate ABE process for this datapoint
			point->startABETimeout( newWrite.getSessionID() );
		}
		else
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
                "Failed to send command to %s, slave ID = %d, for %s",
                m_stationSystemName.c_str(),
                m_slaveID,
                point->getDataPointName().c_str() );

			// update control status of the data point to indicate that
			// the output command has failed
			point->updateControlState ( TA_Base_Bus::comms_failed, isSystemSession ); //wenching++ (TD13999) //limin++, SystemSession should not access db
		}

		point = 0;
	}


	void StationSystemOutputWriter::writeAnalogueDataPoint ( TA_Base_Bus::DataPointWriteRequest & newWrite )
	{
		TA_Base_Bus::DataPoint * point = 0;
		unsigned long registerAddress = 0;
		TA_Base_Bus::DataBlock < WORD > registerDataBlock;
		bool isValueValid = true;

        bool isSystemSession = isSystemSessionID( newWrite.getSessionID() );//limin++, SystemSession should not access db

		// get the data point from the new request
		point = newWrite.getDataPoint();

		// get the data point's register address
		registerAddress = point->getDataPointRegisterAddress();

		registerAddress = registerAddress - m_offsetAddress;

		registerAddress = m_rtu.adjustAddress(registerAddress);

		switch ( point->getDataPointDataType() )
		{
			case TA_Base_Bus::DPT_INT16_TYPE :
				{
					// initialise register data block
					registerDataBlock.setStartAndLength ( registerAddress, 1 );

					// use the number conversion utility, convert the data point value
					// to INT16 format, where the -ve value is represented in 2's
					// Complement format
					WORD convertedNumber = 0;
					if ( true == m_numberConversionUtil.convertToINT16_Value ( (int)newWrite.getValue().getFloat(), convertedNumber ) )
					{
						// set the value in data block
						registerDataBlock.set( registerAddress, convertedNumber );
					}
					else
					{
						isValueValid = false;
					}
					break;
				}

			case TA_Base_Bus::DPT_INT16S_TYPE :
				{
					// initialise register data block
					registerDataBlock.setStartAndLength ( registerAddress, 1 );

					// use the number conversion utility, convert the data point value
					// to INT16S format, where the -ve value is represented in the most significant bit
					WORD convertedNumber = 0;
					if ( true == m_numberConversionUtil.convertToINT16S_Value ( (int)newWrite.getValue().getFloat(), convertedNumber ) )
					{
						// set the value in data block
						registerDataBlock.set( registerAddress, convertedNumber );
					}
					else
					{
						isValueValid = false;
					}
					break;
				}

			case TA_Base_Bus::DPT_UINT16_TYPE :
				{
					// initialise register data block
					registerDataBlock.setStartAndLength ( registerAddress, 1 );

					// set the value in data block
					registerDataBlock.set( registerAddress, (unsigned int)newWrite.getValue().getFloat() );
					break;
				}

			case TA_Base_Bus::DPT_UINT32_TYPE :
				{
					// initialise register data block
					registerDataBlock.setStartAndLength ( registerAddress, 2 );

					// get the unsigned integer
					unsigned long temp = (unsigned long)newWrite.getValue().getFloat();

					// set the low order 16 bit of the unsigned value in the first address field
					registerDataBlock.set( registerAddress, (WORD) (0x0000FFFF & temp) );

					// set the high order 16 bit of the unsigned value in the second address field
					registerDataBlock.set( registerAddress + 1, (WORD) ((0xFFFF0000 & temp) >> 16) );

					break;
				}

			case TA_Base_Bus::DPT_INT32_TYPE :
				{
					// initialise register data block
					registerDataBlock.setStartAndLength ( registerAddress, 2 );

					// use the number conversion utility, convert the data point value
					// to INT32 format, where the -ve value is represented in 2's
					// Complement format
					double convertedNumber = 0;
					if ( true == m_numberConversionUtil.convertToINT32_Value ( newWrite.getValue().getFloat(), convertedNumber ) )
					{
						// set low order 16 bit of the value in first address of data block
						registerDataBlock.set( registerAddress, (WORD) (0x0000FFFF & (long) convertedNumber) );

						// set high order 16 bit of the value in second address of data block
						registerDataBlock.set( registerAddress, (WORD) ((0xFFFF0000 & (long) convertedNumber) >> 16) );
					}
					else
					{
						isValueValid = false;
					}
					break;
				}

			case TA_Base_Bus::DPT_INT32S_TYPE :
				{
					// initialise register data block
					registerDataBlock.setStartAndLength ( registerAddress, 2 );

					// use the number conversion utility, convert the data point value
					// to INT32S format, where the -ve value is represented in the most significant bit
					double convertedNumber = 0;
					if ( true == m_numberConversionUtil.convertToINT32S_Value ( newWrite.getValue().getFloat(), convertedNumber ) )
					{
						// set low order 16 bit of the value in first address of data block
						registerDataBlock.set( registerAddress, (WORD) (0x0000FFFF & (long) convertedNumber) );

						// set high order 16 bit of the value in second address of data block
						registerDataBlock.set( registerAddress, (WORD) ((0xFFFF0000 & (long) convertedNumber) >> 16) );
					}
					else
					{
						isValueValid = false;
					}
					break;
				}

			case TA_Base_Bus::DPT_IEEE32_TYPE :
				{
					// not implemented yet

					isValueValid = false;
					break;
				}

            //TD11070, consider enum datatype as common int16
            case TA_Base_Bus::DPT_ENUM_TYPE :
                {
                    // initialise register data block
                    registerDataBlock.setStartAndLength ( registerAddress, 1 );
                    
                    // use the number conversion utility, convert the data point value
                    // to INT16 format, where the -ve value is represented in 2's
                    // Complement format
                    WORD convertedNumber = 0;
                    if ( true == m_numberConversionUtil.convertToINT16_Value ( (int)newWrite.getValue().getEnum(), convertedNumber ) )
                    {
                        // set the value in data block
                        registerDataBlock.set( registerAddress, convertedNumber );
                    }
                    else
                    {
                        isValueValid = false;
                    }
                    break;
                }
                
            default :
				{
					isValueValid = false;
					break;
				}
		}

		if ( isValueValid )
		{
			//
			// sending the command
			//

			// update control status of the data point to indicate that it
			// is waiting for confirmation
			point->updateControlState ( TA_Base_Bus::pending, isSystemSession ); //wenching++ (TD13999) //limin++, SystemSession should not access db

			// send command now
			if ( true == sendCommand ( registerDataBlock ) )
			{
				LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
                    "Successful to send command to %s, slave ID = %d. Start ABE timeout mechanism for %s",
                    m_stationSystemName.c_str(),
                    m_slaveID,
                    point->getDataPointName().c_str() );

				// activate ABE process for this datapoint
				point->startABETimeout( newWrite.getSessionID() );
			}
			else
			{
				LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
                    "Failed to send command to %s, slave ID = %d, for %s",
                    m_stationSystemName.c_str(),
                    m_slaveID,
                    point->getDataPointName().c_str() );

				// update control status of the data point to indicate that
				// the output command has failed
				point->updateControlState ( TA_Base_Bus::comms_failed, isSystemSession ); //wenching++ (TD13999) //limin++, SystemSession should not access db
			}
		}
		else
		{
			// update control status of the data point to indicate that
			// the output command has failed because of faulty value
			point->updateControlState ( TA_Base_Bus::comms_failed, isSystemSession ); //wenching++ (TD13999) //limin++, SystemSession should not access db
			LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
                "Failed to send the control data to device.  Check the validity of the value..." );
		}

		point = 0;
	}


	bool StationSystemOutputWriter::addDataPoint( const TA_Base_Bus::DataPoint * dp )
	{
        TA_ASSERT( 0 != dp, "dp == NULL in StationSystemOutputWriter::addDataPoint." );
        TA_ASSERT( true == dp->isWriteable(), "StationSystemOutputWriter is only interested in writeable (output) datapoint." );
        
        if ( false == m_addressBlock.isInRange( dp->getDataPointRegisterAddress() ) )
        {
            return false;
        }

        bool dpAdded = false;
        
        if ( true == isDpStationSystemNameMatched( dp->getDataPointName() ))
        {
            // if the station sub system name of this output worker thread is not empty,
            // i.e this worker is for SWC sub systems
            // save the data point to internal list for further references
            m_dataPoints[ dp->getEntityKey() ] = dp;
            
            dpAdded = true;
        }
        
        return dpAdded;
    }
    

    void StationSystemOutputWriter::removeDataPoint( const TA_Base_Bus::DataPoint * dp )
	{
        TA_ASSERT( 0 != dp, "dp == NULL" );

        RTUOutputDataPointMap::iterator itr = m_dataPoints.find( dp->getEntityKey() );

        if ( itr != m_dataPoints.end() )
        {
            m_dataPoints.erase( itr );
		}
	}
    
    
    void StationSystemOutputWriter::setInServiceState( bool inService )
	{
        TA_Base_Core::ThreadGuard guard(m_modbusLock);

        // prevent setting the same state
        if (m_inService == inService)
        {
            return;
        }

		m_inService = inService;

		// if the worker thread is IN SERVICE
		/*
		if ( true == inService )
		{
			// create modbus connection if not done so
			TA_ASSERT( NULL == m_currentModbusConnection, "m_currentModbusConnection is not NULL in StationSystemOutputWriter::setInServiceState" );

            m_currentModbusConnection = m_rtu.createModbusConnection( m_rtuServicePortNumber, m_slaveID, CONNTYPE_COMMAND );
		}
		else
		{
			m_rtu.destroyModbusConnection( m_currentModbusConnection );
		}
		*/
	}


	bool StationSystemOutputWriter::sendCommand ( const TA_Base_Bus::DataBlock < WORD > & commandValues )
	{
		bool raiseAlarm = false;
		bool operationStatus = true;
		std::string additionalAlarmComment;

        //TA_Base_Core::ThreadReadGuard guard(m_modbusLock);
        TA_Base_Core::ThreadGuard guard(m_modbusLock);

		if ( true == m_inService )
		{
			// create modbus connection if not done so
			TA_ASSERT( NULL == m_currentModbusConnection, "m_currentModbusConnection is not NULL in StationSystemOutputWriter::setInServiceState" );

            m_currentModbusConnection = m_rtu.createModbusConnection( m_rtuServicePortNumber, m_slaveID, CONNTYPE_COMMAND );
		}
		
		// if modbus connection is available
		if ( NULL != m_currentModbusConnection )
		{
		    try
		    {
		        // enable the Modbus Connection for writting command
                m_currentModbusConnection->enable();

				// write the command
				m_currentModbusConnection->writeMultipleRegisters ( commandValues );
            }
		    catch ( const TA_Base_Bus::ModbusException & me )
		    {
                additionalAlarmComment = me.what();

			    // set the flag to indicate the operation has failed
			    raiseAlarm = true;
			    operationStatus = false;
		    }
		    catch ( ... )
		    {
                // set the flag to indicate the operation has failed
			    raiseAlarm = true;
			    operationStatus = false;
		    }

            // disable the Modbus Connection
            m_currentModbusConnection->disable();
			m_rtu.destroyModbusConnection( m_currentModbusConnection );
			m_currentModbusConnection = NULL;
        }
        else
        {
            // set the flag to indicate the operation has failed
            additionalAlarmComment = "TCP Modbus connection is not available";
            raiseAlarm = true;
			operationStatus = false;
		}

        // raise/close comms alarm if neccessary
		m_rtu.updateCommsAlarm( raiseAlarm, m_rtuServicePortNumber, additionalAlarmComment );

		return operationStatus;
	}


	bool StationSystemOutputWriter::sendCommand( const TA_Base_Bus::DataBlock < bool > & commandValues )
	{
		bool raiseAlarm = false;
		bool operationStatus = true;
		std::string additionalAlarmComment;

        //TA_Base_Core::ThreadReadGuard guard(m_modbusLock);
        TA_Base_Core::ThreadGuard guard(m_modbusLock);

		if ( true == m_inService )
		{
			// create modbus connection if not done so
			TA_ASSERT( NULL == m_currentModbusConnection, "m_currentModbusConnection is not NULL in StationSystemOutputWriter::setInServiceState" );

            m_currentModbusConnection = m_rtu.createModbusConnection( m_rtuServicePortNumber, m_slaveID, CONNTYPE_COMMAND );
		}

		// if modbus connection is available
		if ( NULL != m_currentModbusConnection )
		{
		    try
		    {
                // enable the Modbus Connection for writting command
		        m_currentModbusConnection->enable();

                // write the command
				m_currentModbusConnection->writeMultipleCoils( commandValues );
			}
		    catch( const TA_Base_Bus::ModbusException & me )
		    {
                additionalAlarmComment = me.what();

			    // set the flag to indicate the operation has failed
			    raiseAlarm = true;
			    operationStatus = false;
		    }
		    catch( ... )
		    {
                // set the flag to indicate the operation has failed
			    raiseAlarm = true;
			    operationStatus = false;
		    }

            // disable the Modbus Connection
            m_currentModbusConnection->disable();
			m_rtu.destroyModbusConnection( m_currentModbusConnection );
			m_currentModbusConnection = NULL;
        }
        else
		{
			// set the flag to indicate the operation has failed
			additionalAlarmComment = "TCP Modbus connection is not available";
			raiseAlarm = true;
			operationStatus = false;
		}

		// raise/close comms alarm if neccessary
		m_rtu.updateCommsAlarm( raiseAlarm, m_rtuServicePortNumber, additionalAlarmComment );

		return operationStatus;
	}


    bool StationSystemOutputWriter::isDpStationSystemNameMatched( const std::string & dpName )
    {
        // find the station/system/subsystem/equip configuration from the member
        // m_stationSystemName, minus the "_CONFIGURATION"
        std::string configuredSystem;
        std::string::size_type pos = m_stationSystemName.find ( "_CONFIGURATION", 0 );
        if ( pos != std::string::npos )
        {
            configuredSystem = m_stationSystemName.substr ( 0, pos );
            
            // check if the data point's name has the configured system/subsystem/equip name
            return ( 0 == configuredSystem.compare( 0, configuredSystem.length(), dpName.substr( 0, configuredSystem.length() ) ) );
        }
        // if station system name doesn't include "_CONFIGURATION", just return true to indicate this check is not need
        else
        {
            return true;
        }
        
    }
    
}
