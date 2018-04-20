/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/TA_BASE/transactive/bus/mfc_extensions/src/redraw_control_lock/LockControlRedraw.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * Helper class to handle setting and resetting of redraw flag
  *     for controls (to reduce flicker, when making major changes)
  *
  */

#pragma once

namespace TA_Base_Bus
{    
    #ifdef IMPORT_EXTENSIONS
    class __declspec(dllimport) LockControlRedraw
    #else
    class AFX_EXT_CLASS LockControlRedraw
    #endif
    {
    public:
    
        LockControlRedraw(CWnd& window);
        virtual ~LockControlRedraw();
    
    private:
    
        CWnd&   m_window;
    
        // There is no such thing as GetRedraw..
        //BOOL    m_oldRedrawState;  
    };
}    
