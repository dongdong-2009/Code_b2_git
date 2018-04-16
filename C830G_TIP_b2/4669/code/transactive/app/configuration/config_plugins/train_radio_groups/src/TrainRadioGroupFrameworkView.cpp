/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_4669_TRAIN_ECP/CODE_4669/transactive/app/configuration/config_plugins/train_radio_groups/src/TrainRadioGroupFrameworkView.cpp $
  * @author:  oliverk
  * @version: $Revision: #1 $
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  * This view contains a splitter. It then loads the left side of the splitter as a list of train location
  * and the right side as a view for the user to configure train radio groups details.
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/train_radio_groups/src/stdafx.h"
#include "app/configuration/config_plugins/train_radio_groups/src/TrainRadioGroupFrameworkView.h"
#include "app/configuration/config_plugins/train_radio_groups/src/TrainRadioGroupDetailView.h"
#include "app/configuration/config_plugins/train_radio_groups/src/TrainRadioGroupListView.h"

#include "core/utilities/src/DebugUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::DebugUtil;

namespace TA_IRS_App
{
    
    IMPLEMENT_DYNCREATE( TrainRadioGroupFrameworkView, CView )


    TrainRadioGroupFrameworkView::TrainRadioGroupFrameworkView()
    {
        FUNCTION_ENTRY( "TrainRadioGroupFrameworkView" );
        FUNCTION_EXIT;
    }


    TrainRadioGroupFrameworkView::~TrainRadioGroupFrameworkView()
    {
        FUNCTION_ENTRY( "~TrainRadioGroupFrameworkView" );
        FUNCTION_EXIT;
    }


    BEGIN_MESSAGE_MAP( TrainRadioGroupFrameworkView, AbstractFrameworkView )
        //{{AFX_MSG_MAP(TrainRadioGroupFrameworkView)
        //}}AFX_MSG_MAP
    END_MESSAGE_MAP()


    CRuntimeClass* TrainRadioGroupFrameworkView::getRuntimeClassForListView()
    {
        FUNCTION_ENTRY( "getRuntimeClassForListView" );
        FUNCTION_EXIT;
        return RUNTIME_CLASS( TrainRadioGroupListView );
    }


    CRuntimeClass* TrainRadioGroupFrameworkView::getRuntimeClassForDetailView()
    {
        FUNCTION_ENTRY( "getRuntimeClassForDetailView" );
        FUNCTION_EXIT;
        return RUNTIME_CLASS( TrainRadioGroupDetailView );
    }


#ifdef _DEBUG
    void TrainRadioGroupFrameworkView::AssertValid() const
    {
        FUNCTION_ENTRY( "AssertValid" );
        FUNCTION_EXIT;
        AbstractFrameworkView::AssertValid();
    }


    void TrainRadioGroupFrameworkView::Dump( CDumpContext& dc ) const
    {
        FUNCTION_ENTRY( "Dump" );
        FUNCTION_EXIT;
        AbstractFrameworkView::Dump( dc );
    }

#endif //_DEBUG

}