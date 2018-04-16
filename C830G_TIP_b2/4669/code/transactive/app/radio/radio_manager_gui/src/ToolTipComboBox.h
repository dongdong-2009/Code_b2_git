/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/app/radio/radio_manager_gui/src/ToolTipComboBox.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * Combo Box with tool tips (except doesn't have tool tips at present, because
  * not sure how to get tool tips to work on a dropped down combo box)
  *
  */

#pragma once

class ToolTipComboBox
:
public CComboBox
{
public:
    
    virtual void PreSubclassWindow();

    /**
     * @return the tooltip text that should be displayed for the
     *          currently highlighted item
     */
    virtual CString getToolTipText();

protected:
    
    BOOL OnToolTipNotify(NMHDR * pNotifyStruct, LRESULT* result);
    virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
    DECLARE_MESSAGE_MAP()

private:

};

