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
  * Object for requesting inputs be switched to a quad and for
  * processing the response.
  *
  */

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif // _MSC_VER

#include "app/cctv/video_switch_agent/protocols/COE/src/SwitchQuadInputsCommand.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COEHelper.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COESwitchManager.h"
//#include "app/cctv/video_switch_agent/protocols/COE/src/COEQuad.h"

#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/VideoSwitchAgentInternalException.h"
#include "core/utilities/src/DebugUtil.h"

#include <sstream>

namespace TA_IRS_App
{

	using namespace TA_COE;
    //
    // SwitchQuadInputsCommand
    //
    SwitchQuadInputsCommand::SwitchQuadInputsCommand(unsigned long topLeftVideoInput,
                                                     unsigned long topRightVideoInput,
                                                     unsigned long bottomLeftVideoInput,
                                                     unsigned long bottomRightVideoInput,
                                                     COEVideoOutput& monitor)
        : AbstractCOERequestCommand(),
          m_expectedResponseCmd(PROG_QUAD_COMMAND),
          m_topLeftVideoInput(topLeftVideoInput),
          m_topRightVideoInput(topRightVideoInput),
          m_bottomLeftVideoInput(bottomLeftVideoInput),
          m_bottomRightVideoInput(bottomRightVideoInput),
          m_monitor(monitor)
    {
        COECamera* pCOECamera = 0;
        std::pair< unsigned short, unsigned short > topLeftAddr(0, 0), topRightAddr(0, 0), bottomLeftAddr(0, 0), bottomRightAddr(0, 0);
        fillUpCameraAddressPairByKey(topLeftAddr, m_topLeftVideoInput);
        fillUpCameraAddressPairByKey(topRightAddr, m_topRightVideoInput);
        fillUpCameraAddressPairByKey(bottomLeftAddr, m_bottomLeftVideoInput);
        fillUpCameraAddressPairByKey(bottomRightAddr, m_bottomRightVideoInput);
        m_data.erase(m_data.begin(), m_data.end());
        unsigned short monitorID;
        std::istringstream monitorAddr(m_monitor.getAddress());
        monitorAddr >> monitorID;
        std::vector< char > msg(17, 0x00);
        msg[ 0 ] = 15;
        msg[ 3 ] = PROG_QUAD_COMMAND;
        msg[ 4 ] = (monitorID % 0x0100);
        msg[ 5 ] = (topLeftAddr.first % 0x0100);
        msg[ 6 ] = (topLeftAddr.second % 0x0100);
        msg[ 7 ] = (topLeftAddr.second / 0x0100);
        msg[ 8 ] = (topRightAddr.first % 0x0100);
        msg[ 9 ] = (topRightAddr.second % 0x0100);
        msg[ 10 ] = (topRightAddr.second / 0x0100);
        msg[ 11 ] = (bottomLeftAddr.first % 0x0100);
        msg[ 12 ] = (bottomLeftAddr.second % 0x0100);
        msg[ 13 ] = (bottomLeftAddr.second / 0x0100);
        msg[ 14 ] = (bottomRightAddr.first % 0x0100);
        msg[ 15 ] = (bottomRightAddr.second % 0x0100);
        msg[ 16 ] = (bottomRightAddr.second / 0x0100);
        m_data.insert(m_data.end(), msg.begin(), msg.end());
    }

    SwitchQuadInputsCommand::~SwitchQuadInputsCommand()
    {
    }

    void SwitchQuadInputsCommand::fillUpCameraAddressPairByKey(std::pair< unsigned short, unsigned short >& addressPair, unsigned long cameraKey)
    {
        if(0 != cameraKey)
        {
            COECamera* pCOECamera = COESwitchManager::getInstance().getCamera(cameraKey);
			TA_ASSERT(NULL != pCOECamera, "");
            addressPair = pCOECamera->getParsedAddress();
        }
    }
    //
    // processResponse
    //
    bool SwitchQuadInputsCommand::processResponse(const std::vector< unsigned char >& data)
    {
        if(processGenericResponsePacketAttributes(data,  m_expectedResponseCmd))
        {
            return SUCCEEDED == m_completionState;
        }
        return false;
    }

}
