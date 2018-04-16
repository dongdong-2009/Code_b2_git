/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/stis_manager/src/CallbackButton.cpp $
  * @author:  Adam Radics
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * This is a button that listens for its own button press events
  * and passes them on to the specified interface.
  * Done so that the owning dialog doesnt have to listen for and handle the events.
  */

#include "stdafx.h"
#include "CallbackButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_IRS_App
{

    CallbackButton::CallbackButton()
        : m_buttonPressListener(NULL)
    {
    }

    CallbackButton::~CallbackButton()
    {
    }


    BEGIN_MESSAGE_MAP(CallbackButton, CButton)
	    //{{AFX_MSG_MAP(CallbackButton)
		ON_CONTROL_REFLECT_EX(BN_CLICKED, onButtonClicked)
	    //}}AFX_MSG_MAP
    END_MESSAGE_MAP()


    void CallbackButton::setCallback(IButtonListener* buttonPressListener)
    {
        m_buttonPressListener = buttonPressListener;
    }

    BOOL CallbackButton::onButtonClicked()
    {
        if (m_buttonPressListener != NULL)
        {
            m_buttonPressListener->buttonPressed(this);
        }

        // allow other windows to handle it
        return FALSE;
    }
}
