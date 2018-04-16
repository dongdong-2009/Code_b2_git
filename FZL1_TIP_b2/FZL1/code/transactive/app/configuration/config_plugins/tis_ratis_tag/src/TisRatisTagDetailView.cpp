/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/configuration/config_plugins/tis_ratis_tag/src/TisRatisTagDetailView.cpp $
  * @author:  James
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view allows the user to configure an Tis Ratis Tag's details.
  */

#pragma warning(disable:4786 4284)

#include <crtdbg.h>

#include "StdAfx.h"
#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"
#include "core/data_access_interface/stis_manager/src/IConfigTiRatisTag.h"

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
    const int TisRatisTagDetailView::TAG_GROUP_FIELD_LENGTH           = 3;
    const int TisRatisTagDetailView::DETAIL_FIELD_LENGTH              = 5;
        
    IMPLEMENT_DYNCREATE(TisRatisTagDetailView, CFormView)


    TisRatisTagDetailView::TisRatisTagDetailView() : 
        m_configItem(NULL),
        m_listView(NULL),
        m_databaseAccessor(NULL),
        AbstractDetailView(TisRatisTagDetailView::IDD)
    {
	    //{{AFX_DATA_INIT(TisRatisTagDetailView)
	//}}AFX_DATA_INIT
    }


    TisRatisTagDetailView::~TisRatisTagDetailView()
    {
    }


    void TisRatisTagDetailView::DoDataExchange(CDataExchange* pDX)
    {
	    CFormView::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(TisRatisTagDetailView)
    	DDX_Control(pDX, IDC_BUTTON_APPLY, m_applyBtn); 
	    DDX_Control(pDX, IDC_EDIT_TAG_GROUP, m_tagGroupEdt);
        DDX_Control(pDX, IDC_EDIT_DETAIL, m_detailEdt);
    	//}}AFX_DATA_MAP
    }


