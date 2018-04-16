#include "TiltButton.h"
#include "CCTVCommon.h"

namespace TA_IRS_App
{
    CTiltButton::CTiltButton ( unsigned short tiltDirction ) : m_tiltDirection ( tiltDirction )
    {
    }

    CTiltButton::~CTiltButton ( void )
    {
    }

    bool CTiltButton::handleButtonUp ()
    {
        return CPTZButton::m_commandListener->onTiltCommand ( m_tiltDirection, true);
    }

    bool CTiltButton::handleButtonDown()
    {
        return CPTZButton::m_commandListener->onTiltCommand ( m_tiltDirection, false);
    }
}