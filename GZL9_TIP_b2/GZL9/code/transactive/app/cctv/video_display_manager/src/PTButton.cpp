#include "PTButton.h"
#include "CCTVCommon.h"

namespace TA_IRS_App
{
    CPTButton::CPTButton ( unsigned short ptDirection)
        : m_ptDirection( ptDirection )
    {
    }

    CPTButton::~CPTButton()
    {
    }

    bool CPTButton::handleButtonUp ()
    {
        return CPTZButton::m_commandListener->onPTCommand ( m_ptDirection, true );
    }

    bool CPTButton::handleButtonDown()
    {
        return CPTZButton::m_commandListener->onPTCommand ( m_ptDirection, false );
    }
}
