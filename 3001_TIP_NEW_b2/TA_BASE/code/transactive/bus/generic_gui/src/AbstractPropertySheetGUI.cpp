/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/generic_gui/src/AbstractPropertySheetGUI.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
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
