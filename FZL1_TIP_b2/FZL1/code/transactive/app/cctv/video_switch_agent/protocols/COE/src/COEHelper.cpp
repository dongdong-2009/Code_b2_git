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
  * Some constants and static functions to assist in the creation and processing of COE Protocol agent
  */

#include "app/cctv/video_switch_agent/protocols/COE/src/COEHelper.h"
#include "app/cctv/video_switch_agent/protocols/src/IAgentRequestCommand.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/SetSequenceConfigCommand.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/SwitchInputToOutputCommand.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/SwitchQuadInputsCommand.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/SummaryPollingCommand.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/RecordingPlaybackCommand.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/VideoSwitchAgentInternalException.h"
#include <sstream>
#include <iterator>

namespace TA_IRS_App
{
    namespace TA_COE
    {
        const char* BAD_ADDRESS_ERROR_MSG = "Invalid value set to Address";

        /////////////////////////////
        ////  Utility Functions  ////
        /////////////////////////////

        //
        // getPacketLength
        //
        unsigned short getPacketLength(unsigned char leastSigByte, unsigned char mostSigByte)
        {
            return ((mostSigByte * BYTE_HEX_CEILING) + leastSigByte);
        }


        //
        // setPacketLength
        //
        void setPacketLength(unsigned short packetLength, char& leastSigByte, char& mostSigByte)
        {
            leastSigByte = (packetLength % BYTE_HEX_CEILING);
            mostSigByte = (packetLength / BYTE_HEX_CEILING);
        }


		std::pair< unsigned short, unsigned short > getParsedAddress(const std::string& address)
		{
	        std::pair< unsigned short, unsigned short > retVal;
	        std::string::size_type split = address.find(':');
	        if(split == std::string::npos)
	        {
	            TA_THROW(TA_Base_Core::VideoSwitchAgentInternalException(TA_Base_Core::VideoSwitchAgentInternalException::CONFIGURATION_ERROR, "Invalid value set to Address"));
	        }
	        std::istringstream part1(address.substr(0, split));
	        if(!(part1 >> retVal.first && part1.eof()) || retVal.first > 0xff)
	        {
	            TA_THROW(TA_Base_Core::VideoSwitchAgentInternalException(TA_Base_Core::VideoSwitchAgentInternalException::CONFIGURATION_ERROR, "Invalid value set to Address"));
	        }
	        std::istringstream part2(address.substr(split + 1, std::string::npos));
	        if(!(part2 >> retVal.second && part2.eof()) || retVal.second > 0xffff)
	        {
	            TA_THROW(TA_Base_Core::VideoSwitchAgentInternalException(TA_Base_Core::VideoSwitchAgentInternalException::CONFIGURATION_ERROR, "Invalid value set to Address"));
	        }
	        return retVal;
		}
        //
        // getIDFromAddress
        //
        unsigned short getIdFromAddress(const std::string& address)
        {
            unsigned short Id;
            std::istringstream addressIStream(address);
            if(!(addressIStream >> Id && addressIStream.eof()))
            {
                TA_THROW(TA_Base_Core::VideoSwitchAgentInternalException(TA_Base_Core::VideoSwitchAgentInternalException::CONFIGURATION_ERROR, "BAD_ADDRESS_ERROR_MSG"));
            }
            return Id;
        }


        //
        // getIdFromByte
        //
        unsigned short getIdFromByte(unsigned char idByte)
        {
            return static_cast< unsigned short >(idByte);
        }
    }
}
