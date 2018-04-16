/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  oliverk
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */

#include "bus/trains/TrainSelectionComponent/src/StdAfx.h"
#include "bus/trains/TrainSelectionComponent/src/TrainStaticGroupComboBox.h"

#include "core/utilities/src/DebugUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


namespace TA_IRS_Bus
{
    TrainStaticGroupComboBox::TrainStaticGroupComboBox()
		: m_eventListener( NULL )
    {}


    TrainStaticGroupComboBox::~TrainStaticGroupComboBox()
    {}


	BEGIN_MESSAGE_MAP(TrainStaticGroupComboBox, CComboBox)
		//{{AFX_MSG_MAP(TrainGroupComboBox)
		ON_CONTROL_REFLECT_EX(CBN_SELCHANGE, onSelectionChange)
        ON_CONTROL_REFLECT_EX(CBN_EDITCHANGE, onEditChange)
		//}}AFX_MSG_MAP
    END_MESSAGE_MAP()


    void TrainStaticGroupComboBox::updateGroupNames(const std::vector<std::string>& trainListNames)
    {
		FUNCTION_ENTRY( "updateGroupNames" );

        //Get the currently selected item
		CString currentSelectedString;
		int sel = GetCurSel();
		if ( sel >= 0 )
		{
			GetLBText( sel, currentSelectedString );
		}
		else
		{
			SetWindowText( currentSelectedString );
		}

		// Turn off redraw for this operation
        SetRedraw(FALSE);

		if (GetDroppedState() && (0 == trainListNames.size()) )
        {
            // (see TestDirector Defect #2274 for why this is done)
            ShowDropDown(FALSE);
        }

        //Clear the combo box
		ResetContent();

        //add a single blank item
        AddString("");

        //add all items from the list to the combo box
		for (unsigned int i = 0; i < trainListNames.size(); i ++)
        {
            AddString(trainListNames[i].c_str());
        }
		SetWindowText("");
		SetRedraw(TRUE);
		RedrawWindow();
		
        //if the old selected item still exists in the combo, reselect it
		if (currentSelectedString.GetLength() > 0 )
		{
			int newSelIndex = SelectString(-1, currentSelectedString.GetBuffer( 0 ) );
			if( newSelIndex >= 0 )
			{
				// exists in the list
				SetCurSel( newSelIndex );
				CString itemText;
				GetLBText(newSelIndex, itemText);
				SetWindowText(itemText);

				// send the message to the owning dialog
				CWnd* parent = GetParent();
                if (parent != NULL)
                {
                    parent->SendMessage( WM_COMMAND, MAKEWPARAM( GetDlgCtrlID(),CBN_SELCHANGE ),
										 reinterpret_cast<LPARAM>( GetSafeHwnd() ) );
                }
			}
			else
			{
				SetWindowText(currentSelectedString);
			}
		}

		FUNCTION_EXIT;
    }


    std::string TrainStaticGroupComboBox::getCurrentGroupName()
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


    void TrainStaticGroupComboBox::setEventListener(ITrainStaticGroupComboEventListener* eventListener)
    {
		FUNCTION_ENTRY( "setEventListener" );

		m_eventListener = eventListener;

		FUNCTION_EXIT;
    }


    BOOL TrainStaticGroupComboBox::onEditChange()
    {
		FUNCTION_ENTRY( "onEditChange" );

		// if there is something managing events
		if (m_eventListener != NULL)
		{
			m_eventListener->editChangedStaticGroupCombo();
		}

		FUNCTION_EXIT;
        return FALSE;
    }


    BOOL TrainStaticGroupComboBox::onSelectionChange()
    {
		FUNCTION_ENTRY( "onSelectionChange" );

        // if there is something managing events
		if (m_eventListener != NULL)
		{
			m_eventListener->selectionChangedStaticGroupCombo();
		}
		
		FUNCTION_EXIT;
        // this allows the owning window to handle this as well
        return FALSE;
    }
}