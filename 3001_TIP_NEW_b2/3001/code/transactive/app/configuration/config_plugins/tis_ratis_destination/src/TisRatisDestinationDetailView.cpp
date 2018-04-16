/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/configuration/config_plugins/tis_ratis_destination/src/TisRatisDestinationDetailView.cpp $
  * @author:  James
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * This view allows the user to configure an Tis Ratis Destination's details.
  */

#pragma warning(disable:4786 4284)

#include <crtdbg.h>

#include "StdAfx.h"
#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"
#include "core/data_access_interface/stis_manager/src/IConfigTiRatisDestination.h"

#include "bus/generic_gui/src/TransactiveMessage.h" // TD14164
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/src/DatabaseKey.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/FieldValidityChecker.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::RunParams;
using TA_Base_Core::DebugUtil;
using  namespace TA_Base_App::TA_Configuration;
using namespace TA_IRS_Core;

namespace TA_IRS_App
{
    // The titles for the change dialog box
    const int TisRatisDestinationDetailView::DESTINATION_FIELD_LENGTH           = 3;
        
    IMPLEMENT_DYNCREATE(TisRatisDestinationDetailView, CFormView)


    TisRatisDestinationDetailView::TisRatisDestinationDetailView() : 
        m_configItem(NULL),
        m_listView(NULL),
        m_databaseAccessor(NULL),
        AbstractDetailView(TisRatisDestinationDetailView::IDD)
    {
	    //{{AFX_DATA_INIT(TisRatisDestinationDetailView)
	//}}AFX_DATA_INIT
    }


    TisRatisDestinationDetailView::~TisRatisDestinationDetailView()
    {
    }


