#include "PanButton.h"
#include "CCTVCommon.h"

namespace TA_IRS_App
{
	CPanButton::CPanButton(unsigned short panDirection):m_panDirection(panDirection)
	{
	}

	CPanButton::~CPanButton()
	{
	}

    bool CPanButton::handleButtonUp ()
    {
        return m_commandListener->onPanCommand (m_panDirection, true);
    }

    bool CPanButton::handleButtonDown()
    {
        return m_commandListener->onPanCommand ( m_panDirection, false);
    }
}
