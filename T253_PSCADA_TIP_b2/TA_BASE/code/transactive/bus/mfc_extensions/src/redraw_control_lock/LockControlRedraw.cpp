/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/bus/mfc_extensions/src/redraw_control_lock/LockControlRedraw.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2017/10/31 16:21:54 $
  * Last modified by: $Author: CM $
  * 
  * Helper class to handle setting and resetting of redraw flag
  *     for controls (to reduce flicker, when making major changes)
  *
  */

#include "../stdafx.h"
#include "bus/mfc_extensions/src/redraw_control_lock/LockControlRedraw.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_Base_Bus
{

    LockControlRedraw::LockControlRedraw(CWnd& window)
    :
    m_window(window)
    {    
        // Turn off redraw
        m_window.SetRedraw(FALSE);
    }
    
    
    LockControlRedraw::~LockControlRedraw()
    {
        // Restore the redraw state
        m_window.SetRedraw(TRUE);
    
        // Invalidate the window to force complete redraw
        m_window.InvalidateRect(0, TRUE);    
    }

}
    
