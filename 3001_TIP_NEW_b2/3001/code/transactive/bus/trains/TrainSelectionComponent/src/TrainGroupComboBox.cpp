/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/bus/trains/TrainSelectionComponent/src/TrainGroupComboBox.cpp $
  * @author:  Adam Radics
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * A Combo box that accepts updateSelectionCombo messages.
  * In the resource editor it must be a dropdown box, not a drop list box
  * to allow typing of new group names.
  */

#include "bus/trains/TrainSelectionComponent/src/StdAfx.h"
#include "bus/trains/TrainSelectionComponent/src/TrainGroupComboBox.h"

#include "core/utilities/src/DebugUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_IRS_Bus
{

    TrainGroupComboBox::TrainGroupComboBox()
        : m_eventListener(NULL)
    {}

    TrainGroupComboBox::~TrainGroupComboBox()
    {}


    BEGIN_MESSAGE_MAP(TrainGroupComboBox, CComboBox)
	    //{{AFX_MSG_MAP(TrainGroupComboBox)
		ON_CONTROL_REFLECT_EX(CBN_SELCHANGE, onSelectionChange)
        ON_CONTROL_REFLECT_EX(CBN_EDITCHANGE, onEditchange)
	    //}}AFX_MSG_MAP
    END_MESSAGE_MAP()


    void TrainGroupComboBox::updateGroupNames( const std::vector<std::string>& trainListNames )
    {
		FUNCTION_ENTRY( "updateGroupNames" );

        // Ensure we retain selection of current group (if possible)
        // While updating the dynamic group selection list
        CString currentSelectedString;
        int sel = GetCurSel();
        if (sel >= 0)
        {
            GetLBText( sel, currentSelectedString );
        }
        else
        {
            // if an operator typed in a name and clicked save,
            // this wouldnt have shown up as a selected item
            // check if there is something in the list box section
            GetWindowText(currentSelectedString);
        }

        // Turn off redraw for this operation
        SetRedraw(FALSE);

        if (GetDroppedState() && (0 == trainListNames.size()) )
        {
            // (see TestDirector Defect #2274 for why this is done)
            ShowDropDown(FALSE);
        }

        // Replace all strings
        ResetContent();
        for (unsigned int i = 0; i < trainListNames.size(); i ++)
        {
            AddString(trainListNames[i].c_str());
        }

        // clear the input area
        SetWindowText("");

        // enable redraw again
        SetRedraw(TRUE);
        // force it to redraw - invalidating doesnt seem to work
        RedrawWindow();

        // Re select item we had selected before
        if (currentSelectedString.GetLength() > 0)
        {
            int newSelIndex = SelectString( -1, currentSelectedString.GetBuffer(0));
            if (newSelIndex >= 0)
            {
                // exists in the list
                SetCurSel(newSelIndex);

                // put the name into the list box part as well
                CString itemText;
                GetLBText(newSelIndex, itemText);
                SetWindowText(itemText);

                // if the selection was modified externally, there should be a refresh
                // send the message to the owning dialog
                CWnd* parent = GetParent();
                if (parent != NULL)
                {
                    parent->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(),CBN_SELCHANGE),
                                        reinterpret_cast<LPARAM>(GetSafeHwnd()));
                }
            }
            else
            {
                // maybe the operator was in the process of typing a name
                // better put back what was there
                SetWindowText(currentSelectedString);
            }
        }

		FUNCTION_EXIT;
    }


    std::string TrainGroupComboBox::getCurrentGroupName()
    {
		FUNCTION_ENTRY( "getCurrentGroupName" );

        CString currentSelectedString;
        int sel = GetCurSel();
        if (sel >= 0)
        {
            GetLBText(sel, currentSelectedString);
        }
        else
        {
            // if an operator typed in a name not in the list,
            // this wouldnt have shown up as a selected item
            // check if there is something in the list box section
            GetWindowText(currentSelectedString);
        }

		FUNCTION_EXIT;
        return std::string(currentSelectedString);
    }


    bool TrainGroupComboBox::currentGroupNameIsAnExistingGroup()
    {
		FUNCTION_ENTRY( "currentGroupNameIsAnExistingGroup" );

        std::string currentName = getCurrentGroupName();
        
		FUNCTION_EXIT;
        return ( (currentName != "") &&
                 (FindStringExact(0, currentName.c_str()) > -1) );
    }

    
    void TrainGroupComboBox::setEventListener( ITrainGroupComboEventListener* eventListener )
    {
		FUNCTION_ENTRY( "setEventListener" );

        m_eventListener = eventListener;

		FUNCTION_EXIT;
    }


    BOOL TrainGroupComboBox::onSelectionChange() 
    {
		FUNCTION_ENTRY( "onSelectionChange" );

        // if there is something managing events
	    if (m_eventListener != NULL)
	    {
		    m_eventListener->selectionChangedGroupCombo();
	    }

		FUNCTION_EXIT;
        // this allows the owning window to handle this as well
        return FALSE;
    }

    BOOL TrainGroupComboBox::onEditchange()
    {
		FUNCTION_ENTRY( "onEditchange" );

        // if there is something managing events
	    if (m_eventListener != NULL)
	    {
		    m_eventListener->editChangedGroupCombo();
	    }

		FUNCTION_EXIT;
        // this allows the owning window to handle this as well
        return FALSE;
    }

} // TA_IRS_Bus