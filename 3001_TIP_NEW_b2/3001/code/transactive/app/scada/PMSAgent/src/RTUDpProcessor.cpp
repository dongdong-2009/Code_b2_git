/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/3001_TIP_NEW/3001/transactive/app/scada/PMSAgent/src/RTUDpProcessor.cpp $
  * @author:	Raymond Pau
  * @version:	$Revision: #8 $
  *
  * Last modification:	$DateTime: 2017/12/08 17:42:23 $
  * Last modified by:	$Author: Ouyang $
  *
  * Description:
  *
  *	The RTUDpProcessor object is responsible for processing input data polled from the PMS RTU
  *
  */

#include <set>
#include <math.h>
#include "time.h"
#include "ace/OS.h"

#include "app/scada/PMSAgent/src/RTUDpProcessor.h"

#include "bus/scada/datapoint_library/src/DataPoint.h"

#include "core/exceptions/src/GenericAgentException.h"
#include "core/alarm/src/AlarmConstants.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;
using TA_Base_Core::GenericAgentException;

namespace TA_IRS_App
{
	const int DEFAULT_ANALOGUE_DP_UPDATE_INTERVAL = 10; //TD16169

	RTUDpProcessor::RTUDpProcessor( const std::string & stationName,
                                    const std::string & stationSystemName,
                                    unsigned long pollingStartAddress,
                                    unsigned long pollingEndAddress,
                                    unsigned long offsetAddress,
									const std::vector<std::pair<unsigned long, unsigned long> >& blockAddresses,
                                    int slaveID,
									int statusWordNumber,
									int analogueDpUpdateInterval)  //TD16169
	:
    m_stationName( stationName ),
	m_stationSystemName( stationSystemName ),
	m_startAddress( pollingStartAddress ),
	m_lastDpAddress( pollingEndAddress ),
    m_offsetAddress( offsetAddress ),
	m_analogueDpUpdateInterval( DEFAULT_ANALOGUE_DP_UPDATE_INTERVAL ), //TD16169
	m_analogueDpUpdateCounter( 0 ),  //TD16169
    m_slaveID( slaveID ),
	m_statusWordNumber(statusWordNumber),
	m_threadTerminated( false ),
	m_updateAllDataPoint( false ),
    m_isStandardTcpModbusDevice( false ),
	m_opertionMode(TA_Base_Core::NotApplicable),
	m_queueSemaphore( 0 ),
	m_blockAddresses( blockAddresses )
	{
        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Constructing RTUDpProcessor for station [%s]:[%s], start/end/offset address [%u/%u/%u]", 
            stationName.c_str(), stationSystemName.c_str(), m_startAddress, m_lastDpAddress, m_offsetAddress); 

		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "stationSystemName:%s, m_slaveID[%d], m_statusWordNumber[%d]", stationSystemName.c_str(), m_slaveID, m_statusWordNumber);
        
		// check runtime parameter ModbusDevice
		std::string modbusDevice = TA_Base_Core::RunParams::getInstance().get ( "ModbusDevice" );

		// if defined as STE RTU device
		if ( "STE" == modbusDevice )
		{
			m_isStandardTcpModbusDevice = false;
		}
		else
		{
			m_isStandardTcpModbusDevice = true;
		}

        //TD16169
		if ( analogueDpUpdateInterval > 0 )
		{
			m_analogueDpUpdateInterval = analogueDpUpdateInterval;
		}
		else
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Sub system %s: set the analogue datapoints update interval to a default value [%d].", 
				m_stationSystemName.c_str(), m_analogueDpUpdateInterval);
		}

		checkBlockAddressRange();  //TD16182

        // start thread
        start();
	}

	RTUDpProcessor::~RTUDpProcessor()
	{
        // terminate thread
        terminateAndWait();
    }


	bool RTUDpProcessor::addDataPoint( TA_Base_Bus::DataPoint * dp )
	{
        TA_ASSERT( NULL != dp, "dp is NULL in RTUModbusTable::addDataPoint" );
        
        bool validAddress = false;
        unsigned long registerAddress = dp->getDataPointRegisterAddress();
        
        if ( ( 0 <= m_startAddress ) && ( 0 < m_lastDpAddress ) )
        {
            if ( ( m_startAddress <= registerAddress ) &&
                ( m_lastDpAddress >= registerAddress ) )
            {
                if ( true == isDpStationSystemNameMatched( dp->getDataPointName() ) )
                {
                    // save the data point to internal list for further references
                    sortAndSaveDataPointToList( dp );
                    validAddress = true;
                    
                    addDataPointMap(dp);
                }
            }
        }
        
        return validAddress;
    }


    // ++ TD15293 : AZ, optimize the updating of DPs by adding map of block address
	// TD16169 : seperate handling for analogue datapoints
	void RTUDpProcessor::addDataPointMap( TA_Base_Bus::DataPoint * dp )
    {
        TA_Base_Core::ThreadGuard guard( m_datapointsLock );

		unsigned long registerAddress = dp->getDataPointRegisterAddress();

        // add to the map
        // should depend on datapoint data type
        if (dp->getIsDigitalDataPoint())
        {
            std::vector< TA_Base_Bus::DataPoint* > &dataPointList = m_blockDPs[registerAddress];
            dataPointList.push_back(dp);
        }
        else if (dp->getIsTextDataPoint())
        {
            //m_blockTextDPs.push_back(dp);
            // add this dp to all the register addresses that it occupied
            for(int i=0; i < dp->getMaxTextLength(); ++i)
            {
                std::vector< TA_Base_Bus::DataPoint* > &dataPointList = m_blockTextDPs[registerAddress+i];
                dataPointList.push_back(dp);
            }
        }
        // others are all analogue datapoints
        else
        {
            std::vector< TA_Base_Bus::DataPoint* > &dataPointList = m_blockAnalogueDPs[registerAddress];
            dataPointList.push_back(dp);
            
            if (use2Registers(dp))
            {
                std::vector< TA_Base_Bus::DataPoint* > &nextDataPointList = m_blockAnalogueDPs[registerAddress + 1];
                nextDataPointList.push_back(dp);
            }
        }
    }
    // TD15293 ++


	// TD16169: Seperate handling for analogue datapoint
    void RTUDpProcessor::removeDataPoint( TA_Base_Bus::DataPoint * dp )
    {
        TA_ASSERT( NULL != dp, "dp is NULL in RTUDpProcessor::removeDataPoint" );
		TA_Base_Core::ThreadGuard guard ( m_datapointsLock );

        unsigned long dataPointKey = dp->getEntityKey();

		TA_Base_Bus::DataPoint * dpRemoved = NULL;
		for ( std::vector< TA_Base_Bus::DataPoint * >::iterator itr = m_datapoints.begin(); itr != m_datapoints.end();/* ++itr*/ )
		{
			if ( dataPointKey == (*itr)->getEntityKey() )
			{
				dpRemoved = (*itr);
				itr = m_datapoints.erase( itr );	// must get return iterator

                // ++ TD15293 : AZ, optimize the updating of DPs
                // remove from map
				unsigned long registerAddress = dpRemoved->getDataPointRegisterAddress();
			
                // base on datapoint data type
                if (dp->getIsDigitalDataPoint())
                {
                    std::vector<TA_Base_Bus::DataPoint*> &blockIt = m_blockDPs[registerAddress];
                    std::vector<TA_Base_Bus::DataPoint*>::iterator it;
                    
                    for (it = blockIt.begin(); it != blockIt.end(); ++it)
                    {
                        if ((*it)->getEntityKey() == dataPointKey)
                        {
                            blockIt.erase(it);	// legal erase as break here
                            break;
                        }
                    }
                }
                else if (dp->getIsTextDataPoint())
                {
                    for(int i=0; i < dpRemoved->getMaxTextLength(); ++i)
                    {
                        std::vector< TA_Base_Bus::DataPoint* > &blockIt = m_blockTextDPs[registerAddress+i];
                        for (std::vector<TA_Base_Bus::DataPoint*>::iterator it = blockIt.begin();
                                    it != blockIt.end(); ++it)
                        {
                            if ((*it)->getEntityKey() == dataPointKey)
                            {
                                blockIt.erase(it);	// legal erase as break here
                                break;
                            }
                        }
                    }
                }
                // others are all analogue datapoints
                else
                {
                    std::vector<TA_Base_Bus::DataPoint*> &blockIt = m_blockAnalogueDPs[registerAddress];
                    std::vector<TA_Base_Bus::DataPoint*>::iterator it;
                    
                    for (it = blockIt.begin(); it != blockIt.end(); ++it)
                    {
                        if ((*it)->getEntityKey() == dataPointKey)
                        {
                            blockIt.erase(it); // legal erase as break here
                            break;
                        }
                    }
                    
                    if(use2Registers(dpRemoved))
                    {
                        std::vector< TA_Base_Bus::DataPoint* > &nextblockIt = m_blockAnalogueDPs[registerAddress + 1];
                        for (it = nextblockIt.begin(); it != nextblockIt.end(); ++it)
                        {
                            if ((*it)->getEntityKey() == dataPointKey)
                            {
                                nextblockIt.erase(it); // legal erase as break here
                                break;
                            }
                        }
                    }
                }
            }
			else
			{
				++itr;
			}
		}

	}


	void RTUDpProcessor::terminate()
	{
		// the thread has been terminated, set the flag
		m_threadTerminated = true;

		// notify the running thread to break the thread loop
		m_queueSemaphore.post();
	}


	void RTUDpProcessor::run()
	{
        ModbusDataBlock dataBlock, prevDataBlock;

		// enter the thread loop
		while ( m_threadTerminated == false )
		{
			m_queueSemaphore.wait();
             
			bool queueEmpty = true;
			{
				TA_Base_Core::ThreadGuard guard( m_dataBlockLock );
				queueEmpty = m_dataBlockQueue.empty();
			}
			
            //TD15792, move the first logic into updateDataPoint() function.
			while (false == m_threadTerminated && false == queueEmpty)
            {
                // ++ TD15293 : AZ, optimize the updating of DPs
                {
                    TA_Base_Core::ThreadGuard guard( m_dataBlockLock );
					if ( false == m_dataBlockQueue.empty() )
					{
						dataBlock = m_dataBlockQueue.front();
						m_dataBlockQueue.pop();
					}
					else
					{
						break;
					}
                }
                
                int lCount;
                lCount = updateDataPoints( dataBlock, prevDataBlock );
				
				{
					TA_Base_Core::ThreadGuard guard( m_updateAllDataPointLock );
					if (m_updateAllDataPoint)
					{
						prevDataBlock.quality = TA_Base_Bus::QUALITY_BAD_NO_SPECIFIC_REASON;
						m_updateAllDataPoint = false;
					}
					else
					{
						prevDataBlock = dataBlock;
					}
				}	
            }
        }

		// exit thread loop --> reset the flag
		m_threadTerminated = false;
	}


	void RTUDpProcessor::processRTUDataBlock( const TA_Base_Bus::DataBlock< WORD > & addressBlock, TA_Base_Bus::EQualityStatus newStatus )
	{
        ModbusDataBlock dataBlock;
        dataBlock.quality = newStatus;
        dataBlock.timestamp = extractTimeStamp(m_startAddress - m_offsetAddress, addressBlock);

        // to process data block with station system offset address
        // so can process rtu status block as the same as other station systems
        int length = ( m_lastDpAddress - m_startAddress + 1 );
        dataBlock.data.setStartAndLength( m_startAddress , length );
        
        // copy the data from phicial table to logical table
        for ( unsigned long loc = m_startAddress; loc <= m_lastDpAddress; ++ loc )
        {
            dataBlock.data.set( loc, addressBlock[loc - m_offsetAddress] );
        }

		//wenching
		std::vector<std::pair<unsigned long, unsigned long> >::iterator blockAddressIt;

		for (blockAddressIt=m_blockAddresses.begin(); blockAddressIt!=m_blockAddresses.end(); ++blockAddressIt)
		{
			dataBlock.blockTimestamp[(*blockAddressIt).first] = extractTimeStamp((*blockAddressIt).first- m_offsetAddress, addressBlock);
		}
		
        {
            TA_Base_Core::ThreadGuard guard( m_dataBlockLock );
            m_dataBlockQueue.push(dataBlock);
		}

		// notify the running thread to process the modbus table queue
		m_queueSemaphore.post();
	}

	//TD16169: update analogue datapoint update interval
	void RTUDpProcessor::updateAnalogueDpUpdateInterval(int analogueDpUpdateInterval)
	{
		if ( analogueDpUpdateInterval > 0 )
		{
			m_analogueDpUpdateInterval = analogueDpUpdateInterval;
		}
		else
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Station System %s: New analogue data point update interval (%d) not in valid range.", m_stationSystemName.c_str(), analogueDpUpdateInterval );
		}
	}

	
    // ++ TD15293 : AZ, optimize the updating of DPs
	int RTUDpProcessor::populateDataPoints( const ModbusDataBlock & dataBlock )
	{
		TA_Base_Core::ThreadGuard guard( m_datapointsLock );

        int updateCount = 0;

		// update the DataPoint objects with the latest values in the modbus table
        for ( std::vector< TA_Base_Bus::DataPoint* >::iterator dpItr = m_datapoints.begin(); dpItr != m_datapoints.end(); ++dpItr )
		{
            processDataPoint(*dpItr, dataBlock);
            ++updateCount;
		}

        return updateCount;
	}

	//TD16169 : Update analogue datapoints according to the interval set
    int RTUDpProcessor::updateDataPoints( const ModbusDataBlock & dataBlock, const ModbusDataBlock & prevDataBlock )
    {
        int updateCount = 0;

		++m_analogueDpUpdateCounter;

        // should update all DPs when the quality has changed
        if ( dataBlock.quality != prevDataBlock.quality)
        {
            updateCount = populateDataPoints( dataBlock );

			LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "xx--Station system %s: finish updating all %d datapoints",
				m_stationSystemName.c_str(), updateCount);
			return updateCount;
        }

        // to keep track of which DPs have been processed
        std::set<TA_Base_Bus::DataPoint*> processedAIDPs;
        std::set<TA_Base_Bus::DataPoint*> processedTextDPs;
		bool updateAnalogueDps = (0 == m_analogueDpUpdateCounter % m_analogueDpUpdateInterval);

	    TA_Base_Core::ThreadGuard guard( m_datapointsLock );

        std::vector<TA_Base_Bus::DataPoint*>::iterator dpItr;

        unsigned long startAddress = std::max(m_startAddress, (unsigned long)dataBlock.data.start());
        unsigned long endAddress = std::min(m_lastDpAddress, (unsigned long)dataBlock.data.end());

        for (unsigned long i = startAddress; i <= endAddress; ++i)
        {
            if (dataBlock.data[i] == prevDataBlock.data[i])
			{
				// No changes; continue to check next DataBlock
				continue;
			}

            {
			// update dps which are not analogue or text dps
			BlockDpIt blockIt = m_blockDPs.find(i);
			if (blockIt != m_blockDPs.end())
			{
				for (dpItr = blockIt->second.begin(); dpItr != blockIt->second.end(); ++dpItr)
				{
					++updateCount;
					processDataPoint(*dpItr, dataBlock);
				}	
			}
            }

			if ( false == updateAnalogueDps )
			{
				// update analogue dps
				BlockDpIt blockIt = m_blockAnalogueDPs.find(i);
				if (blockIt != m_blockAnalogueDPs.end())
				{
					for (dpItr = blockIt->second.begin(); dpItr != blockIt->second.end(); ++dpItr)
					{
						// special case for those require more than 1 register, avoid doing this twice
						if (use2Registers(*dpItr))
						{
							if (processedAIDPs.find(*dpItr) == processedAIDPs.end())
							{
								++updateCount;
								processDataPoint(*dpItr, dataBlock);
								processedAIDPs.insert(*dpItr);
							}
						}
						else
						{
							++updateCount;
							processDataPoint(*dpItr, dataBlock);
						}
					}
				}
			}

            {
                // udpate text datapoints
                BlockDpIt blockIt = m_blockTextDPs.find(i);
                if (blockIt != m_blockTextDPs.end())
                {
				    for (dpItr = blockIt->second.begin(); dpItr != blockIt->second.end(); ++dpItr)
				    {
                        if (processedTextDPs.find(*dpItr) == processedTextDPs.end())
                        {
					        ++updateCount;
					        processDataPoint(*dpItr, dataBlock);
                            processedTextDPs.insert(*dpItr);
                        }
				    }
                }
            }
        }

		if ( true == updateAnalogueDps )
		{
			// update analogue dps
			for (BlockDpIt blockIt=m_blockAnalogueDPs.begin(); blockIt !=m_blockAnalogueDPs.end(); ++blockIt)
			{
				for (dpItr = blockIt->second.begin(); dpItr != blockIt->second.end(); ++dpItr)
				{
					++updateCount;
					processDataPoint(*dpItr, dataBlock);
				}
            }
        }

        if (updateCount > 0)
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "xx--Station system %s: finish updating %d datapoints.", m_stationSystemName.c_str(), updateCount);
        }
        
        return updateCount;
    }


    bool RTUDpProcessor::use2Registers(TA_Base_Bus::DataPoint * dp)
    {
        switch (dp->getDataPointDataType())
        {
        case TA_Base_Bus::DPT_IEEE32_TYPE:
		case TA_Base_Bus::DPT_INT32_TYPE:
		case TA_Base_Bus::DPT_INT32S_TYPE:
        case TA_Base_Bus::DPT_UINT32_TYPE:
            return true;
        }

        return false;
    }
    

    void RTUDpProcessor::processDataPoint( TA_Base_Bus::DataPoint * dp, const ModbusDataBlock & dataBlock )
    {
        switch ( dp->getDataPointDataType() )
        {
        // if the data point is an IEEE 32bit floating point number
        case TA_Base_Bus::DPT_IEEE32_TYPE:
            processIEEE32DataPoint( dp, dataBlock );
            break;
            
        // if the data point is a 32bit integer number, where the signed value is represented
		// in 2's Complement format
		case TA_Base_Bus::DPT_INT32_TYPE:
            processINT32DataPoint( dp, dataBlock );
            break;
            
        // if the data point is a 32bit integer number, where the signed value is represented
		// in the most significant bit
		case TA_Base_Bus::DPT_INT32S_TYPE:
            processINT32SDataPoint( dp, dataBlock );
			break;
        
        // if the data point is a 32bit unsigned integer number
        case TA_Base_Bus::DPT_UINT32_TYPE:
            processUINT32DataPoint( dp, dataBlock );
			break;
        
        // if the data point is a 16bit integer number, where the signed value is represented
		// in 2's Complement format
		case TA_Base_Bus::DPT_INT16_TYPE:
            processINT16DataPoint( dp, dataBlock );
            break;

        // if the data point is a 16bit integer number, where the signed value is represented
        // in the most significant bit
        case TA_Base_Bus::DPT_INT16S_TYPE:
            processINT16SDataPoint( dp, dataBlock );
            break;
            
        // if the data point is representing a 16bit unsigned integer number or an enum
        case TA_Base_Bus::DPT_UINT16_TYPE:
            processUINT16DataPoint( dp, dataBlock );
			break;

        // if the data point is a digital input
        case TA_Base_Bus::DPT_BOOL_TYPE:
            processDigitalDataPoint( dp, dataBlock );
            break;
        
        // if the data point is a text data point
		case TA_Base_Bus::DPT_TEXT_TYPE:
            processTextDataPoint( dp, dataBlock );
            break;

		// if the data point is a Enum (16bit integer number), where the signed value is represented
		// in 2's Complement format
		case TA_Base_Bus::DPT_ENUM_TYPE:
        
            processEnumDataPoint( dp, dataBlock );
            break;
            
        default:
            LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Unable to process datapoint (%s) with unknown DataPointDataType (%d)",
                dp->getDataPointName().c_str(), dp->getDataPointDataType() );
            break;
        }
    }
    // TD15293 ++


	bool RTUDpProcessor::getBooleanValue( int bitMask, WORD registerValue )
	{
		return ( 0 != ( bitMask & registerValue ) );
	}


	void RTUDpProcessor::processIEEE32DataPoint( TA_Base_Bus::DataPoint * dp, const ModbusDataBlock & dataBlock )
	{
		unsigned long registerAddress = dp->getDataPointRegisterAddress();

		timeb currentTimeStamp = getTimeStamp(registerAddress, dataBlock); //TD16182
		// get the first register associated with the data point's register address
		WORD registerValue1 = dataBlock.data[ registerAddress ];

		// get the second register associated with the data point's register address,
		// make sure it is still within the address block
		registerAddress += 1;
		if ( ( registerAddress >= (unsigned long) dataBlock.data.start() ) && 
			 ( registerAddress <= (unsigned long) dataBlock.data.end() ) )
		{
			WORD registerValue2 = dataBlock.data[ registerAddress ];

			// TD# 3603: swap the high and low order 16-bit words
			// use the number conversion utility, extract the IEEE 32bit floating point number
			// stored in the registers
            double IEEE_Value = 0.0;
			if ( true == dp->getWordSwapIndication() )
			{
				IEEE_Value = m_numberConversionUtil.convertFromIEEE32_Value( registerValue2, registerValue1 );
			}
			else
			{
				IEEE_Value = m_numberConversionUtil.convertFromIEEE32_Value( registerValue1, registerValue2 );
			}

			// set up DataPointState with the specified timestamp, quality and value
            TA_Base_Bus::DpValue dpValue( dp->getDataPointDataType(), dp->getEnumLabelsMap(), dp->getBooleanLabels() );
			dpValue.setFloat( IEEE_Value, dp->getPrecision() );
			TA_Base_Bus::DataPointState newState( dpValue, currentTimeStamp, dataBlock.quality );

			// update the datapoint
			dp->updateFieldState( newState );
		}
	}


	void RTUDpProcessor::processINT32DataPoint( TA_Base_Bus::DataPoint * dp, const ModbusDataBlock & dataBlock )
	{
		unsigned long registerAddress = dp->getDataPointRegisterAddress();
		timeb currentTimeStamp = getTimeStamp(registerAddress, dataBlock);  //TD16182

		// get the first register associated with the data point's register address
		WORD registerValue1 = dataBlock.data[ registerAddress ];

		// get the second register associated with the data point's register address,
		// make sure it is still within the address block
		registerAddress += 1;
		if ( registerAddress >= (unsigned long) dataBlock.data.start() && 
			 registerAddress <= (unsigned long) dataBlock.data.end() )
		{
			WORD registerValue2 = dataBlock.data[ registerAddress ];

			// TD# 3603: swap the high and low order 16-bit words
			// use the number conversion utility, extract the 32bit integer number
			// stored in the registers (signed value represented in 2's Complement format)
            double int32Value = 0.0;
			if ( true == dp->getWordSwapIndication() )
			{
				int32Value = m_numberConversionUtil.convertFromINT32_Value( registerValue2, registerValue1 );
			}
			else
			{
				int32Value = m_numberConversionUtil.convertFromINT32_Value( registerValue1, registerValue2 );
			}

			// set up DataPointState with the specified timestamp, quality and value
            TA_Base_Bus::DpValue dpValue( dp->getDataPointDataType(), dp->getEnumLabelsMap(), dp->getBooleanLabels() );
			dpValue.setFloat( int32Value, dp->getPrecision() );
			TA_Base_Bus::DataPointState newState( dpValue, currentTimeStamp, dataBlock.quality );
			
			// update the datapoint
            dp->updateFieldState( newState );
		}
	}


	void RTUDpProcessor::processINT32SDataPoint( TA_Base_Bus::DataPoint * dp, const ModbusDataBlock & dataBlock )
	{
		unsigned long registerAddress = dp->getDataPointRegisterAddress();

		timeb currentTimeStamp = getTimeStamp(registerAddress, dataBlock);  //TD16182

		// get the first register associated with the data point's register address
		WORD registerValue1 = dataBlock.data[ registerAddress ];

		// get the second register associated with the data point's register address,
		// make sure it is still within the address block
		registerAddress = registerAddress + 1;
		if ( registerAddress >= (unsigned long) dataBlock.data.start() && 
			 registerAddress <= (unsigned long) dataBlock.data.end() )
		{
			WORD registerValue2 = dataBlock.data[ registerAddress ];

			// TD# 3603: swap the high and low order 16-bit words
			// use the number conversion utility, extract the 32bit integer number
			// stored in the registers (signed value represented in the most significant bit)
            double int32SValue = 0.0;
			if ( true == dp->getWordSwapIndication() )
			{
				int32SValue = m_numberConversionUtil.convertFromINT32S_Value( registerValue2, registerValue1 );
			}
			else
			{
				int32SValue = m_numberConversionUtil.convertFromINT32S_Value( registerValue1, registerValue2 );
			}

			// set up DataPointState with the specified timestamp, quality and value
            TA_Base_Bus::DpValue dpValue( dp->getDataPointDataType(), dp->getEnumLabelsMap(), dp->getBooleanLabels() );
			dpValue.setFloat( int32SValue, dp->getPrecision() );
			TA_Base_Bus::DataPointState newState( dpValue, currentTimeStamp, dataBlock.quality );

			// update the datapoint
			dp->updateFieldState( newState );
		}
	}


	void RTUDpProcessor::processUINT32DataPoint( TA_Base_Bus::DataPoint * dp, const ModbusDataBlock & dataBlock )
	{
		unsigned long registerAddress = dp->getDataPointRegisterAddress();

		timeb currentTimeStamp = getTimeStamp(registerAddress, dataBlock);  //TD16182

		// get the first register associated with the data point's register address
		WORD registerValue1 = dataBlock.data[ registerAddress ];

		// get the second register associated with the data point's register address,
		// make sure it is still within the address block
		registerAddress = registerAddress + 1;
		if ( registerAddress >= (unsigned long) dataBlock.data.start() && 
			 registerAddress <= (unsigned long) dataBlock.data.end() )
		{
			WORD registerValue2 = dataBlock.data[ registerAddress ];

			// TD# 3603: swap the high and low order 16-bit words
			// use the number conversion utility, extract the 32bit unsigned integer number
			// stored in the registers
            double uint32Value = 0.0;
			if ( true == dp->getWordSwapIndication() )
			{
				uint32Value = m_numberConversionUtil.convertFromUINT32_Value( registerValue2, registerValue1 );
			}
			else
			{
				uint32Value = m_numberConversionUtil.convertFromUINT32_Value( registerValue1, registerValue2 );
			}

			// set up DataPointState with the specified timestamp, quality and value
            TA_Base_Bus::DpValue dpValue( dp->getDataPointDataType(), dp->getEnumLabelsMap(), dp->getBooleanLabels() );
			dpValue.setFloat( uint32Value, dp->getPrecision() );
            TA_Base_Bus::DataPointState newState( dpValue, currentTimeStamp, dataBlock.quality );

			// update the datapoint
			dp->updateFieldState( newState );
		}
	}


	void RTUDpProcessor::processINT16DataPoint( TA_Base_Bus::DataPoint * dp, const ModbusDataBlock & dataBlock )
	{
		unsigned long registerAddress = dp->getDataPointRegisterAddress();

		timeb currentTimeStamp = getTimeStamp(registerAddress, dataBlock);   //TD16182

		// get the value of the register associated with the data point's register address
		WORD registerValue = dataBlock.data[ registerAddress ];

		// use the number conversion utility, extract the 16bit integer number
		// stored in the registers (signed value represented in 2's Complement format)
		int int16Value = m_numberConversionUtil.convertFromINT16_Value( registerValue );

		// set up DataPointState with the specified timestamp, quality and value
        TA_Base_Bus::DpValue dpValue( dp->getDataPointDataType(), dp->getEnumLabelsMap(), dp->getBooleanLabels() );
		dpValue.setFloat( int16Value, dp->getPrecision() );
		TA_Base_Bus::DataPointState newState( dpValue, currentTimeStamp, dataBlock.quality );

		// update the datapoint
		dp->updateFieldState( newState );
	}


	void RTUDpProcessor::processEnumDataPoint( TA_Base_Bus::DataPoint * dp, const ModbusDataBlock & dataBlock )
	{
		unsigned long registerAddress = dp->getDataPointRegisterAddress();

		timeb currentTimeStamp = getTimeStamp(registerAddress, dataBlock);  //TD16182

		// get the value of the register associated with the data point's register address
		WORD registerValue = dataBlock.data[ registerAddress ];

		// use the number conversion utility, extract the 16bit integer number
		// stored in the registers (signed value represented in 2's Complement format)
		int int16Value = m_numberConversionUtil.convertFromINT16_Value( registerValue );

		// set up DataPointState with the specified timestamp, quality and value
		TA_Base_Bus::DpValue dpValue( dp->getDataPointDataType(), dp->getEnumLabelsMap(), dp->getBooleanLabels() );
        dpValue.setEnum( (unsigned long)int16Value );
		TA_Base_Bus::DataPointState newState ( dpValue, currentTimeStamp, dataBlock.quality );

		// update the datapoint
		dp->updateFieldState( newState );
	}

	void RTUDpProcessor::processINT16SDataPoint( TA_Base_Bus::DataPoint * dp, const ModbusDataBlock & dataBlock )
	{
		unsigned long registerAddress = dp->getDataPointRegisterAddress();

		timeb currentTimeStamp = getTimeStamp(registerAddress, dataBlock);  //TD16182

		// get the value of the register associated with the data point's register address
		WORD registerValue = dataBlock.data[ registerAddress ];

		// use the number conversion utility, extract the 16bit integer number
		// stored in the registers (signed value represented in the most significant bit)
		int int16SValue = m_numberConversionUtil.convertFromINT16S_Value ( registerValue );

		// set up DataPointState with the specified timestamp, quality and value
        TA_Base_Bus::DpValue dpValue( dp->getDataPointDataType(), dp->getEnumLabelsMap(), dp->getBooleanLabels() );
		dpValue.setFloat( int16SValue, dp->getPrecision() );
		TA_Base_Bus::DataPointState newState( dpValue, currentTimeStamp, dataBlock.quality );

		// update the datapoint itself
		dp->updateFieldState( newState );
	}


	void RTUDpProcessor::processUINT16DataPoint( TA_Base_Bus::DataPoint * dp, const ModbusDataBlock & dataBlock )
	{
		unsigned long registerAddress = dp->getDataPointRegisterAddress();

		timeb currentTimeStamp = getTimeStamp(registerAddress, dataBlock);  //TD16182

		// get the first register associated with the data point's register address
		WORD registerValue = dataBlock.data[ registerAddress ];

		// use the number conversion utility, extract the 16bit unsigned integer number
		// stored in the register
		int uint16Value = m_numberConversionUtil.convertFromUINT16_Value ( registerValue );

		// set up DataPointState with the specified timestamp, quality and value
		TA_Base_Bus::DpValue dpValue( dp->getDataPointDataType(), dp->getEnumLabelsMap(), dp->getBooleanLabels() );
        dpValue.setFloat( uint16Value, dp->getPrecision() );
		TA_Base_Bus::DataPointState newState ( dpValue, currentTimeStamp, dataBlock.quality );

		// update the datapoint itself
		dp->updateFieldState( newState );
	}


	void RTUDpProcessor::processDigitalDataPoint( TA_Base_Bus::DataPoint * dp, const ModbusDataBlock & dataBlock )
	{
		unsigned long registerAddress = dp->getDataPointRegisterAddress();

		timeb currentTimeStamp = getTimeStamp(registerAddress, dataBlock);  //TD16182

		// get the value of the register associated with the data point's register address
		WORD registerValue = dataBlock.data[ registerAddress ];

		// get the bit address of the digital data point
		int bitAddress = dp->getDataPointBitAddress();

		// if a standard modbus, reverse the bit order in the register, i.e 0->15, 15->0,....
		if ( m_isStandardTcpModbusDevice )
		{
			bitAddress = 15 - bitAddress;
		}

		WORD bitmask = 0x0001 << bitAddress;

		// set up DataPointState with the specified timestamp, quality and value
		TA_Base_Bus::DpValue dpValue( dp->getDataPointDataType(), dp->getEnumLabelsMap(), dp->getBooleanLabels() );
        dpValue.setBoolean( getBooleanValue( bitmask, registerValue ) );
        TA_Base_Bus::DataPointState newState( dpValue, currentTimeStamp, dataBlock.quality );

		// update the datapoint
		dp->updateFieldState( newState );
	}


	void RTUDpProcessor::processTextDataPoint( TA_Base_Bus::DataPoint * dp, const ModbusDataBlock & dataBlock )
	{
		TA_Base_Bus::DpValue dpValue( dp->getDataPointDataType(), dp->getEnumLabelsMap(), dp->getBooleanLabels() );

		// get the size of the text data point, i.e number of chars/bytes
		int maxNumberOfBytes = dp->getMaxTextLength();

		// only proceed if the length is > 0
		if ( 0 >= maxNumberOfBytes )
		{
            LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, "Not processing text datapoint %s; MaxTextLength = %d",
                dp->getDataPointName().c_str(), maxNumberOfBytes );
            return;
        }

        unsigned long registerAddress = dp->getDataPointRegisterAddress();

		timeb currentTimeStamp = getTimeStamp(registerAddress, dataBlock);  //TD16182

        std::string::size_type pos = std::string::npos;

        // special case: RTU config version tiiRTU-ConfigVersion
        pos = dp->getDataPointName().find( "tiiRTU-ConfigVersion" );
        if ( pos != std::string::npos )
        {
            // get RTU Config File version: word 9
            //TD15823, according to Dai Moi's comments, the version string should be:
            // [major version].[version].[index].[release]
            // bits 12-15: major version, if 0, need not display
            // bits 8-11: version number
            // bits 4-7: index number
            // bits 0-3: release number
			WORD registerValue = dataBlock.data[ registerAddress ];
            std::ostringstream ossValue;
            
            if ((( 0xF000 & registerValue ) >> 12) > 0)
            {
                // bits 12 - 15: major version number
                ossValue << ( ( 0xF000 & registerValue ) >> 12 ) << ".";
            }
            
            // bits 8 - 11: version number
            ossValue << ( ( 0x0F00 & registerValue ) >> 8 ) << ".";
            
            // bits 4 - 7: index number
            ossValue << ( ( 0x00F0 & registerValue ) >> 4 ) << ".";
            
            // bits 0 - 3: release number
            ossValue << ( 0x000F & registerValue );
            
            dpValue.setText ( ossValue.str() );
            
            TA_Base_Bus::DataPointState newState ( dpValue, currentTimeStamp, dataBlock.quality );  //TD16182
            dp->updateFieldState( newState );

            return;
        }

        // special case: RTU config down timestamp tiiRTU-ConfigDwnldTime
        pos = dp->getDataPointName().find( "tiiRTU-ConfigDwnldTime" );
        if ( pos != std::string::npos )
        {
            // get RTU Config File download timestamp: 2 words <word 10 (H) and 11 (L)>
            unsigned long timestamp;
            timestamp = dataBlock.data[ registerAddress ];
            timestamp <<= 16;
            timestamp |= dataBlock.data[ registerAddress + 1 ];
            
            //
            // convert RTU config file down load time stamp to string
            //
            time_t ltime = timestamp;
            struct tm tmstruct;
            ACE_OS::localtime_r(&ltime, &tmstruct);
            char str[30] = {0};
            sprintf(str, "%02d/%02d/%04d %02d:%02d:%02d", tmstruct.tm_mday, tmstruct.tm_mon + 1,
                tmstruct.tm_year + 1900, tmstruct.tm_hour, tmstruct.tm_min, tmstruct.tm_sec);

            dpValue.setText ( str );
            
            TA_Base_Bus::DataPointState newState ( dpValue, currentTimeStamp, dataBlock.quality );  //TD16182
            dp->updateFieldState( newState );

            return;
        }

        // special case: RTU config software version tiiRTU-SoftwareVersion
        pos = dp->getDataPointName().find( "tiiRTU-SoftwareVersion" );
        if ( pos != std::string::npos )
        {
            // get RTU Software version: word 12
            // bits 0 - 3: Bug/Fault fixed
            // bits 4 - 7: Track Changes Request/Customised
            // bits 8 - 11: Release Version/New Feature
            // bits 12 - 15: 0x0D for Demo Release, 0x0E for Engineering Release, otherwise release number
			WORD registerValue = dataBlock.data[ registerAddress ];
            std::ostringstream ossValue;

            if ( 0x0D == ( ( 0xF000 & ( registerValue ) ) >> 12 ) )
            {
                ossValue << "D.";
            }
            else if ( 0x0E == ( ( 0xF000 & ( registerValue ) ) >> 12 ) )
            {
                ossValue << "E.";
            }
            else
            {
                ossValue << ( ( 0xF000 & ( registerValue ) ) >> 12 ) << ".";
            }
            
            // bits 8-11
            ossValue << ( ( 0x0F00 & ( registerValue ) ) >> 8 ) << ".";
            
            // bits 4-7
            ossValue << ( ( 0x00F0 & ( registerValue ) ) >> 4 ) << ".";
            
            // bits 0-3
            ossValue << ( 0x000F & ( registerValue ) );
            
            dpValue.setText ( ossValue.str() );
            
            TA_Base_Bus::DataPointState newState ( dpValue, currentTimeStamp, dataBlock.quality );  //TD16182
            dp->updateFieldState( newState );

            return;
        }


		// special case: PLC Version Number and PLC Config Type
		//

		// if the specified data point representing BMF data point, PLCA-ConfigType
        pos = dp->getDataPointName().find( "tiiPLCA-ConfigType" );
		if ( pos != std::string::npos ) //TD15153
		{
            std::ostringstream ossValue;
            char * configType[15] = {
                "1A",	// 0000
                "1B",	// 0001
			    "1C",	// 0010
			    "1D",	// 0011
			    "1E",	// 0100
			    "1F",	// 0101
			    "1G",	// 0110
    			"1H",	// 0111
			    "1I",	// 1000
			    "1J",	// 1001
			    "1K",	// 1010
			    "1L",	// 1011
			    "1M",	// 1100
			    "1N",	// 1101
			    "1O"	//1110
            };

			//
			// occupy upper word of the register
			//
			WORD registerValue = dataBlock.data[ registerAddress ];

			// bits 12-15: PLC Configuration Type
            int plcConfigType = (0xF000 & registerValue) >> 12;

			if ( (plcConfigType >= 0 ) && (plcConfigType < 15 ) )
			{
				ossValue << configType[ plcConfigType ];

				// bits 10-11: not used

				// bits 8-9: number of MUXes
				ossValue << " with " << ( ( 0x0300 & ( registerValue ) ) >> 8 ) << " MUX(s)";
			}
			else
			{
				ossValue << "Unknown";
			}

			dpValue.setText ( ossValue.str() );

			TA_Base_Bus::DataPointState newState ( dpValue, currentTimeStamp, dataBlock.quality );  //TD16182
			dp->updateFieldState( newState );

			return;
		}

		// if the specified data point representing BMF data point, PLCA-VersionNumber
		pos = dp->getDataPointName().find( "tiiPLCA-VersionNumber" );
		if ( pos != std::string::npos ) //TD15153
		{
			std::ostringstream ossValue;

			//
			// occupy lower word of the register
			//

			WORD registerValue = dataBlock.data[ registerAddress ];

			// bit 7-4: Major version number
			ossValue << ( (0x00F0 & registerValue) >> 4 ) << ".";

			// bits 3-0: Minor version number
			ossValue << ( 0x000F & registerValue );

			dpValue.setText( ossValue.str() );

			TA_Base_Bus::DataPointState newState ( dpValue, currentTimeStamp, dataBlock.quality );   //TD16182
			dp->updateFieldState( newState );

			return;
		}
		// if the specified data point representing BMF data point, PLC-ConfigType
        pos = dp->getDataPointName().find( "tiiPLC-ConfigType" );
		if ( pos != std::string::npos ) //TD15153
		{
			//DTL-1544 Liuy 2013-07-09
			//Change the process method for PLC-ConfigType datapoint.
            std::ostringstream ossValue;
 			//
			// occupy upper word of the register
			//
			WORD registerValue = dataBlock.data[ registerAddress ];

			// bits 8-10: PLC Configuration Type
            int plcConfigType = (0x0700 & registerValue) >> 8;

			if ( (plcConfigType > 0 ) && (plcConfigType < 7 ) )
			{
				// bits 8-10: 
				ossValue << plcConfigType << " RIO node";
			}
			else
			{
				ossValue << "Unknown";
			}

			dpValue.setText ( ossValue.str() );

			TA_Base_Bus::DataPointState newState ( dpValue, currentTimeStamp, dataBlock.quality );  //TD16182
			dp->updateFieldState( newState );

			return;
		}

		// if the specified data point representing BMF data point, PLC-VersionNumber
		pos = dp->getDataPointName().find( "tiiPLC-VersionNumber" );
		if ( pos != std::string::npos ) //TD15153
		{
			std::ostringstream ossValue;

			//
			// occupy lower word of the register
			//

			WORD registerValue = dataBlock.data[ registerAddress ];

			// bit 7-4: Major version number
			ossValue << ( (0x00F0 & registerValue) >> 4 ) << ".";

			// bits 3-0: Minor version number
			ossValue << ( 0x000F & registerValue );

			dpValue.setText( ossValue.str() );

			TA_Base_Bus::DataPointState newState ( dpValue, currentTimeStamp, dataBlock.quality );   //TD16182
			dp->updateFieldState( newState );

			return;
		}

		//
		// if the data point is not one of the above, continue on
		//

		//
		// calculate the total number of words that makes up the
		// size of the text data point
		//
		int numberOfWords = 0;
		div_t divResult = div ( maxNumberOfBytes, 2 );
		numberOfWords = divResult.quot;
		if ( 0 < divResult.rem )
		{
			numberOfWords++;
		}

		//
		// extracting the data from the data block
		//
		std::ostringstream buffer;
		int currentByteIndex = 1;
		bool badQuality = false;
        WORD registerValue = 0;
		for ( int currentWord = 0; currentWord < numberOfWords; ++currentWord )
		{
			// only proceed if the queried address is within the range
			if ( true == dataBlock.data.isInRange( registerAddress + currentWord ) )
			{
				registerValue = dataBlock.data[ registerAddress + currentWord];

				//TD15784, don't swap the high and low bytes.
                //Add extra parameter to indicate if to swap bytes
                bool byteSwap = dp->getByteSwapIndication();
                if (byteSwap)
                {
                    buffer << (char) (0xFF & ( registerValue));	// low order byte
                }
                else
                {
                    buffer << (char) (0xFF & ( registerValue >> 8 ));	// high order byte
                }

                ++currentByteIndex;

				if ( currentByteIndex > maxNumberOfBytes )
				{
					break;
				}

                if (byteSwap)
                {
                    buffer << (char) (0xFF & ( registerValue >> 8 ));	// high order byte
                }
                else
                {
                    buffer << (char) (0xFF & registerValue);    // low order byte
                }

                ++currentByteIndex;

                if ( currentByteIndex > maxNumberOfBytes )
				{
					break;
				}
			}
            else
			{
                // if the address is not within the range of data block
				// set quality flag and then exit loop
				badQuality = true;
				break;
			}
		}

		dpValue.setText( buffer.str() );

		if ( true == badQuality )
		{
			TA_Base_Bus::DataPointState newState( dpValue, currentTimeStamp, TA_Base_Bus::QUALITY_BAD_CONFIGURATION_ERROR );  //TD16182
			dp->updateFieldState( newState );
		}
		else
		{
			TA_Base_Bus::DataPointState newState( dpValue, currentTimeStamp, dataBlock.quality );   //TD16182
			dp->updateFieldState( newState );
		}
	}


	void RTUDpProcessor::sortAndSaveDataPointToList( TA_Base_Bus::DataPoint * dp )
	{
        TA_ASSERT( NULL != dp, "dp is NULL in RTUDpProcessor::sortAndSaveDataPointToList" );

        // stop multi threads accessing the list of DataPoint objects at the same time
        TA_Base_Core::ThreadGuard guard( m_datapointsLock );

		// if the list is empty
		if ( true == m_datapoints.empty() )
		{
			m_datapoints.push_back( dp );
            return;
		}

        //
        // check for low priority first
        //
        
        TA_Base_Bus::DataPoint * lastDp = m_datapoints.back();
        
        TA_ASSERT( NULL != lastDp, "lastDp is NULL in RTUDpProcessor::sortAndSaveDataPointToList" );

        // if the priority level of the specified data point is less/equal the 
        // priority level of the last data point in the list
        if ( dp->getUpdatePriorityLevel() <= lastDp->getUpdatePriorityLevel() )
        {
            // insert the data point to the end of the list
            m_datapoints.push_back( dp );
            return;
        }

        //
        // if got here, the data point must have a high update priority, insert the
        // data point to the begining of the list
        //
        std::vector< TA_Base_Bus::DataPoint * >::iterator dpItr = m_datapoints.begin();

        // if found the first iterator
        if ( m_datapoints.end() != dpItr )
        {
            // insert the specified data point to the list, just before the data point
            // pointed to by the current iterator
            m_datapoints.insert( dpItr, dp );
            return;
        }
        
        // if none of the above 2 conditions have been met,
        // insert the data point to the end of the list anyway
        m_datapoints.push_back( dp );
    }

    
    bool RTUDpProcessor::isDpStationSystemNameMatched( const std::string & dpName )
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
    
    
     //TD16182
	void RTUDpProcessor::checkBlockAddressRange()
	{
        std::vector<std::pair<unsigned long, unsigned long> >::iterator blockAddressIt;

		if ( "" == m_stationSystemName )
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "RTUModbusTable::checkBlockAddressRange(): Station system data not initialised yet, return without checking block Addresses");

			return;
		}

		// Check if block addresses are empty
		if ( "" != m_stationSystemName && true == m_blockAddresses.empty() )
		{
			std::ostringstream desc;
			desc << "Subsystem [" 
				 << m_stationSystemName
				 << "] - block addresses undefined ";

			TA_THROW( GenericAgentException(GenericAgentException::CONFIGURATION_ERROR, desc.str().c_str()) );
		}

		for (blockAddressIt=m_blockAddresses.begin(); blockAddressIt!=m_blockAddresses.end(); ++blockAddressIt)
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "RTUModbusTable[%s]::checkBlockAddressRange(): Sub block range - Start Address = %d, End Address = %d",
				m_stationSystemName.c_str(), (*blockAddressIt).first, (*blockAddressIt).second);
		}
		
		// Check the block address range, throw if it was configured wrongly
		for (blockAddressIt=m_blockAddresses.begin(); blockAddressIt!=m_blockAddresses.end(); ++blockAddressIt)
		{
			// Check if block start address out of polling address range
			if ( (*blockAddressIt).first < m_startAddress || (*blockAddressIt).first > m_lastDpAddress )
			{
				std::ostringstream desc;
				desc << "Subsystem [" 
					 << m_stationSystemName
					 << "] - start address is out of range. "
				     << "Polling start address = "
					 << m_startAddress
					 << " and end address = "
					 << m_lastDpAddress;

				TA_THROW( GenericAgentException(GenericAgentException::CONFIGURATION_ERROR, desc.str().c_str()) );
			}

			// Check if block end address out of polling address range
			if ( (*blockAddressIt).second < m_startAddress || (*blockAddressIt).second > m_lastDpAddress )
			{
				std::ostringstream desc;
				desc << "Subsystem [" 
					 << m_stationSystemName
					 << "] - end address is out of range. "
				     << "Polling start address = "
					 << m_startAddress
					 << " and end address = "
					 << m_lastDpAddress;

				TA_THROW( GenericAgentException(GenericAgentException::CONFIGURATION_ERROR, desc.str().c_str()) );
			}

			// Check if block start address greater than end address
			if ( (*blockAddressIt).second < (*blockAddressIt).first )
			{
				std::ostringstream desc;
				desc << "Subsystem [" 
					 << m_stationSystemName
					 << "] - start address is greater than end address for block with start address = "
				     << (*blockAddressIt).first
					 << " and end address = "
					 << (*blockAddressIt).second;

				TA_THROW( GenericAgentException(GenericAgentException::CONFIGURATION_ERROR, desc.str().c_str()) );
			}
		}

		// Check if there is any overlapped address range, throw if it was incorrect
		//TD17034
		if (false == m_blockAddresses.empty())
		{
			unsigned long prevStartAddress, prevEndAddress;
			blockAddressIt=m_blockAddresses.begin();
			prevStartAddress = (*blockAddressIt).first;
			prevEndAddress = (*blockAddressIt).second;
			++blockAddressIt;

			// Arrange the address range in order
			for (; blockAddressIt!=m_blockAddresses.end(); ++blockAddressIt)
			{
				if ( prevEndAddress >= (*blockAddressIt).first )
				{
					LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "RTUModbusTable[%s]::checkBlockAddressRange(): Block=[%d,%d] with Block=[%d,%d] are overlapped",
						m_stationSystemName.c_str(), prevStartAddress, prevEndAddress,
						(*blockAddressIt).first, (*blockAddressIt).second);

					std::ostringstream desc;
					desc << "Subsystem [" 
						 << m_stationSystemName
						 << "] - blocks address is overlapped.";

					TA_THROW( GenericAgentException(GenericAgentException::CONFIGURATION_ERROR, desc.str().c_str()) );
				}

				prevStartAddress = (*blockAddressIt).first;
				prevEndAddress = (*blockAddressIt).second;
			}
		}
	}

	timeb RTUDpProcessor::getTimeStamp(unsigned long registerAddress, const ModbusDataBlock & dataBlock)
	{
		
		std::map<unsigned long, unsigned long>::iterator dpAddressBlockIt = m_datapointsAddressBlock.find(registerAddress);
		
		if ( dpAddressBlockIt != m_datapointsAddressBlock.end() )
		{
			std::map<unsigned long, timeb>::const_iterator iter=dataBlock.blockTimestamp.find(dpAddressBlockIt->second);

			if( dataBlock.blockTimestamp.end() != iter)
			{
				LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "RTUModbusTable::getTimesStamp()[%s]: Register address - %d, found timestamp in datablock=%d",
					m_stationSystemName.c_str(), registerAddress, iter->second);
				return iter->second;
			}
		}
	
		std::vector<std::pair<unsigned long, unsigned long> >::iterator blockAddressIt;

		for (blockAddressIt=m_blockAddresses.begin(); blockAddressIt!=m_blockAddresses.end(); ++blockAddressIt)
		{
			if ( registerAddress >= (*blockAddressIt).first && registerAddress <= (*blockAddressIt).second )
			{
				std::map<unsigned long, timeb>::const_iterator iter=dataBlock.blockTimestamp.find((*blockAddressIt).first);

				if( dataBlock.blockTimestamp.end() != iter)
				{
					m_datapointsAddressBlock[registerAddress] = (*blockAddressIt).first;

					LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "RTUModbusTable::getTimesStamp()[%s]: Register address - %d, found timestamp in datablock=%d",
						m_stationSystemName.c_str(), registerAddress, iter->second);

					return iter->second;
				}
			}
		}
						
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "RTUModbusTable::getTimeStamp[%s] - default timestamp returned [%d].", 
				m_stationSystemName.c_str(), dataBlock.timestamp);

	
		return TA_Base_Core::AlarmConstants::defaultTime;
		//return dataBlock.timestamp;
	}


     //TD16182
	timeb RTUDpProcessor::extractTimeStamp( unsigned long startTimeStampAddress, const TA_Base_Bus::DataBlock< WORD > & dataBlock )
	{
		timeb timestamp = TA_Base_Core::AlarmConstants::defaultTime;
		
		// RTU Timestamp format: first 2 words = UTC timestamp in secs in since start
		// of the epoch, last word (the 3rd) contains milliseconds.

		// get the UTC timestamp in secs
		unsigned long timestampInSeconds = dataBlock[ startTimeStampAddress ];
		timestampInSeconds <<= 16;
		timestampInSeconds |= dataBlock[ startTimeStampAddress + 1 ];

        // get the msecs.  Only accept value if it is between 0 and 1000 inclusive
        unsigned short timestampInMSeconds = 0;

        if ( dataBlock[ startTimeStampAddress + 2 ] <= 1000 )
		{
            timestampInMSeconds = dataBlock[ startTimeStampAddress + 2 ];
		}

		timestamp.time = timestampInSeconds;
   		timestamp.millitm = timestampInMSeconds;

		// if standard modbus PLC, we timestamp the data
		if ( true == m_isStandardTcpModbusDevice )
		{
			ftime( &timestamp );
		}

		return timestamp;
	}
    
	void RTUDpProcessor::setOperationMode(TA_Base_Core::EOperationMode mode)
	{
		if (mode != TA_Base_Core::Control && m_opertionMode == TA_Base_Core::Control)
		{
			int itemCount = 0;
			{
				TA_Base_Core::ThreadGuard guard( m_dataBlockLock );
				itemCount = m_dataBlockQueue.size();
				while( false == m_dataBlockQueue.empty())
				{
					m_dataBlockQueue.pop();
				}
			}
			
			{
				TA_Base_Core::ThreadGuard guard( m_updateAllDataPointLock );
				m_updateAllDataPoint = true;
			}	

			LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "xx--Change not control mode,clean m_dataBlockQueue size:%d, m_updateAllDataPoint:%d", 
				itemCount, m_updateAllDataPoint);
		}

		m_opertionMode = mode;
	}
}
