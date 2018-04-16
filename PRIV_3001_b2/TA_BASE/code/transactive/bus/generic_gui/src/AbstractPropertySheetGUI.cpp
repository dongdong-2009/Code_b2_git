/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/bus/generic_gui/src/AbstractPropertySheetGUI.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * This inherits from AbstractGUIApplication and implements all methods specific to 
  * property sheets.
  */

#include "bus\generic_gui\src\StdAfx.h"

#include "bus\generic_gui\src\IGUIApplication.h"
#include "bus\generic_gui\src\IGUIAccess.h"
#include "bus\generic_gui\src\AbstractGUIApplication.h"
#include "bus\generic_gui\src\AbstractPropertySheetGUI.h"

#include "core\utilities\src\DebugUtil.h"
#include "core\utilities\src\TAAssert.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{
    AbstractPropertySheetGUI::AbstractPropertySheetGUI()
    {
    }


    AbstractPropertySheetGUI::~AbstractPropertySheetGUI()
    {
    }


    bool AbstractPropertySheetGUI::displayApplication()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "displayApplication" );

        createApplicationObject();

        CWnd* app = getApplicationWnd();
        TA_ASSERT(app != NULL, "CWnd pointer was never set up correctly. Nothing assigned to m_application");

        // Assign the property sheet as the main window.
        AfxGetApp()->m_pMainWnd = app;

        // Start the property sheet
        CPropertySheet* propertySheet = dynamic_cast<CPropertySheet*>(app);
        TA_ASSERT(propertySheet != NULL, "m_application is not a CPropertySheet");

        propertySheet->DoModal();

        LOG( SourceInfo, DebugUtil::FunctionExit, "displayApplication" );

        return false;
    }	    
}
