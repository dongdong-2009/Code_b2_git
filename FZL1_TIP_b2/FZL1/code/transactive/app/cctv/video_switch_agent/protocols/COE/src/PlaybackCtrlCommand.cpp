#include "app/cctv/video_switch_agent/protocols/COE/src/PlaybackCtrlCommand.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COEHelper.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COESwitchManager.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COEVideoOutput.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/VideoSwitchAgentInternalException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_IRS_App
{

    using namespace TA_COE;
    static const int CMD_DATA_LEN = 2;
    PlaybackCtrlCommand::PlaybackCtrlCommand(COEVideoOutput& monitor, CommandType cmd)
        : AbstractCOERequestCommand(),
          m_expectedResponseCmd(PLAYBACK_CTRL_COMMAND),
          m_monitor(monitor)
    {
        // Get monitor id
        unsigned short id;
        std::istringstream idStr(m_monitor.getAddress());
        idStr >> id;
        // Build command protocal format
        unsigned int packetLength = PACKET_HEADER_LENGTH + CMD_DATA_LEN;
        unsigned int totalPacketLength = NUM_LENGTH_BYTES + packetLength;
        std::vector< char > msg(totalPacketLength, 0x00);
        setPacketLength(totalPacketLength - TA_COE::NUM_LENGTH_BYTES, msg[LSB_LENGTH_BYTE], msg[MSB_LENGTH_BYTE]);
        msg[ COMMAND_BYTE ] = m_expectedResponseCmd;
        msg[ 4 ] = id % 0x0100;
        msg[ 5 ] = (char)cmd;
        m_data.insert(m_data.end(), msg.begin(), msg.end());
    }


    //
    // processResponse
    //
    bool PlaybackCtrlCommand::processResponse(const std::vector< unsigned char >& data)
    {
        return processGenericResponsePacketAttributes(data, m_expectedResponseCmd);
    }

    PlaybackCtrlCommand::~PlaybackCtrlCommand()
    {
    }

}
