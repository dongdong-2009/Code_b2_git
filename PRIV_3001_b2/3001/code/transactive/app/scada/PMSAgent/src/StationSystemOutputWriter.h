/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/PRIV_3001/3001/transactive/app/scada/PMSAgent/src/StationSystemOutputWriter.h $
  * @author:	Raymond Pau
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2018/03/14 13:51:20 $
  * Last modified by:	$Author: lim.cy $
  *
  * Description:
  *
  *	The StationSystemOutputWriter object is responsible for processing write data commands to the PMS RTU
  *
  */

#ifndef STATIONSYSTEMOUTPUTWRITER_H
#define STATIONSYSTEMOUTPUTWRITER_H

#include <map>

#include "bus/modbus_comms/src/DataBlock.h"
#include "bus/scada/datapoint_library/src/NumberConversionUtil.h"

#include "core/sockets/src/SocketOptions.h"
#include "core/synchronisation/src/WriteReadThreadLockable.h"

namespace TA_Base_Bus
{
    class DataPoint;
    class DataPointWriteRequest;
    class IModbus;
	class NumberConversionUtil;
}

namespace TA_IRS_App
{
    // forward declaration
    class RTU;
    typedef std::map < unsigned long, const TA_Base_Bus::DataPoint * > RTUOutputDataPointMap;

	class StationSystemOutputWriter
	{
    public:

		StationSystemOutputWriter ( RTU& rtu,
                                    const std::string & stationSystemName,
                                    const std::string & rtuServicePortNumber,
                                    unsigned long startIOAddress,
                                    unsigned long endIOAddress,
                                    int slaveID,
                                    unsigned long offsetAddress = 0 );

		virtual ~StationSystemOutputWriter();

		/**
		  * addDataPoint
		  *
		  * Add the specified DataPoint object to the list of data points to be
		  * monitored by the worker thread
		  *
		  * @param theDataPoint	the DataPoint object that is monitored by the worker thread
		  *
		  */
		virtual bool addDataPoint( const TA_Base_Bus::DataPoint * dp );

		virtual void removeDataPoint( const TA_Base_Bus::DataPoint * dp );

		/**
		  * setInServiceState
		  *
		  * Set the worker thread in or out of service.  A worker thread that is out of service
		  * would close down the ModbusTcpConnection and not perform any task
		  *
		  * @param inService	Flag to indicate if the worker thread is in service or not
		  *
		  */
		virtual void setInServiceState( bool inService );

		/**
		  * processWriteRequest
		  *
		  * Process the specified DataPoint write request
		  *
		  */
		virtual bool processWriteRequest( TA_Base_Bus::DataPointWriteRequest & newWrite );
		
        /**
		  * sendCommand
		  *
		  * Send the register commands specified in the data block to the device
		  *
		  * @param commandValues	The command and values to be sent to the device
		  *
		  */
		virtual bool sendCommand( const TA_Base_Bus::DataBlock < WORD > & commandValues );

		/**
		  * sendCommand
		  *
		  * Send the coild commands specified in the data block to the device
		  *
		  * @param commandValues	The command and values to be sent to the device
		  *
		  */
		virtual bool sendCommand( const TA_Base_Bus::DataBlock < bool > & commandValues );

        virtual int getSlaveID() const
		{
			return m_slaveID;
		};

    protected:

		/**
		  * writeDigitalDataPoint
		  *
		  * Send the specified write request to a digital DataPoint
		  *
		  */
		void writeDigitalDataPoint ( TA_Base_Bus::DataPointWriteRequest & newWrite );


		/**
		  * writeAnalogueDataPoint
		  *
		  * Send the specified write request to an analogue DataPoint
		  *
		  */
		void writeAnalogueDataPoint ( TA_Base_Bus::DataPointWriteRequest & newWrite );
        bool isDpStationSystemNameMatched( const std::string & dpName );

	private:
        // disabled constructor
		StationSystemOutputWriter();
		StationSystemOutputWriter ( const StationSystemOutputWriter & obj );
		StationSystemOutputWriter & operator= ( const StationSystemOutputWriter & rhs );

    // attributes
	protected:
        RTU & m_rtu;
		std::string							m_stationSystemName;
		unsigned long						m_offsetAddress;
        std::string                         m_rtuServicePortNumber;
        TA_Base_Bus::DataBlock < WORD >     m_addressBlock;		
        int                                 m_slaveID;
        bool                                m_inService;
        TA_Base_Bus::IModbus *              m_currentModbusConnection;        
        
        RTUOutputDataPointMap                   m_dataPoints;
        TA_Base_Core::WriteReadThreadLockable   m_modbusLock;
		TA_Base_Bus::NumberConversionUtil       m_numberConversionUtil;
	};
}

#endif
