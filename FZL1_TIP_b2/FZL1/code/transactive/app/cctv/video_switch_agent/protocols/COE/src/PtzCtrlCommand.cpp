#include "PtzCtrlCommand.h"
#include "COECamera.h"

namespace TA_IRS_App
{
    using namespace TA_COE;
    static const int CMD_DATA_LEN = 4;
    PtzCtrlCommand::PtzCtrlCommand(COECamera& camera, CommandType type)
        : AbstractCOERequestCommand(),
          m_expectedResponseCmd(PTZ_CTRL_COMMAND),
          m_camera(camera),
          m_type(type)
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
        std::vector<char>::iterator it = msg.begin() + FIRST_DATA_BYTE;
        fillupDataInLittleEndian(msg, it, (char)stationId);
        fillupDataInLittleEndian(msg, it, cameraId);
        fillupDataInLittleEndian(msg, it, (char)m_type);
        m_data.insert(m_data.end(), msg.begin(), msg.end());
    }

    PtzCtrlCommand::~PtzCtrlCommand()
    {
    }

    bool PtzCtrlCommand::processResponse(const std::vector< unsigned char >& data)
    {
        return processGenericResponsePacketAttributes(data, m_expectedResponseCmd);
    }
}