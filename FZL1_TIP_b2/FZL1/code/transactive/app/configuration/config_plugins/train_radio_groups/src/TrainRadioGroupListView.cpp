/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_4669_TRAIN_ECP/CODE_4669/transactive/app/configuration/config_plugins/train_radio_groups/src/TrainRadioGroupListView.cpp $
  * @author:  oliverk
  * @version: $Revision: #1$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  * This view contains a list of all train radio grups available to be configured. This view must communicate
  * with the TrainRadioGroupsDetailView as this will show the details of the item selected in this view.
  */

#include "app/configuration/config_plugins/train_radio_groups/src/TrainRadioGroupListView.h"
#include "app/configuration/config_plugins/train_radio_groups/src/TrainRadioGroupComponent.h"
#include "app/configuration/config_plugins/train_radio_groups/src/resource.h"

#include "bus/resources/resource.h"

#include "core/utilities/src/DebugUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


namespace TA_IRS_App
{
    
    
    IMPLEMENT_DYNCREATE( TrainRadioGroupListView, CListView )


    TrainRadioGroupListView::TrainRadioGroupListView()
    {
        FUNCTION_ENTRY( "TrainRadioGroupListView" );
        
        m_componentIcon = LoadIcon( GetModuleHandle( TrainRadioGroupComponent::DLL_NAME ), MAKEINTRESOURCE( IDI_CONFIG_SI_REGION_TALKGOUP ) );

        FUNCTION_EXIT;
    }


    TrainRadioGroupListView::~TrainRadioGroupListView()
    {
        FUNCTION_ENTRY( "~TrainRadioGroupListView" );
        FUNCTION_EXIT;
    }


    BEGIN_MESSAGE_MAP( TrainRadioGroupListView, AbstractListView )
        //{{AFX_MSG_MAP(TrainRadioGroupListView)
        //}}AFX_MSG_MAP
    END_MESSAGE_MAP()


    void TrainRadioGroupListView::OnDraw( CDC* pDC )
    {
        FUNCTION_ENTRY( "OnDraw" );

        CDocument* pDoc = GetDocument();

        FUNCTION_EXIT;
    }


#ifdef _DEBUG

    void TrainRadioGroupListView::AssertValid() const
    {
        FUNCTION_ENTRY( "AssertValid" );

        AbstractListView::AssertValid();

        FUNCTION_EXIT;
    }


    void TrainRadioGroupListView::Dump( CDumpContext& dc ) const
    {
        FUNCTION_ENTRY( "Dump" );

        AbstractListView::Dump( dc );

        FUNCTION_EXIT;
    }

#endif //_DEBUG


    HICON TrainRadioGroupListView::getComponentIcon()
    {
        FUNCTION_ENTRY( "getComponentIcon" );
        FUNCTION_EXIT;
        return m_componentIcon;
    }


    TA_Base_App::MessageSender::ConfigUpdateType TrainRadioGroupListView::getConfigMessageType() const
    {
        FUNCTION_ENTRY( "getConfigMessageType" );
        FUNCTION_EXIT;
        return TA_Base_App::MessageSender::NOT_APPLICABLE;
    }

}