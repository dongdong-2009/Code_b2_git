/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP/3001/transactive/app/trains/TrainBorne_PA/src/LockControlRedraw.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2009/12/29 14:12:57 $
  * Last modified by: $Author: grace.koh $
  * 
  * Helper class to handle setting and resetting of redraw flag
  *     for controls (to reduce flicker, when making major changes)
  *
  */

#include "app/trains/TrainBorne_PA/src/stdafx.h"
#include "app/trains/TrainBorne_PA/src/LockControlRedraw.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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
