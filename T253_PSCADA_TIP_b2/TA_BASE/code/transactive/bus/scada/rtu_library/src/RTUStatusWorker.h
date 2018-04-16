/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/bus/scada/rtu_library/src/RTUStatusWorker.h $
  * @author:	HoaVu
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2017/10/31 16:21:54 $
  * Last modified by:	$Author: CM $
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

namespace TA_Base_Bus
{
	class RTUStatusWorker
	{
  	// operations
  	public:
    		RTUStatusWorker();
    		virtual ~RTUStatusWorker();

    		//
    		virtual void processRTUDataBlock( const TA_Base_Bus::DataBlock< WORD > & addressBlock, const bool isCommsOK );
        virtual bool getIsCommsOK() const;
        virtual bool getIsRTUSwitching() const;
        virtual bool getIsPrimaryRTU() const;
        virtual void resetStatusWorker();
          
  	private:
          // disabled constructors
      		RTUStatusWorker( const RTUStatusWorker & obj );
      		RTUStatusWorker & operator= ( const RTUStatusWorker & rhs );

  	// attributes
  	protected:
          bool m_isCommsOK;
  	};
}

#endif
