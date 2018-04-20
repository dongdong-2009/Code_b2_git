/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/4669_T01271350/4669/transactive/app/scada/PMSAgent/src/RTUStatusWorker.h $
  * @author:	HoaVu
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2008/11/28 16:26:01 $
  * Last modified by:	$Author: builder $
  *
  * Description:
  *
  *	The RTUStatusWorker object is responsible for processing PMS RTU status data
  *
  */

#ifndef RTUSTATUSWORKER_H
#define RTUSTATUSWORKER_H

#include <vector>
#include <string>

#include "bus/modbus_comms/src/IModbus.h"

namespace TA_IRS_App
{
	class RTUStatusWorker
	{
	// operations
	public:

		RTUStatusWorker( unsigned long statusOffsetAddress,
                         unsigned long pollingStartAddress,
                         unsigned long pollingEndAddress,
                         int slaveID );

		virtual ~RTUStatusWorker();

		//
		void processRTUDataBlock( const TA_Base_Bus::DataBlock< WORD > & addressBlock, const bool isCommsOK );

		//
		// operation specific to RTUStatusWorker
		//


		/**
		  * getIsCommsWithStationSystemsValid
		  *
		  * Check for comms error between the RTU and the specified external system.
		  * Return true if comms is OK otherwise false is returned
		  *
		  */
		bool getIsCommsWithStationSystemsValid ( int systemID ) const;


        //
		// operations to monitor RTU status
		//

		bool getIsPrimaryRTU() const;
		bool getIsRTUSwitching() const;
		bool getIsDownloadRequired() const; //TD10586
		int getSWCWeightage() const;

        bool getIsCommsOK() const;
        
	private:

        // disabled constructors
		RTUStatusWorker();
		RTUStatusWorker( const RTUStatusWorker & obj );
		RTUStatusWorker & operator= ( const RTUStatusWorker & rhs );

		/**
		  * processRTUStatus
		  *
		  * Process the data stored in the address block
		  */
		void processRTUStatus ( const TA_Base_Bus::DataBlock< WORD > & addressBlock );

		const std::string & getRTUConfigFileVersion() const;
		const std::string & getRTUConfigFileDownLoadTimeStamp() const;
		const std::string & getRTUSoftwareVersion() const;
		const std::string & getSWCWeightageString() const;

        // ++ TD15293 : AZ, make this a bit simpler
        enum EWordRange
        {
            MIN_WORD = 13,
            MAX_WORD = 31
        };

		bool getIsCommsValidAtWord(int index) const;
        // TD15293 ++

	// attributes
	private:

        int m_slaveID;
        bool m_isCommsOK;
		bool m_firstScanOfData;
		bool m_rtuIsDownloadRequired; //TD10586
		bool m_rtuSwitching;
		int m_SWCWeightage;
		unsigned int m_primaryStatus;
        unsigned long m_startAddress;
        unsigned long m_endAddress;
        unsigned long m_statusOffsetAddress;
		unsigned long m_rtuConfigurationFileVersion;
		unsigned long m_rtuConfigurationFileDownLoadTimestamp;
		unsigned long m_rtuSoftwareVersion;

		//
		// communication with SWC
		//
        // AZ: some unused bool in this array, to make it simple
		volatile bool m_commsWithSubSystemAtWord[MAX_WORD + 1];
	};
}

#endif
