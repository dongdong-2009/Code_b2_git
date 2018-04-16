/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: depot/FZL1_TIP/FZL1/code/transactive/app/ats/AtsAgent/ModBusProtocolHandler/src/SIGStatusWorker.h $
  * @author:	HoaVu
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2015/01/21 16:51:05 $
  * Last modified by:	$Author: haijun.li $
  *
  * Description:
  *
  *	The FEPStatusWorker object is responsible for processing FEP status data
  *
  */

#ifndef SIGSTATUSWORKER_H
#define SIGSTATUSWORKER_H

#include <vector>
#include <string>

#include "bus/modbus_comms/src/IModbus.h"
#include "bus/scada/rtu_library/src/RTUStatusWorker.h"

namespace TA_IRS_App
{
	class SIGStatusWorker: public TA_Base_Bus::RTUStatusWorker
	{
	// operations
	public:

		SIGStatusWorker();

		virtual ~SIGStatusWorker();

		virtual void processRTUDataBlock( const TA_Base_Bus::DataBlock< WORD > & addressBlock, const bool isCommsOK );

        virtual bool getIsCommsOK() const {return m_isCommsOK;};
        virtual bool getIsRTUSwitching() const {return false;};
        virtual bool getIsPrimaryRTU() const {return m_isOnline;};		
	private:

        // disabled constructors
		SIGStatusWorker( const SIGStatusWorker & obj );
		SIGStatusWorker & operator= ( const SIGStatusWorker & rhs );

	// attributes
	private:
		bool m_isOnline;
	};
}

#endif
