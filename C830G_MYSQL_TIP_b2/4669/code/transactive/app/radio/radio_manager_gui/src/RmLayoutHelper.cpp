/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/app/radio/radio_manager_gui/src/RmLayoutHelper.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * Central point for setting up default characteristics of controls
  * 
  */

#include "app/radio/radio_manager_gui/src/stdafx.h"
#include "RmLayoutHelper.h"
#include "bus/mfc_extensions/src/fixed_column_header_ctrl/FixedHeaderCtrl.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_IRS_App
{

    RmLayoutHelper& RmLayoutHelper::getInstance()
    {
        static RmLayoutHelper sInstance;
        return sInstance;
    }
    
    
    RmLayoutHelper::~RmLayoutHelper()
    {
        try
        {
            for (HeaderCtrlList::iterator it = m_headerControls.begin(); it != m_headerControls.end(); ++it )
            {
                delete (*it);
                (*it) = 0;
            }
            m_headerControls.clear();
        }
        catch (...)
        {
        }
    }

    
    void RmLayoutHelper::layoutListControl(CListCtrl& listControl, int lastColumnIndex)
    {
        // Create a new header control object, to stop column resizing
        m_headerControls.resize(m_headerControls.size() + 1);

        TA_Base_Bus::FixedHeaderCtrl* headerCtrl = new TA_Base_Bus::FixedHeaderCtrl();

        TA_ASSERT(0 != headerCtrl, "Header control is NULL");

        m_headerControls.push_back(headerCtrl);

        headerCtrl->subclassHeader(listControl.GetHeaderCtrl());

        // Give all lists tooltips
        listControl.SetExtendedStyle( listControl.GetExtendedStyle() | LVS_EX_INFOTIP);

        // Expand width of last column to fill in remaining space, accounting for scroll bar
        int nColumnCount = listControl.GetHeaderCtrl()->GetItemCount();
        int actualLastColumn = nColumnCount - 1;

        if (lastColumnIndex < 0)
        {
            lastColumnIndex = actualLastColumn;
        }

        if (lastColumnIndex >= 0)
        {
            // Determine the amount of space to be fill in by extending the last column
            CRect rc;
            listControl.GetClientRect(rc);
            int headerTotalWidth = rc.Width();

            int widthSoFar = 0;
            for (int i = 0; i < lastColumnIndex; ++i )
            {
                widthSoFar += listControl.GetColumnWidth(i);                
            }

            // If column widths span off the end of the list control, we can't do anything about
            // the width of the final column in this list            
            if (headerTotalWidth > widthSoFar)
            {
                listControl.SetColumnWidth(lastColumnIndex, headerTotalWidth - widthSoFar);
            }
        }
    }

}   // end namespace TA_IRS_App
