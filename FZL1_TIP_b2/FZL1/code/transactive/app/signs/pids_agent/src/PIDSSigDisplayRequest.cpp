/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3002_TIP/3002/transactive/app/signs/pids_agent/src/PIDSSigDisplayRequest.cpp $
  * @author:   huang.wenbo
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: $
  * Last modified by:  $Author: $
**/


#include "app/signs/pids_agent/src/PIDSSigDisplayRequest.h"
#include "app/signs/pids_agent/src/RawTable.h"
#include "core/utilities/src/DebugUtil.h"
namespace TA_IRS_App
{
	const int PIDSSigDisplayRequest::SIGINFO_FEP_REGISTER_ADDRESS = 511;

    void PIDSSigDisplayRequest::setData( const TA_Base_Bus::IPIDSManagerCorbaDef::SigData& data)
    {
		m_data = data;
    }

    TA_Base_Bus::DataBlock<WORD> PIDSSigDisplayRequest::createProtocolMessage()
    {        
        RawTable rawTable( SIGINFO_FEP_REGISTER_ADDRESS, SIGINFO_FEP_REGISTER_ADDRESS+(m_data.length() + 1)/2-1);
		for(unsigned long i = 0; i < m_data.length(); ++i)
		{
			rawTable.putByte(i, m_data[i]);
		}
        return rawTable.getDataBlock();
    }
}