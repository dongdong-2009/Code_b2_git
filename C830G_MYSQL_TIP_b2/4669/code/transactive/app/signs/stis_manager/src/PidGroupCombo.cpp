/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/signs/stis_manager/src/PidGroupCombo.cpp $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * A Combo box that accepts updateSelectionCombo messages.
  * In the resource editor it must be a dropdown box, not a drop list box
  * to allow typing of new group names.
  */


#include "StdAfx.h"
#include "PidGroupCombo.h"
#include "PidSelectionManager.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::DebugUtil;
using TA_Base_Bus::ColourCombo;

namespace TA_IRS_App
{

    const std::string PidGroupCombo::HMIGroupName = "Current Schematic Selected Zone";
    static const DWORD COLOR_RED = RGB(255, 0, 0);

    PidGroupCombo::PidGroupCombo()
        : m_pidSelectionManager(NULL)
    {}


    PidGroupCombo::~PidGroupCombo()
    {}


    BEGIN_MESSAGE_MAP(PidGroupCombo, ColourCombo)
	    //{{AFX_MSG_MAP(PidGroupCombo)
		ON_CONTROL_REFLECT_EX(CBN_SELCHANGE, onSelectionChange)
	    //}}AFX_MSG_MAP
    END_MESSAGE_MAP()


    void PidGroupCombo::PreSubclassWindow() 
    {
	    CComboBox::PreSubclassWindow();

        // The schematic group will have item data 0 that will show up red
        mapItemDataToColour(0, COLOR_RED);
    }


    void PidGroupCombo::setPidSelectionManager(PidSelectionManager* pidSelectionManager)
    {
        m_pidSelectionManager = pidSelectionManager;
    }


    void PidGroupCombo::updateGroupNames(const std::vector<std::string>& groupNames)
    {
        // Ensure we retain selection of current group (if possible)
        // While updating the dynamic group selection list
		int sel = GetCurSel();
		std::string currentSelectedString;
		if( sel != CB_ERR)
		{
			currentSelectedString = getCurrentGroupName();
		}

        // Turn off redraw for this operation
        SetRedraw(FALSE);

        if (GetDroppedState() && (0 == groupNames.size()) )
        {
            // (see TestDirector Defect #2274 for why this is done)
            ShowDropDown(FALSE);
        }

        // Replace all strings
        ResetContent();

        for (unsigned int i = 0; i < groupNames.size(); i ++)
        {
            int newPos = AddString(groupNames[i].c_str());

			//if this is the group associated with the schematic change the text to read
			if(groupNames[i] == HMIGroupName)
			{
                SetItemData(newPos, 0); // mapped to RED
			}
            else
            {
                SetItemData(newPos, 1); // not mapped to a colour
            }
        }

        // enable redraw again
        SetRedraw(TRUE);
        // force it to redraw - invalidating doesnt seem to work
        RedrawWindow();

        // Re select item we had selected before if it is still avaiable or select the schematic item
		if( sel != CB_ERR)
		{
			if( CB_ERR != FindString( -1 , currentSelectedString.c_str() ) )
			{
				selectGroupName(currentSelectedString);
			}
			else
			{
				selectGroupName(HMIGroupName);
			}

			
		}
		else
		{
			selectGroupName(HMIGroupName);
		}
    }


    std::string PidGroupCombo::getCurrentGroupName()
    {
        CString currentSelectedString;
        int sel = GetCurSel();

        TA_ASSERT(sel != CB_ERR, "The PID group combo should always have at least one item in it")

        GetLBText(sel, currentSelectedString);

        return std::string(currentSelectedString.GetBuffer(0));
    }


    bool PidGroupCombo::groupNameIsAnExistingGroup(std::string groupName)
    {
        return ( (FindStringExact(0, groupName.c_str()) > -1) );
    }


    void PidGroupCombo::selectGroupName(std::string groupName)
    {
        int newSelIndex = SelectString( -1, groupName.c_str());

        if (m_pidSelectionManager != NULL)
	    {
		    m_pidSelectionManager->pidGroupComboChanged();
	    }
    }

    
    BOOL PidGroupCombo::onSelectionChange() 
    {
        // if there is something managing events
	    if (m_pidSelectionManager != NULL)
	    {
		    m_pidSelectionManager->pidGroupComboChanged();
	    }

        // this allows the owning window to handle this as well
        return FALSE;
    }


} // TA_Bus
