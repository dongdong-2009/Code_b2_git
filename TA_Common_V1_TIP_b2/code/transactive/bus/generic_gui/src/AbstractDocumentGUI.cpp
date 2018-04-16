/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/generic_gui/src/AbstractDocumentGUI.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This inherits from AbstractGUIApplication and implements all methods specific to documents.
  */


#include "bus\generic_gui\src\StdAfx.h"
#include "bus\generic_gui\src\IGUIApplication.h"
#include "bus\generic_gui\src\IGUIAccess.h"
#include "bus\generic_gui\src\AbstractDocumentGUI.h"
#include "bus\generic_gui\src\ITransActiveFrame.h"

#include "core\utilities\src\DebugUtil.h"
#include "core\utilities\src\TAAssert.h"


using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{
    AbstractDocumentGUI::AbstractDocumentGUI()
    {
    }


    AbstractDocumentGUI::~AbstractDocumentGUI()
    {
    }


    bool AbstractDocumentGUI::displayApplication()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "displayApplication" );

        createApplicationObject();

        CWnd* app = getApplicationWnd();
        TA_ASSERT(app != NULL, "CWnd pointer was never set up correctly. Nothing assigned to m_application");

        // This must be called to set this object and to ensure that all initialisation gets done.
        TA_Base_Bus::ITransActiveFrame* frame = dynamic_cast<TA_Base_Bus::ITransActiveFrame*>(AfxGetMainWnd());

        TA_ASSERT(frame != NULL, "The main window is not a ITransActiveFrame");
        
        frame->setCallbackObject(*this);

        app->ShowWindow(SW_SHOW);
        app->UpdateWindow();

        LOG( SourceInfo, DebugUtil::FunctionExit, "displayApplication" );

        return true;
    }
} // end namespace TA_Base_Bus


