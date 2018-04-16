/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/3001_TIP_NEW/3001/transactive/app/scada/PMSAgent/src/RTUCommandWorker.h $
  * @author:	Raymond Pau
  * @version:	$Revision: #3 $
  *
  * Last modification:	$DateTime: 2012/06/12 16:25:12 $
  * Last modified by:	$Author: haijun.li $
  *
  * Description:
  *
  *	The RTUCommandWorker object is responsible for sending commands for controlling
  * the status of the RTU
  *
  */


#ifndef RTUCOMMANDWORKER_H
#define RTUCOMMANDWORKER_H

#include <string>

#include "app/scada/PMSAgent/src/StationSystemOutputWriter.h"

#include "bus/modbus_comms/src/DataBlock.h"

#include "core/message/src/NameValuePair.h"
#include "core/sockets/src/SocketOptions.h"
#include "core/synchronisation/src/WriteReadThreadLockable.h"

namespace TA_Base_Bus
{
    class IModbus;
}

namespace TA_Base_Core
{
    class AuditMessageSender;
    class MessageType;
}

namespace TA_IRS_App
{
    // forward declaration
    class RTU;

	class RTUCommandWorker
	{
	// operations
	public:

		RTUCommandWorker ( RTU& rtu, StationSystemOutputWriter& outputWriter);
			
		virtual ~RTUCommandWorker();

		//
		// operations to command RTU
		//

		bool resetRTU();
		bool switchPrimaryToStandby();
		bool switchStandbyToPrimary();
		bool setDownloadRequired();
		bool setDownloadCompleted(); //TD10586

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
		bool enablePSD3Polling();
		bool enablePSD4Polling();
		bool enablePLC1Polling();
		bool enablePLC2Polling();
		bool enablePLC3Polling();
		bool enablePLC4Polling();
		bool enableReserved1Polling();
		bool enableReserved2Polling();
		bool enableReserved3Polling();
		bool enableReserved4Polling();

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
		bool inhibitPSD3Polling();
		bool inhibitPSD4Polling();
		bool inhibitPLC1Polling();
		bool inhibitPLC2Polling();
		bool inhibitPLC3Polling();
		bool inhibitPLC4Polling();
		bool inhibitReserved1Polling();
		bool inhibitReserved2Polling();
		bool inhibitReserved3Polling();
		bool inhibitReserved4Polling();

	// operations
	private:

		bool sendRTUCommand( const WORD & commandValue );

        // disabled constructors
		RTUCommandWorker();
		RTUCommandWorker ( const RTUCommandWorker & obj );
		RTUCommandWorker & operator= ( const RTUCommandWorker & rhs );

	// attributes
	private:
        TA_Base_Core::WriteReadThreadLockable m_modbusLock;

        RTU& m_rtu;
        StationSystemOutputWriter& m_outputWriter;
	};
}

#endif
