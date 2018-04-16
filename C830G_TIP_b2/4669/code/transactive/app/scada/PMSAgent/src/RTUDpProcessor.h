/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/4669_T01271350/4669/transactive/app/scada/PMSAgent/src/RTUDpProcessor.h $
  * @author:	HoaVu
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2008/11/28 16:26:01 $
  * Last modified by:	$Author: builder $
  *
  * Description:
  *
  *	The RTUDpProcessor object is responsible for processing input data polled from the PMS RTU
  *
  */

#ifndef RTU_DP_PROCESSOR_H
#define RTU_DP_PROCESSOR_H

#include <sys/timeb.h>
#include <vector>
#include <queue>
#include <map>

#include "bus/scada/common_library/src/CommonDefs.h"
#include "bus/scada/datapoint_library/src/NumberConversionUtil.h"
#include "bus/modbus_comms/src/DataBlock.h"
#include "bus/modbus_comms/src/IModbus.h"

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/Semaphore.h"
#include "core/threads/src/Thread.h"

namespace TA_Base_Bus
{
	class DataPoint;
	class NumberConversionUtil;
}

namespace TA_IRS_App
{
	typedef struct _ModbusDataBlock {
		TA_Base_Bus::DataBlock < WORD >	data;
        TA_Base_Bus::EQualityStatus quality;
		timeb timestamp;
		std::map<unsigned long, timeb> blockTimestamp; // wenching
	} ModbusDataBlock;

	class RTUDpProcessor : public TA_Base_Core::Thread
	{
	public:

        RTUDpProcessor( const std::string & stationName,
                        const std::string & stationSystemName,
                        unsigned long pollingStartAddress,
                        unsigned long pollingEndAddress,
                        unsigned long offsetAddress,
						const std::vector<std::pair<unsigned long, unsigned long> >& blockAddresses,
                        int slaveID,
						int analogueDpUpdateInterval);  //TD16169

		virtual ~RTUDpProcessor();

		// 
		// operations required by the TA_Core::TA_Thread interface
		//
		virtual void run();
		virtual void terminate();

		//
		// operations specific to RTUDpProcessor
		//

		/**
		  * addDataPoint
		  *
		  * Add the specified TA_Base_Bus::DataPoint object to the list of data point
		  * monitored by the RTUDpProcessor worker thread.
		  *
		  * @param theDataPoint	the TA_Base_Bus::DataPoint object that is monitored
		  *						by the RTUDpProcessor worker thread
		  */
		virtual bool addDataPoint( TA_Base_Bus::DataPoint * dp );


		/**
		  * removeDataPoint
		  *
		  * Remove from the internal list the data point whose primary key matches
		  * with the specified primary key.  Returns the reference to the data point
		  * that has been removed.  Null pointer is returned if there is no
		  * matching
		  *
		  * @param	dataPointKey	The specified primary key of the data point
		  *							to be removed from the table
		  *
		  */
		virtual void removeDataPoint( TA_Base_Bus::DataPoint * dp );


		/**
		  * getSlaveID
		  *
		  * Get the ID of the Station System ID associate with this modbus table
		  *
		  */
        virtual int getSlaveID() const { return m_slaveID; }


		/**
		  * processRTUDataBlock
		  *
		  * Update the address block associated with the RTUDpProcessor with the specified
		  * block
		  *
		  * @param addressBlock	The new address block that is assigned to the RTUDpProcessor
		  *						worker thread
		  * @param newStatus	The quality status for the new data associated with the new block
		  */
        virtual void processRTUDataBlock( const TA_Base_Bus::DataBlock< WORD > & addressBlock, TA_Base_Bus::EQualityStatus newStatus );

		virtual void updateAnalogueDpUpdateInterval(int analogueDpUpdateInterval); //TD16169
		
	protected:

		/**
		  * processIEEE32DataPoint
		  *
		  * Process and update the value of the IEEE 32bit floating typed TA_Bus::TA_Base_Bus::DataPoint entity based on
		  * the info passed in via the parameter list
		  *
		  * @param theDataPoint	The TA_Bus::TA_Base_Bus::DataPoint whose value is to be updated
		  * @param addressBlock	The address block containing the data relevant to the TA_Bus::TA_Base_Bus::DataPoint
		  * @param timestamp	The timestamp of the new value
		  * @param newStatus	The quality status for the new data associated with the new block
		  */
		virtual void processIEEE32DataPoint( TA_Base_Bus::DataPoint * dp, const ModbusDataBlock & dataBlock );

