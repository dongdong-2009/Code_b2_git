/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Robert Young
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  *
  */

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif // _MSC_VER

#include "app/cctv/video_switch_agent/protocols/COE/src/SummaryPollingCommand.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COECommunicationsHandler.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COEHelper.h"

#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_App
{
	using namespace TA_COE;
    static const int CMD_DATA_LEN = 0;
    //
    // SummaryPollingCommand
    //
    SummaryPollingCommand::SummaryPollingCommand()
        : AbstractCOERequestCommand()
    {
        m_data.erase(m_data.begin(), m_data.end());
		unsigned int totalPacketLength = MIN_VALID_RECEIVED_LENGTH + CMD_DATA_LEN;
        std::vector< char > commandPacket(totalPacketLength, '\x00');
		setPacketLength(totalPacketLength - TA_COE::NUM_LENGTH_BYTES, commandPacket[ LSB_LENGTH_BYTE ], commandPacket[ MSB_LENGTH_BYTE ]);
        commandPacket[ COMMAND_BYTE ] = SUMMARY_POLLING_COMMAND;
        m_data.insert(m_data.end(), commandPacket.begin(), commandPacket.end());
    }


    //
    // processResponse
    //
    bool SummaryPollingCommand::processResponse(const std::vector< unsigned char >& data)
    {
        FUNCTION_ENTRY("processResponse");
        FUNCTION_EXIT;
        return processGenericResponsePacketAttributes(data, SUMMARY_POLLING_COMMAND);
    }
}