    void TisRatisDestinationDetailView::DoDataExchange(CDataExchange* pDX)
    {
	    CFormView::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(TisRatisDestinationDetailView)
		//TD18055 marc_bugfix support for destination mandatory marker image
		DDX_Control(pDX, IDC_DESTINATION_BITMAP, m_destinationBitmap); 
    	DDX_Control(pDX, IDC_BUTTON_APPLY, m_applyBtn); 
	    DDX_Control(pDX, IDC_EDIT_DESTINATION, m_destinationEdt);
    	//}}AFX_DATA_MAP
    }


BEGIN_MESSAGE_MAP(TisRatisDestinationDetailView, AbstractDetailView)
	//{{AFX_MSG_MAP(TisRatisDestinationDetailView)
	ON_BN_CLICKED(IDC_BUTTON_RESET, onButtonReset)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, onButtonApply)

    ON_EN_KILLFOCUS(IDC_EDIT_DESTINATION, onKillfocusDestinationEdt)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    void TisRatisDestinationDetailView::OnInitialUpdate() 
    {
        FUNCTION_ENTRY("OnInitialUpdate");

	    CFormView::OnInitialUpdate();

        m_destinationEdt.SetLimitText(DESTINATION_FIELD_LENGTH);

        // Hide all the Tis Ratis Destination details until we are told to populate.
	    showItemDetails(true);

        FUNCTION_EXIT;
    }


    void TisRatisDestinationDetailView::showItemDetails(bool isShown)
    {        
        // Show or hide all the controls from the view
        CWnd* nextChild = GetWindow(GW_CHILD);

        while (0 != nextChild)
        {
            nextChild->ShowWindow(isShown);
    
            nextChild = nextChild->GetWindow(GW_HWNDNEXT);
        }
    }


    void TisRatisDestinationDetailView::populateItemDetails(TA_Base_Core::IConfigItem* currentItem)
    {
        FUNCTION_ENTRY("populateItemDetails");

        if (m_databaseAccessor == NULL || m_listView == NULL)
        {
            m_databaseAccessor = dynamic_cast<TisRatisDestinationDatabaseAccessor*>(getDatabaseAccessor());
            m_listView = dynamic_cast<TisRatisDestinationListView*>(getListView());

            TA_ASSERT(m_databaseAccessor != NULL, "setDatabaseAccessor() must be called before this method");
            TA_ASSERT(m_listView != NULL, "The list view has not been set yet or is not a proper TisRatisDestinationDetailView");
        }


        m_configItem = dynamic_cast<TA_IRS_Core::IConfigTiRatisDestination*>(currentItem);

        if (m_configItem == NULL)
        { 
            showItemDetails(false);
            FUNCTION_EXIT;
            return;
        }

        try
        {
            //  Now populate all the fields with the data retrieved from the IConfigTisRatisDestination* object
            CString buffer;

            m_destinationEdt.SetWindowText(m_configItem->getDestination().c_str());         

            showItemDetails(true);
            refreshButtonAvailability();

        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Cannot retrieve Tis Ratis Destination details to display");
            
            //UE-030001
			// TD14164 ++
            /*MessageBox("Unable to retrieve configuration information. Please try again, and if problem persists, contact support.",
                        RunParams::getInstance().get(RPARAM_APPNAME).c_str(),MB_OK|MB_ICONSTOP);*/
			TA_Base_Bus::TransActiveMessage userMsg;
			userMsg.showMsgBox(IDS_UE_030001);
			// TD14164 ++
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Cannot retrieve Tis Ratis Destination details to display");

            //UE-010001
            CString errorMsg("Database cannot be accessed by the ");
            errorMsg += RunParams::getInstance().get(RPARAM_APPNAME).c_str();
            errorMsg += ".";
			// TD14164 ++
            /*MessageBox(errorMsg,RunParams::getInstance().get(RPARAM_APPNAME).c_str(),MB_OK|MB_ICONSTOP);*/
			TA_Base_Bus::TransActiveMessage userMsg;
			CString fieldName = "Tis Ratis Destination";
			userMsg << fieldName;
			userMsg << errorMsg;
			userMsg.showMsgBox(IDS_UE_030067);
			// ++ TD14164 
        }

        FUNCTION_EXIT;
    }

    void TisRatisDestinationDetailView::refreshButtonAvailability()
    {
        if (m_configItem == NULL)
        {
            return;
        }

        if (m_configItem->isNew() )
        {
            GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(false);
            GetDlgItem(IDC_BUTTON_APPLY)->EnableWindow(true);
            return;
        }
        
        // We have an old item
        if (m_configItem->hasChanged())
        {
            GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(true);
            GetDlgItem(IDC_BUTTON_APPLY)->EnableWindow(true);
        }
        else
        {
            GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(false);
            GetDlgItem(IDC_BUTTON_APPLY)->EnableWindow(false);
        }
    } 


    void TisRatisDestinationDetailView::onButtonReset() 
    {
        FUNCTION_ENTRY("onButtonReset");

        resetButtonPressed(m_configItem);

        FUNCTION_EXIT;
    }


    void TisRatisDestinationDetailView::onButtonApply() 
    {
	    FUNCTION_ENTRY("onButtonApply");

        // Under normal circumstances, the onKillfocus*() methods should be called to update the underlying 
        // IConfig objects.  However, when the operator presses the 'Enter' key, this method is called without
        // first calling onKillfocus*() for the previous widget.  This following line should force the implicit
        // call to the onKillfocus*() method.  
        m_applyBtn.SetFocus();

        if (!m_destinationEdt.isTextValid())
        {
            //UE-030060
			// TD14164 ++
            /*MessageBox("The item could not be applied because invalid names still exist in the fields.",
                       RunParams::getInstance().get(RPARAM_APPNAME).c_str(), MB_OK|MB_ICONSTOP);*/
			TA_Base_Bus::TransActiveMessage userMsg;
			CString reason = "invalid names still exist in the fields";
			userMsg << reason;
			userMsg.showMsgBox(IDS_UE_030068);
			// ++ TD14164 
            return;
        }
        applyButtonPressed(m_configItem);

        // Force this call so our status column updates
        m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(), "");

        FUNCTION_EXIT;
    }

    void TisRatisDestinationDetailView::onKillfocusDestinationEdt()
    {
        FUNCTION_ENTRY("onKillfocusDestinationEdt");
        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        if (!m_configItem) 
        {
            // For the odd occasion that the operator has clicked new item straight after modifying
            // this field
            return;
        }

        CString destinationString;
        m_destinationEdt.GetWindowText(destinationString);

        try
        {
            if (destinationString == m_configItem->getDestination().c_str())
            {
                m_destinationEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
                FUNCTION_EXIT;
                return;
            }

            m_configItem->setDestination( (LPCTSTR) destinationString );
            m_destinationEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database","The Tis Ratis Destination could not be found in the database");

            m_destinationEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );

            FUNCTION_EXIT;
            return;
        }

	    m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
        refreshButtonAvailability();

        FUNCTION_EXIT;
    }

    /////////////////////////////////////////////////////////////////////////////
    // TisRatisDestinationDetailView diagnostics

    #ifdef _DEBUG
    void TisRatisDestinationDetailView::AssertValid() const
    {
	    AbstractDetailView::AssertValid();
    }

    void TisRatisDestinationDetailView::Dump(CDumpContext& dc) const
    {
	    AbstractDetailView::Dump(dc);
    }
    #endif //_DEBUG

}