		/**
		  * processINT32DataPoint
		  *
		  * Process and update the value of the 32bit integer typed TA_Bus::TA_Base_Bus::DataPoint entity based on
		  * the info passed in via the parameter list
		  *
		  * @param theDataPoint	The TA_Base_Bus::DataPoint whose value is to be updated
		  * @param addressBlock	The address block containing the data relevant to the TA_Base_Bus::DataPoint
		  * @param timestamp	The timestamp of the new value
		  * @param newStatus	The quality status for the new data associated with the new block
		  */
		virtual void processINT32DataPoint( TA_Base_Bus::DataPoint * dp, const ModbusDataBlock & dataBlock );

		/**
		  * processINT32SDataPoint
		  *
		  * Process and update the value of the 32bit signed integer typed TA_Base_Bus::DataPoint entity based on
		  * the info passed in via the parameter list
		  *
		  * @param theDataPoint	The TA_Base_Bus::DataPoint whose value is to be updated
		  * @param addressBlock	The address block containing the data relevant to the TA_Base_Bus::DataPoint
		  * @param timestamp	The timestamp of the new value
		  * @param newStatus	The quality status for the new data associated with the new block
		  */
		virtual void processINT32SDataPoint( TA_Base_Bus::DataPoint * dp, const ModbusDataBlock & dataBlock );

		/**
		  * processINT16DataPoint
		  *
		  * Process and update the value of the 16bit integer typed TA_Base_Bus::DataPoint entity based on
		  * the info passed in via the parameter list
		  *
		  * @param theDataPoint	The TA_Base_Bus::DataPoint whose value is to be updated
		  * @param addressBlock	The address block containing the data relevant to the TA_Base_Bus::DataPoint
		  * @param timestamp	The timestamp of the new value
		  * @param newStatus	The quality status for the new data associated with the new block
		  */
		virtual void processINT16DataPoint( TA_Base_Bus::DataPoint * dp, const ModbusDataBlock & dataBlock );

		/**
		  * processEnumDataPoint
		  *
		  * Process and update the value of the Enum (16bit integer typed) TA_Base_Bus::DataPoint entity based on
		  * the info passed in via the parameter list
		  *
		  * @param theDataPoint	The TA_Base_Bus::DataPoint whose value is to be updated
		  * @param addressBlock	The address block containing the data relevant to the TA_Base_Bus::DataPoint
		  * @param timestamp	The timestamp of the new value
		  * @param newStatus	The quality status for the new data associated with the new block
		  */
        virtual void processEnumDataPoint( TA_Base_Bus::DataPoint * dp, const ModbusDataBlock & dataBlock );

		/**
		  * processINT16SDataPoint
		  *
		  * Process and update the value of the 16bit signed integer typed TA_Base_Bus::DataPoint entity based on
		  * the info passed in via the parameter list
		  *
		  * @param theDataPoint	The TA_Base_Bus::DataPoint whose value is to be updated
		  * @param addressBlock	The address block containing the data relevant to the TA_Base_Bus::DataPoint
		  * @param timestamp	The timestamp of the new value
		  * @param newStatus	The quality status for the new data associated with the new block
		  */
		virtual void processINT16SDataPoint( TA_Base_Bus::DataPoint * dp, const ModbusDataBlock & dataBlock );

		/**
		  * processUINT16DataPoint
		  *
		  * Process and update the value of the 16bit unsigned integer typed TA_Base_Bus::DataPoint entity based on
		  * the info passed in via the parameter list
		  *
		  * @param theDataPoint	The TA_Base_Bus::DataPoint whose value is to be updated
		  * @param addressBlock	The address block containing the data relevant to the TA_Base_Bus::DataPoint
		  * @param timestamp	The timestamp of the new value
		  * @param newStatus	The quality status for the new data associated with the new block
		  */
		virtual void processUINT16DataPoint( TA_Base_Bus::DataPoint * dp, const ModbusDataBlock & dataBlock );

		/**
		  * processUINT32DataPoint
		  *
		  * Process and update the value of the 32bit unsigned integer typed TA_Base_Bus::DataPoint entity based on
		  * the info passed in via the parameter list
		  *
		  * @param theDataPoint	The TA_Base_Bus::DataPoint whose value is to be updated
		  * @param addressBlock	The address block containing the data relevant to the TA_Base_Bus::DataPoint
		  * @param timestamp	The timestamp of the new value
		  * @param newStatus	The quality status for the new data associated with the new block
		  */
		virtual void processUINT32DataPoint( TA_Base_Bus::DataPoint * dp, const ModbusDataBlock & dataBlock );

