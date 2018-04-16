/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File$
 * @author:  Rob Young
 * @version: $Revision$
 *
 * Last modification: $DateTime$
 * Last modified by:  $Author$
 *
 * Object for requesting an input be switched to an output and for
 * processing the response.
 */

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif // _MSC_VER

#include "app/cctv/video_switch_agent/protocols/COE/src/SwitchInputToOutputCommand.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COEHelper.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COESwitchManager.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COEVideoInput.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COEVideoOutput.h"

#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/VideoSwitchAgentInternalException.h"
#include "core/utilities/src/DebugUtil.h"

#include <sstream>

namespace TA_IRS_App
{
	using namespace TA_COE;
    //
    // SwitchInputToOutputCommand
    //
    SwitchInputToOutputCommand::SwitchInputToOutputCommand(unsigned long videoInputEntityKey, COEVideoOutput& output)
        : AbstractCOERequestCommand(),
		  m_videoInputEntityKey(videoInputEntityKey),
          m_videoOutput(output),
		  m_expectedResponseCmd(0x00)
    {
    }


    void SwitchInputToOutputCommand::buildCommand()
    {
        if(m_videoInputEntityKey != 0)
        {
            COESwitchManager::ECOEInputType inputType = COESwitchManager::getInstance().getInputType(m_videoInputEntityKey);
            if(COESwitchManager::CAMERA == inputType)
            {
                buildCamAndMonCmd();
            }
            else if(COESwitchManager::SEQUENCE == inputType)
            {
                buildSeqAndMonCmd();
            }
        }
        else
        {
            buildClearMonCmd();
        }
    }

    void SwitchInputToOutputCommand::buildCamAndMonCmd()
    {
        COECamera* camera = COESwitchManager::getInstance().getCamera(m_videoInputEntityKey);
		TA_ASSERT(NULL != camera, "");
        m_data.erase(m_data.begin(), m_data.end());
        std::pair< unsigned short, unsigned short > addrPr = camera->getParsedAddress();
        unsigned short monID;
        std::istringstream monAddr(m_videoOutput.getAddress());
        monAddr >> monID;
		unsigned int totalPacketLength = 8;
        std::vector< char > msg(totalPacketLength, 0x00);
        setPacketLength(totalPacketLength - TA_COE::NUM_LENGTH_BYTES, msg[LSB_LENGTH_BYTE], msg[MSB_LENGTH_BYTE]);
        m_expectedResponseCmd = msg[ 3 ] = CAM_AND_MON_COMMAND;
        msg[ 4 ] = (addrPr.first % 0x0100);
        msg[ 5 ] = (addrPr.second % 0x0100);
        msg[ 6 ] = (addrPr.second / 0x0100);
        msg[ 7 ] = (monID % 0x0100);
        m_data.insert(m_data.end(), msg.begin(), msg.end());
    }


    void SwitchInputToOutputCommand::buildSeqAndMonCmd()
    {
        COESequence* seq = COESwitchManager::getInstance().getSequence(m_videoInputEntityKey);
		TA_ASSERT(NULL != seq, "");
        m_data.erase(m_data.begin(), m_data.end());
        unsigned short seqID;
        std::istringstream seqAddr((*seq).getAddress());
        unsigned short monID;
        std::istringstream monAddr(m_videoOutput.getAddress());
        monAddr >> monID;
        seqAddr >> seqID;
		unsigned int totalPacketLength = 6;
        std::vector< char > msg(totalPacketLength, 0x00);
        setPacketLength(totalPacketLength - TA_COE::NUM_LENGTH_BYTES, msg[LSB_LENGTH_BYTE], msg[MSB_LENGTH_BYTE]);
        m_expectedResponseCmd = msg[ 3 ] = SEQ_AND_MON_COMMAND;
        msg[ 4 ] = (seqID % 0x0100);
        msg[ 5 ] = (monID % 0x0100);
        m_data.insert(m_data.end(), msg.begin(), msg.end());
    }


    void SwitchInputToOutputCommand::buildClearMonCmd()
    {
        m_data.erase(m_data.begin(), m_data.end());
        unsigned short monID;
        std::istringstream monAddr(m_videoOutput.getAddress());
        monAddr >> monID;
		unsigned int totalPacketLength = 5;
        std::vector< char > msg(totalPacketLength, 0x00);
        setPacketLength(totalPacketLength - TA_COE::NUM_LENGTH_BYTES, msg[LSB_LENGTH_BYTE], msg[MSB_LENGTH_BYTE]);
        m_expectedResponseCmd = msg[ 3 ] = CLEAR_COMMAND;
        msg[ 4 ] = (monID % 0x0100);
        m_data.insert(m_data.end(), msg.begin(), msg.end());
    }


    //
    // processResponse
    //
    bool SwitchInputToOutputCommand::processResponse(const std::vector< unsigned char >& data)
    {
        if(processGenericResponsePacketAttributes(data,  m_expectedResponseCmd))
        {
            if(SUCCEEDED == m_completionState)
            {
                m_videoOutput.notifyInputSwitchedToOutput(m_videoInputEntityKey);
            }
            return true;
        }
		return false;
    }

}
