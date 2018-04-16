/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/configuration/config_plugins/radio_itsi_groups/src/ITSIGroupsListView.cpp $
  * @author:  Derrick Liew
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This view contains a list of all ITSI groups available to be configured. This view must communicate
  * with the ITSIGroupsDetailView as this will show the details of the item selected in this view.
  */


#pragma warning(disable:4786)

#include "app/configuration/config_plugins/radio_itsi_groups/src/StdAfx.h"
#include "app/configuration/config_plugins/radio_itsi_groups/src/ITSIGroupsListView.h"
#include "app/configuration/config_plugins/radio_itsi_groups/src/ITSIGroupsComponent.h"
#include "app/configuration/config_plugins/radio_itsi_groups/src/resource.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/IDatabaseAccessor.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/HelperResource.h"

#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"

#include "core/data_access_interface/src/IConfigItem.h"
#include "core/data_access_interface/radio/src/ConfigITSIGroup.h"

#include "bus/resources/resource.h"

#include "bus/generic_gui/src/TransActiveMessage.h"

#include "core/utilities/src/RunParams.h"

// ITSIGroupsListView.cpp: implementation of the ITSIGroupsListView class.
//
//////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

using namespace TA_Base_Core;
using namespace TA_Base_App::TA_Configuration;

namespace TA_IRS_App
{
    IMPLEMENT_DYNCREATE(ITSIGroupsListView, CListView)


	ITSIGroupsListView::ITSIGroupsListView()
	{
        m_componentIcon = LoadIcon(GetModuleHandle(ITSIGroupsComponent::DLL_NAME), MAKEINTRESOURCE(IDI_CONFIG_PROFILE) );
	}

	ITSIGroupsListView::~ITSIGroupsListView()
	{

	}

	BEGIN_MESSAGE_MAP(ITSIGroupsListView, AbstractListView)
		//{{AFX_MSG_MAP(ITSIGroupsListView)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

    void ITSIGroupsListView::OnDraw(CDC* pDC)
    {
	    CDocument* pDoc = GetDocument();
    }


    void ITSIGroupsListView::deleteItem()
    {
        FUNCTION_ENTRY("deleteItem");

        POSITION pos = GetListCtrl().GetFirstSelectedItemPosition();
        CString actionName;
        actionName.LoadString(IDS_DELETE);
        if (pos == NULL)
        {
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << actionName;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_030059);
            FUNCTION_EXIT;
            return;
        }

        int numberOfItemsNotDeleted = 0;
        CString errorMsg = "The item could not be deleted because it is used in ITSI config. The items preventing the delete are listed below:\n";

        while(pos != NULL)
        {
            int selected = GetListCtrl().GetNextSelectedItem(pos);
            unsigned long itemId = GetListCtrl().GetItemData( selected );

            TA_Base_Core::IConfigItem* item = getDatabaseAccessor()->getItem(itemId);
            TA_Base_Core::ConfigITSIGroup* ITSIGroup = dynamic_cast<TA_Base_Core::ConfigITSIGroup*>(item);
			
			// check whether it it used in the ITSI and it should not be new
			bool canBeDeleted = false;
			// this case is a new ITSI group
			if ( ITSIGroup != NULL && ITSIGroup->isNew() ) canBeDeleted = true; 
			// this case is not a new group but may be used in ITSI Table
			else if ( ITSIGroup != NULL && ITSIGroup->isGroupUsedInProfile(ITSIGroup->getKey()) == false ) canBeDeleted = true;
            
			if ( canBeDeleted == false )
            {
                if (numberOfItemsNotDeleted < MAX_SIZE_OF_LIST_IN_ERROR_MESSAGE)
                {
                    errorMsg += ITSIGroup->getName().c_str();
                    errorMsg += _T("\n");
                    ++numberOfItemsNotDeleted;
                }
                else
                {
                    CString more;
                    more.LoadString(IDS_MORE);
                    errorMsg += more;
                    //  Increment this again so we can tell outside this for-loop whether the
                    // ... has been added yet. This avoids the problem of having exactly
                    // MAX_SIZE_OF_LIST_IN_ERROR_MESSAGE after this loop and breaking even though
                    // there are more items
                    ++numberOfItemsNotDeleted;
                    break;
                }
            }
        }

        // If we have any entries that are invalid we must prevent the delete
        if (numberOfItemsNotDeleted != 0)
        {
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << actionName << errorMsg;
            userMsg.showMsgBox(IDS_UE_030060);

            FUNCTION_EXIT;
            return;
        }

        CString itemType;
        itemType.LoadString(IDS_ITSI_GROUPS);
        TA_Base_Bus::TransActiveMessage userMsg;
        userMsg << itemType;
        UINT selectedButton = userMsg.showMsgBox(IDS_UW_030019);

        AbstractListView::deleteItem();

        FUNCTION_EXIT;
    }


    void ITSIGroupsListView::copyItem()
    {
        FUNCTION_ENTRY("copyItem");

        CString actionName;
        actionName.LoadString(IDS_COPY);

        POSITION pos = GetListCtrl().GetFirstSelectedItemPosition();
        if (pos == NULL)
        {
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << actionName;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_030059);
            FUNCTION_EXIT;
            return;
        }

		//There is no need to check for validity as any ITSI group can be duplicated

        AbstractListView::copyItem();

        FUNCTION_EXIT;
    }

    /////////////////////////////////////////////////////////////////////////////
    // ITSIListView diagnostics

    #ifdef _DEBUG
    void ITSIGroupsListView::AssertValid() const
    {
	    AbstractListView::AssertValid();
    }

    void ITSIGroupsListView::Dump(CDumpContext& dc) const
    {
	    AbstractListView::Dump(dc);
    }
    #endif //_DEBUG

 } //end of namespace