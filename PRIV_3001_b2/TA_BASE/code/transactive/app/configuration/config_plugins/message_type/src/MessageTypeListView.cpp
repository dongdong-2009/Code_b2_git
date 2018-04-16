/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/configuration/config_plugins/message_type/src/MessageTypeListView.cpp $
  * @author:  Dirk McCormick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * This view contains a list of all alarm types available to be configured. This view must communicate
  * with the MessageTypeDetailView as this will show the details of the item selected in this view.
  */


#pragma warning(disable:4786)

#include "core/utilities/src/RunParams.h"
#include "core/data_access_interface/src/IConfigItem.h"
#include "core/data_access_interface/src/IConfigMessageType.h"

#include "app/configuration/config_plugins/message_type/src/StdAfx.h"
#include "app/configuration/config_plugins/message_type/src/MessageTypeListView.h"
#include "app/configuration/config_plugins/message_type/src/MessageTypeComponent.h"
#include "app/configuration/config_plugins/message_type/src/resource.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/IDatabaseAccessor.h"

#include "bus/resources/resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_Base_App
{
    IMPLEMENT_DYNCREATE(MessageTypeListView, CListView)


    MessageTypeListView::MessageTypeListView()
    {
        m_componentIcon = LoadIcon(GetModuleHandle(MessageTypeComponent::DLL_NAME), MAKEINTRESOURCE(IDI_CONFIG_MESSAGE_TYPE) );
    }


    MessageTypeListView::~MessageTypeListView()
    {
    }


BEGIN_MESSAGE_MAP(MessageTypeListView, AbstractListView)
	//{{AFX_MSG_MAP(MessageTypeListView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    void MessageTypeListView::OnDraw(CDC* pDC)
    {
	    CDocument* pDoc = GetDocument();
    }


    /////////////////////////////////////////////////////////////////////////////
    // MessageTypeListView diagnostics

    #ifdef _DEBUG
    void MessageTypeListView::AssertValid() const
    {
	    AbstractListView::AssertValid();
    }

    void MessageTypeListView::Dump(CDumpContext& dc) const
    {
	    AbstractListView::Dump(dc);
    }
    #endif //_DEBUG

 
}

