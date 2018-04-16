#include "app/cctv/video_switch_agent/protocols/COE/src/RecordingPlaybackCommand.h"
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
    static const int CMD_DATA_LEN = 18;


    RecordingPlaybackCommand::RecordingPlaybackCommand(COECamera& camera,
                                                       COEVideoOutput& monitor,
                                                       const Time& startTime,
                                                       const Time& endTime)
        : AbstractCOERequestCommand(),
          m_expectedResponseCmd(RECORDING_PLAYBACK_COMMAND),
          m_monitor(monitor),
          m_camera(camera)
    {
        // Build command protocal format
        unsigned int packetLength = PACKET_HEADER_LENGTH + CMD_DATA_LEN;
        unsigned int totalPacketLength = NUM_LENGTH_BYTES + packetLength;
        std::vector< char > msg(totalPacketLength, 0x00);
        setPacketLength(totalPacketLength - TA_COE::NUM_LENGTH_BYTES, msg[LSB_LENGTH_BYTE], msg[MSB_LENGTH_BYTE]);
        msg[ COMMAND_BYTE ] = m_expectedResponseCmd;
        // Fill data
        std::pair<unsigned short, unsigned short> address = m_camera.getParsedAddress();
        unsigned short stationId = address.first;
        unsigned short cameraId = address.second;
        unsigned short monitorId = getIdFromAddress(m_monitor.getAddress());
        std::vector<char>::iterator it = msg.begin() + FIRST_DATA_BYTE;
        fillupDataInLittleEndian(msg, it, (char)stationId);
        fillupDataInLittleEndian(msg, it, cameraId);
        fillupDataInLittleEndian(msg, it, (char)monitorId);
        fillupDataInLittleEndian(msg, it, startTime.year);
        fillupDataInLittleEndian(msg, it, startTime.month);
        fillupDataInLittleEndian(msg, it, startTime.day);
        fillupDataInLittleEndian(msg, it, startTime.hour);
        fillupDataInLittleEndian(msg, it, startTime.minute);
        fillupDataInLittleEndian(msg, it, startTime.second);
        fillupDataInLittleEndian(msg, it, endTime.year);
        fillupDataInLittleEndian(msg, it, endTime.month);
        fillupDataInLittleEndian(msg, it, endTime.day);
        fillupDataInLittleEndian(msg, it, endTime.hour);
        fillupDataInLittleEndian(msg, it, endTime.minute);
        fillupDataInLittleEndian(msg, it, endTime.second);
        m_data.insert(m_data.end(), msg.begin(), msg.end());
    }


    RecordingPlaybackCommand::~RecordingPlaybackCommand()
    {
    }
    //
    // processResponse
    //
    bool RecordingPlaybackCommand::processResponse(const std::vector< unsigned char >& data)
    {
        return processGenericResponsePacketAttributes(data, m_expectedResponseCmd);
    }

}
