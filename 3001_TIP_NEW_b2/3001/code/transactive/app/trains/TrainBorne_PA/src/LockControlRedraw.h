/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP/3001/transactive/app/trains/TrainBorne_PA/src/LockControlRedraw.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2009/12/29 14:12:57 $
  * Last modified by: $Author: grace.koh $
  * 
  * Helper class to handle setting and resetting of redraw flag
  *     for controls (to reduce flicker, when making major changes)
  *
  */

#ifndef _LockControlRedraw_HEADER_
#define _LockControlRedraw_HEADER_

class LockControlRedraw
{
public:

    LockControlRedraw(CWnd& window);
    virtual ~LockControlRedraw();

private:

    CWnd&   m_window;

    // There is no such thing as GetRedraw..
    //BOOL    m_oldRedrawState;  
};


#endif // _LockControlRedraw_HEADER_