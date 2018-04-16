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
  * Object for requesting new sequence configuration and processing the response.
  *
  */

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif // _MSC_VER

#include "app/cctv/video_switch_agent/protocols/COE/src/SetSequenceConfigCommand.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COEHelper.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COESwitchManager.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/VideoSwitchAgentInternalException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include <boost/foreach.hpp>

namespace TA_IRS_App
{

	using namespace TA_COE;
    //
    // SetSequenceConfigCommand
    //
    SetSequenceConfigCommand::SetSequenceConfigCommand(unsigned short dwellTime, const std::vector< unsigned long >& videoInputs, COESequence& sequence)
        : AbstractCOERequestCommand(),
          m_expectedResponseCmd(PROG_SEQ_COMMAND),
          m_dwellTime(dwellTime),
          m_videoInputs(videoInputs),
          m_sequence(sequence)
    {
        m_data.erase(m_data.begin(), m_data.end());
        unsigned short sequenceID;
        std::istringstream seqAddr(m_sequence.getAddress());
        seqAddr >> sequenceID;
        unsigned int packetLength = 4 + 3*m_videoInputs.size();
		unsigned int totalPacketLength = packetLength + NUM_LENGTH_BYTES;
		if(m_videoInputs.empty())
		{
            m_dwellTime = 0;
		}
        std::vector< char > msg(5, 0x00);
        setPacketLength(totalPacketLength - TA_COE::NUM_LENGTH_BYTES, msg[LSB_LENGTH_BYTE], msg[MSB_LENGTH_BYTE]);
        msg[ 3 ] = PROG_SEQ_COMMAND;
        msg[ 4 ] = (sequenceID % 0x0100);
        msg.push_back(dwellTime % 0x0100);
        COECamera* pCOECamera = 0; // Only Cameras can be configured for sequences in this protocol.
        std::pair< unsigned short, unsigned short > camAddr(0, 0);
		BOOST_FOREACH(unsigned long i, m_videoInputs)
		{
            pCOECamera = COESwitchManager::getInstance().getCamera(i);
            if(0 == pCOECamera)
            {
                TA_THROW(TA_Base_Core::VideoSwitchAgentInternalException(TA_Base_Core::VideoSwitchAgentInternalException::OBJECT_DOES_NOT_EXIST, "The InputType does not exist."));
            }
            camAddr = pCOECamera->getParsedAddress();
            msg.push_back(camAddr.first % 0x0100);
            msg.push_back(camAddr.second % 0x0100);
            msg.push_back(camAddr.second / 0x100);
		}
        TA_ASSERT((msg.size() == (packetLength + 2)), "Incorrect length bytes have been set!");
        m_data.insert(m_data.end(), msg.begin(), msg.end());
    }


    //
    // processResponse
    //
    bool SetSequenceConfigCommand::processResponse(const std::vector< unsigned char >& data)
    {
        if(processGenericResponsePacketAttributes(data,  m_expectedResponseCmd))
        {
            if(SUCCEEDED == m_completionState)
            {
                m_sequence.notifySequenceConfigUpdated(m_dwellTime, m_videoInputs);
            }
            return true;
        }
        return false;
    }

}