		/**
		  * processDigitalDataPoint
		  *
		  * Process and update the value of the Binary/Digital typed TA_Base_Bus::DataPoint entity based on
		  * the info passed in via the parameter list
		  *
		  * @param theDataPoint	The TA_Base_Bus::DataPoint whose value is to be updated
		  * @param addressBlock	The address block containing the data relevant to the TA_Base_Bus::DataPoint
		  * @param timestamp	The timestamp of the new value
		  * @param newStatus	The quality status for the new data associated with the new block
		  */
		virtual void processDigitalDataPoint( TA_Base_Bus::DataPoint * dp, const ModbusDataBlock & dataBlock );

		/**
		  * processTextDataPoint
		  *
		  * Process and update the value of the Analogue typed TA_Base_Bus::DataPoint entity based on
		  * the info passed in via the parameter list
		  *
		  * @param theDataPoint	The TA_Base_Bus::DataPoint whose value is to be updated
		  * @param addressBlock	The address block containing the data relevant to the TA_Base_Bus::DataPoint
		  * @param timestamp	The timestamp of the new value
		  * @param newStatus	The quality status for the new data associated with the new block
		  */
		virtual void processTextDataPoint( TA_Base_Bus::DataPoint * dp, const ModbusDataBlock & dataBlock );

		/**
		  * getBooleanValue
		  *
		  * Return the boolean value of the register value based on the specified bit mask
		  *
		  * @param bitMask			The specified bit mask to ANDGate
		  * @param registerValue	The register value
		  */
		virtual bool getBooleanValue( int bitMask, WORD registerValue );

		virtual void sortAndSaveDataPointToList( TA_Base_Bus::DataPoint * dp );

        //TD16182
		virtual timeb extractTimeStamp( unsigned long startTimeStampAddress, const TA_Base_Bus::DataBlock< WORD > & dataBlock );

		virtual timeb getTimeStamp(unsigned long registerAddress, const ModbusDataBlock & dataBlock);  //TD16182

        // ++ TD15293 : AZ, optimize the updating of DPs
		virtual int populateDataPoints( const ModbusDataBlock & dataBlock );
        int updateDataPoints( const ModbusDataBlock & dataBlock, const ModbusDataBlock & prevDataBlock );
			
        void addDataPointMap(TA_Base_Bus::DataPoint * dp);

    private:

        void processDataPoint( TA_Base_Bus::DataPoint * dp, const ModbusDataBlock & dataBlock );

        bool use2Registers(TA_Base_Bus::DataPoint * dp);

        bool isDpStationSystemNameMatched( const std::string & dpName );
        
        void checkBlockAddressRange(); //TD16182
        
        // Disable Copy constructor and Assignment operator
		RTUDpProcessor( const RTUDpProcessor & obj );
		RTUDpProcessor & operator= ( const RTUDpProcessor & rhs );

        // TD15293 ++

    protected:
		std::string m_stationName;
		std::string m_stationSystemName;
		unsigned long m_startAddress;
		unsigned long m_lastDpAddress;
        unsigned long m_offsetAddress;
		int m_analogueDpUpdateInterval; // TD16169
		unsigned long m_analogueDpUpdateCounter;  //TD16169
		int m_slaveID;
        volatile bool m_threadTerminated;
		volatile bool m_isStandardTcpModbusDevice;
		std::vector< TA_Base_Bus::DataPoint * > m_datapoints;
        std::queue< ModbusDataBlock > m_dataBlockQueue;
		TA_Base_Bus::NumberConversionUtil m_numberConversionUtil;
		TA_Base_Core::ReEntrantThreadLockable   m_datapointsLock;
		TA_Base_Core::ReEntrantThreadLockable	m_dataBlockLock;
		TA_Base_Core::Semaphore m_queueSemaphore;

        // ++ TD15293 : AZ, optimize the updating of DPs
        // map the datapoints to the data block range
        typedef std::map< unsigned long, std::vector< TA_Base_Bus::DataPoint * > >::iterator BlockDpIt;
        std::map< unsigned long, std::vector< TA_Base_Bus::DataPoint * > > m_blockDPs;
		std::map< unsigned long, std::vector< TA_Base_Bus::DataPoint * > > m_blockAnalogueDPs;  //TD16169
        // special consideration for text data points that span multiple words
        //std::vector< TA_Base_Bus::DataPoint * > m_blockTextDPs;
        std::map< unsigned long, std::vector< TA_Base_Bus::DataPoint * > > m_blockTextDPs;
        // TD15293 ++

		//wenching
		std::vector<std::pair<unsigned long, unsigned long> > m_blockAddresses; 
		std::map<unsigned long, unsigned long> m_datapointsAddressBlock;

	};
}

#endif
