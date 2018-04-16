/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/configuration/config_plugins/message_type/src/MessageTypeFrameworkView.cpp $
  * @author:  Dirk McCormick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * This view contains a splitter. It then loads the left side of the splitter as a list of alarm types
  * and the right side as a view for the user to configure alarm type details.
  */

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif // _MSC_VER


#include "app/configuration/config_plugins/message_type/src/StdAfx.h"
#include "app/configuration/config_plugins/message_type/src/MessageTypeFrameworkView.h"

#include "app/configuration/config_plugins/message_type/src/MessageTypeListView.h"
#include "app/configuration/config_plugins/message_type/src/MessageTypeDetailView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
    IMPLEMENT_DYNCREATE(MessageTypeFrameworkView, CView)


    MessageTypeFrameworkView::MessageTypeFrameworkView()
    {
    }


    MessageTypeFrameworkView::~MessageTypeFrameworkView()
    {
    }


BEGIN_MESSAGE_MAP(MessageTypeFrameworkView, AbstractFrameworkView)
	//{{AFX_MSG_MAP(MessageTypeFrameworkView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

    
    CRuntimeClass* MessageTypeFrameworkView::getRuntimeClassForDetailView()
    {
        return RUNTIME_CLASS(MessageTypeDetailView);
    }


    CRuntimeClass* MessageTypeFrameworkView::getRuntimeClassForListView()
    {
        return RUNTIME_CLASS(MessageTypeListView);
    }

    
    /////////////////////////////////////////////////////////////////////////////
    // MessageTypeFrameworkView diagnostics

    #ifdef _DEBUG
    void MessageTypeFrameworkView::AssertValid() const
    {
	    AbstractFrameworkView::AssertValid();
    }

    void MessageTypeFrameworkView::Dump(CDumpContext& dc) const
    {
	    AbstractFrameworkView::Dump(dc);
    }
    #endif //_DEBUG

}



