/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/FZL1_TIP/FZL1/code/transactive/app/ats/AtsAgent/ModBusProtocolHandler/src/SIGStatusWorker.cpp $
  * @author:	HoaVu
  * @version:	$Revision: #10 $
  *
  * Last modification:	$DateTime: 2016/02/19 18:31:55 $
  * Last modified by:	$Author: dhanshri $
  *
  * Description:
  *
  *	The SIGStatusWorker object is responsible for processing FEP status data
  *
  */

#if defined( WIN32 )
	#pragma warning( disable : 4786 )
#endif // defined( WIN32 )

#include "ace/OS.h"

#include "time.h"

#include "app/ats/AtsAgent/ModBusProtocolHandler/src/SIGStatusWorker.h"
#include "bus/scada/rtu_library/src/RTUStatusTableConstants.h"



#include "bus/modbus_comms/src/DataBlock.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"

#include "core/alarm/src/AlarmConstants.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_IRS_App
{

	SIGStatusWorker::SIGStatusWorker() : m_isOnline(false)
	{
	}


	SIGStatusWorker::~SIGStatusWorker()
	{
	}

	void SIGStatusWorker::processRTUDataBlock( const TA_Base_Bus::DataBlock< WORD > & addressBlock, const bool isCommsOK )
	{
		//
		m_isCommsOK = isCommsOK;
		if (false == m_isCommsOK)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "SIG status word and address block would be invalid due to Comms Err with FEP, m_isCommsOK= %d, previous value = %d, curr value = 0", isCommsOK, m_isOnline);
			m_isOnline = false;		
			return; 
		}

		/** Rail Transit Line 1 project in Fuzhou 
		(a) ATS communication device status
		01 ( binary ): ATS FEP offline data is invalid.
		10 ( binary ): ATS FEP online , the data is valid
		**/
		unsigned long startAddress = addressBlock.start();

		// word 0, bit 0, 1 indicates fep online or off-line
		WORD registerValue = addressBlock [ startAddress ];
		
		if(( (0x0001 & registerValue) == 0) && ((0x0002 & registerValue) > 0))
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "FEP status word = 0X%X - ATS FEP data is online., previous m_isOnline = %d, startAddress=%u", registerValue, m_isOnline, startAddress);
			m_isOnline = true;
		}
		else
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Check FEP Status word = 0X%X :bit0 and bit1 should not be of same value.Valid combinations ={(0,1),(1,0)}", registerValue);
			m_isOnline = false;
		}
	}
}