BEGIN_MESSAGE_MAP(TisRatisTagDetailView, AbstractDetailView)
	//{{AFX_MSG_MAP(TisRatisTagDetailView)
	ON_BN_CLICKED(IDC_BUTTON_RESET, onButtonReset)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, onButtonApply)

    ON_EN_KILLFOCUS(IDC_EDIT_TAG_GROUP, onKillfocusTagGroupEdt)
    ON_EN_KILLFOCUS(IDC_EDIT_DETAIL, onKillfocusDetailEdt)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    void TisRatisTagDetailView::OnInitialUpdate() 
    {
        FUNCTION_ENTRY("OnInitialUpdate");

	    CFormView::OnInitialUpdate();

        m_tagGroupEdt.SetLimitText(TAG_GROUP_FIELD_LENGTH);
        m_detailEdt.SetLimitText(DETAIL_FIELD_LENGTH);

        // Hide all the Tis Ratis Tag details until we are told to populate.
	    showItemDetails(true);

        FUNCTION_EXIT;
    }


    void TisRatisTagDetailView::showItemDetails(bool isShown)
    {        
        // Show or hide all the controls from the view
        CWnd* nextChild = GetWindow(GW_CHILD);

        while (0 != nextChild)
        {
            nextChild->ShowWindow(isShown);
    
            nextChild = nextChild->GetWindow(GW_HWNDNEXT);
        }
    }


    void TisRatisTagDetailView::populateItemDetails(TA_Base_Core::IConfigItem* currentItem)
    {
        FUNCTION_ENTRY("populateItemDetails");

        if (m_databaseAccessor == NULL || m_listView == NULL)
        {
            m_databaseAccessor = dynamic_cast<TisRatisTagDatabaseAccessor*>(getDatabaseAccessor());
            m_listView = dynamic_cast<TisRatisTagListView*>(getListView());

            TA_ASSERT(m_databaseAccessor != NULL, "setDatabaseAccessor() must be called before this method");
            TA_ASSERT(m_listView != NULL, "The list view has not been set yet or is not a proper TisRatisTagDetailView");
        }


        m_configItem = dynamic_cast<TA_IRS_Core::IConfigTiRatisTag*>(currentItem);

        if (m_configItem == NULL)
        { 
            showItemDetails(false);
            FUNCTION_EXIT;
            return;
        }

        try
        {
            //  Now populate all the fields with the data retrieved from the IConfigTisRatisTag* object
            CString buffer;

            m_tagGroupEdt.SetWindowText(m_configItem->getTagGroup().c_str());
            m_detailEdt.SetWindowText(m_configItem->getDetail().c_str());           

            showItemDetails(true);
            refreshButtonAvailability();

        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Cannot retrieve Tis Ratis Tag details to display");
            
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
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Cannot retrieve Tis Ratis Tag details to display");

            //UE-010001
            CString errorMsg("Database cannot be accessed by the ");
            errorMsg += RunParams::getInstance().get(RPARAM_APPNAME).c_str();
            errorMsg += ".";
            /*MessageBox(errorMsg,RunParams::getInstance().get(RPARAM_APPNAME).c_str(),MB_OK|MB_ICONSTOP);*/
			// TD14164 ++
			TA_Base_Bus::TransActiveMessage userMsg;
			CString fieldName = "Tis Ratis Tag";
			userMsg << fieldName;
			userMsg << errorMsg;
			userMsg.showMsgBox(IDS_UE_030067);
			// ++ TD14164 
        }

        FUNCTION_EXIT;
    }

    void TisRatisTagDetailView::refreshButtonAvailability()
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


    void TisRatisTagDetailView::onButtonReset() 
    {
        FUNCTION_ENTRY("onButtonReset");

        resetButtonPressed(m_configItem);

        FUNCTION_EXIT;
    }


    void TisRatisTagDetailView::onButtonApply() 
    {
	    FUNCTION_ENTRY("onButtonApply");

        // Under normal circumstances, the onKillfocus*() methods should be called to update the underlying 
        // IConfig objects.  However, when the operator presses the 'Enter' key, this method is called without
        // first calling onKillfocus*() for the previous widget.  This following line should force the implicit
        // call to the onKillfocus*() method.  
        m_applyBtn.SetFocus();

        if (!m_tagGroupEdt.isTextValid() || 
            !m_detailEdt.isTextValid())
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
        else if (m_tagGroupEdt)
        {
            CString tempCStringContainer;

            m_tagGroupEdt.GetWindowText(tempCStringContainer);
            if (tempCStringContainer.GetLength() != 3)
            {
				// TD14164 ++
				/*MessageBox("The item could not be applied because invalid names still exist in the fields.");*/				
				TA_Base_Bus::TransActiveMessage userMsg;
				CString reason = "invalid names still exist in the fields";
				userMsg << reason;
				userMsg.showMsgBox(IDS_UE_030068);
				// ++ TD14164 
                return;
            }

            m_detailEdt.GetWindowText(tempCStringContainer);
            if (tempCStringContainer.GetLength() != 5)
            {
				// TD14164 ++
				/*MessageBox("The item could not be applied because invalid names still exist in the fields.");*/
				TA_Base_Bus::TransActiveMessage userMsg;
				CString reason = "invalid names still exist in the fields";
				userMsg << reason;
				userMsg.showMsgBox(IDS_UE_030068);
				// ++ TD14164 
                return;
            }
        }
        try
        {
            applyButtonPressed(m_configItem);
        }
        catch (...) {
			// TD14164 ++
			// Comment-out AfxMessageBox call and replace with logging
			// Cannot use TransactiveMessage here because of insufficient details
            /*AfxMessageBox("test");*/
			LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception","Caught unknown exception");
			// ++ TD14164 
        }

        // Force this call so our status column updates
        m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(), "");

        FUNCTION_EXIT;
    }

    void TisRatisTagDetailView::onKillfocusTagGroupEdt()
    {
        FUNCTION_ENTRY("onKillfocusTagGroupEdt");
        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        if (!m_configItem) 
        {
            // For the odd occasion that the operator has clicked new item straight after modifying
            // this field
            return;
        }

        CString tagGroupString;
        m_tagGroupEdt.GetWindowText(tagGroupString);

        try
        {
            if (tagGroupString == m_configItem->getTagGroup().c_str())
            {
                m_tagGroupEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
                FUNCTION_EXIT;
                return;
            }

            m_configItem->setTagGroup( (LPCTSTR) tagGroupString );
            m_tagGroupEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database","The Tis Ratis Tag could not be found in the database");

            m_tagGroupEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );

            FUNCTION_EXIT;
            return;
        }

	    m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
        refreshButtonAvailability();

        FUNCTION_EXIT;
    }

    void TisRatisTagDetailView::onKillfocusDetailEdt()
    {
        FUNCTION_ENTRY("onKillfocusDetailEdt");
        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        if (!m_configItem) 
        {
            // For the odd occasion that the operator has clicked new item straight after modifying
            // this field
            return;
        }

        CString detailString;
        m_detailEdt.GetWindowText(detailString);

        try
        {
            if (detailString == m_configItem->getDetail().c_str())
            {                
                m_detailEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
                FUNCTION_EXIT;
                return;
            }

            m_configItem->setDetail( (LPCTSTR) detailString );
            m_detailEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database","The Tis Ratis Tag could not be found in the database");

            m_detailEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );

            FUNCTION_EXIT;
            return;
        }

	    m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
        refreshButtonAvailability();

        FUNCTION_EXIT;
    }

    /////////////////////////////////////////////////////////////////////////////
    // TisRatisTagDetailView diagnostics

    #ifdef _DEBUG
    void TisRatisTagDetailView::AssertValid() const
    {
	    AbstractDetailView::AssertValid();
    }

    void TisRatisTagDetailView::Dump(CDumpContext& dc) const
    {
	    AbstractDetailView::Dump(dc);
    }
    #endif //_DEBUG

}