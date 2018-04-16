/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/bus/scada/rtu_library/src/RTUStatusWorker.cpp $
  * @author:	Liu Yong
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

#if defined( WIN32 )
	#pragma warning( disable : 4786 )
#endif // defined( WIN32 )

#include "bus/scada/rtu_library/src/RTUStatusWorker.h"
#include "bus/modbus_comms/src/DataBlock.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{
    RTUStatusWorker::RTUStatusWorker()
    :
    m_isCommsOK(false)    
    {
    }


    RTUStatusWorker::~RTUStatusWorker()
    {

    }


    void RTUStatusWorker::processRTUDataBlock( const TA_Base_Bus::DataBlock< WORD > & addressBlock, 
                                               const bool isCommsOK )
    {
        m_isCommsOK = isCommsOK;
    }

    bool RTUStatusWorker::getIsCommsOK() const
    {
      return m_isCommsOK ;
    }

    bool RTUStatusWorker::getIsRTUSwitching() const
    {
      return false;
    }

    bool RTUStatusWorker::getIsPrimaryRTU() const
    {
      return m_isCommsOK;
    }
    
    void RTUStatusWorker::resetStatusWorker()
    {
        m_isCommsOK = false;
    }
}
