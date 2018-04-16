/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/configuration/config_plugins/ats_branch_track/src/AtsBranchTrackDetailView.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * This view allows the user to configure an Ats Branch Track's details.
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/ats_branch_track/src/StdAfx.h"
#include "app/configuration/config_plugins/ats_branch_track/src/AtsBranchTrackDetailView.h"
#include "app/configuration/config_plugins/ats_branch_track/src/AtsBranchTrackDatabaseAccessor.h"
#include "app/configuration/config_plugins/ats_branch_track/src/AtsBranchTrackListView.h"
#include "app/configuration/config_plugins/ats_branch_track/src/AtsBranchTrackComponent.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/ChangeDialog.h"
#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"
#include "core/data_access_interface/ats/src/IConfigAtsBranchTrack.h"
#include "core/data_access_interface/ats/src/AtsBranchTrack.h"

#include "bus/generic_gui/src/TransactiveMessage.h" // TD14164
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::RunParams;
using TA_Base_Core::DebugUtil;
using namespace TA_Base_App::TA_Configuration;

namespace TA_IRS_App
{
    const int AtsBranchTrackDetailView::ATS_BRANCH_ID_FIELD_LENGTH = 10;
    
    IMPLEMENT_DYNCREATE(AtsBranchTrackDetailView, CFormView)

    AtsBranchTrackDetailView::AtsBranchTrackDetailView() : 
        m_configItem(NULL),
        m_listView(NULL),
        m_databaseAccessor(NULL),
        AbstractDetailView(AtsBranchTrackDetailView::IDD)
    {
	    //{{AFX_DATA_INIT(AtsBranchTrackDetailView)
	//}}AFX_DATA_INIT
    }


    AtsBranchTrackDetailView::~AtsBranchTrackDetailView()
    {
    }


    void AtsBranchTrackDetailView::DoDataExchange(CDataExchange* pDX)
    {
	    CFormView::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(AtsBranchTrackDetailView)
	DDX_Control(pDX, IDC_EDIT_ATS_BRANCH_ID, m_branchIdEdt);
	DDX_Control(pDX, IDC_BUTTON_APPLY, m_applyBtn);
	DDX_Control(pDX, IDC_COMBO_TRACK, m_trackCombo);
	DDX_Control(pDX, IDC_STATIC_TYPE, m_type);
	//}}AFX_DATA_MAP
    }


BEGIN_MESSAGE_MAP(AtsBranchTrackDetailView, AbstractDetailView)
	//{{AFX_MSG_MAP(AtsBranchTrackDetailView)
	ON_BN_CLICKED(IDC_BUTTON_RESET, onButtonReset)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, onButtonApply)
	ON_EN_KILLFOCUS(IDC_EDIT_ATS_BRANCH_ID, onKillfocusEditBranchId)
	ON_CBN_KILLFOCUS(IDC_COMBO_TRACK, onKillfocusComboTrack)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    void AtsBranchTrackDetailView::OnInitialUpdate() 
    {
        FUNCTION_ENTRY("OnInitialUpdate");

	    CFormView::OnInitialUpdate();
	    
        m_branchIdEdt.SetLimitText(ATS_BRANCH_ID_FIELD_LENGTH);
        m_trackCombo.AddString(TA_IRS_Core::AtsBranchTrack::TRACK_INNERBOUND.c_str());
		m_trackCombo.AddString(TA_IRS_Core::AtsBranchTrack::TRACK_OUTERBOUND.c_str());
		//TD14463
		m_trackCombo.AddString(TA_IRS_Core::AtsBranchTrack::TRACK_CENTRAL_INNERBOUND.c_str());
		m_trackCombo.AddString(TA_IRS_Core::AtsBranchTrack::TRACK_CENTRAL_OUTERBOUND.c_str());

        // Hide all the Ats Branch Track details until we are told to populate.
	    showItemDetails(false);

        FUNCTION_EXIT;
    }


    void AtsBranchTrackDetailView::showItemDetails(bool isShown)
    {
        // Show or hide all the controls from the view

        m_branchIdEdt.ShowWindow(isShown);
        m_trackCombo.ShowWindow(isShown);
        m_type.ShowWindow(isShown);

		GetDlgItem(IDC_BRANCH_STATIC)->ShowWindow(isShown);
		GetDlgItem(IDC_TRACK_STATIC)->ShowWindow(isShown);
		GetDlgItem(IDC_BUTTON_APPLY)->ShowWindow(isShown);
		GetDlgItem(IDC_BUTTON_RESET)->ShowWindow(isShown);
		GetDlgItem(IDC_BRANCH_BITMAP)->ShowWindow(isShown);
		GetDlgItem(IDC_TRACK_BITMAP)->ShowWindow(isShown);
    }


    void AtsBranchTrackDetailView::populateItemDetails(TA_Base_Core::IConfigItem* currentItem)
    {
        FUNCTION_ENTRY("populateItemDetails");

        if (m_databaseAccessor == NULL || m_listView == NULL)
        {
            m_databaseAccessor = dynamic_cast<AtsBranchTrackDatabaseAccessor*>(getDatabaseAccessor());
            m_listView = dynamic_cast<AtsBranchTrackListView*>(getListView());

            TA_ASSERT(m_databaseAccessor != NULL, "setDatabaseAccessor() must be called before this method");
            TA_ASSERT(m_listView != NULL, "The list view has not been set yet or is not a proper AtsBranchTrackDetailView");
        }


        m_configItem = dynamic_cast<TA_IRS_Core::IConfigAtsBranchTrack*>(currentItem);

        if (m_configItem == NULL)
        { 
            showItemDetails(false);
            FUNCTION_EXIT;
            return;
        }

        try
        {
            //  Now populate all the fields with the data retrieved from the IConfigAtsBranchTrack* object

            if (m_configItem->getAtsBranchId() == "")
            {
                m_branchIdEdt.SetWindowText( "" );
                m_branchIdEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
                m_branchIdEdt.EnableWindow(true);
            }
            else
            {
                CString buffer = m_configItem->getAtsBranchId().c_str();
                m_branchIdEdt.SetWindowText( buffer );
                m_branchIdEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
                m_branchIdEdt.EnableWindow(m_configItem->isNew());
            }
            
			m_trackCombo.SetCurSel(m_trackCombo.FindStringExact(-1, m_configItem->getTrack().c_str()));

            if( m_configItem->isNew() )
            {

                // This is a new Ats Branch Track or at least a copy of an Ats Branch Track so we indicate this in the type
                // field and make sure the modify and create dates are blank.

                m_type.SetWindowText(NEW_LABEL);
 
                // Disable the reset button as this is a new item so reset has no functionality
                GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(FALSE);
            }
            else
            {

                m_type.SetWindowText( NO_VALUE );

                // Enable the reset button for the user
                GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(TRUE);
            }
            
            showItemDetails(true);
            refreshButtonAvailability();

        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Cannot retrieve Ats Branch Track details to display");
            
            //UE-030001
			// TD14164 ++
            /*MessageBox("Unable to retrieve configuration information. Please try again, and if problem persists, contact support.",
                        RunParams::getInstance().get(RPARAM_APPNAME).c_str(),MB_OK|MB_ICONSTOP);*/
			TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_030001);
			// ++ TD14164 
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Cannot retrieve Ats Branch Track details to display");

            //UE-010001
            CString errorMsg("Database cannot be accessed by the ");
            errorMsg += RunParams::getInstance().get(RPARAM_APPNAME).c_str();
            errorMsg += ".";
			// TD14164 ++
            /*MessageBox(errorMsg,RunParams::getInstance().get(RPARAM_APPNAME).c_str(),MB_OK|MB_ICONSTOP);*/
			TA_Base_Bus::TransActiveMessage userMsg;
			CString fieldName = "Ats Branch Track";
			userMsg << fieldName;
			userMsg << errorMsg;
			userMsg.showMsgBox(IDS_UE_030067);
			// ++ TD14164
        }

        FUNCTION_EXIT;
    }

    void AtsBranchTrackDetailView::refreshButtonAvailability()
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

    void AtsBranchTrackDetailView::onKillfocusEditBranchId() 
    {
        FUNCTION_ENTRY("onKillfocusEditBranchId");
        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        if (!m_configItem) 
        {
            // For the odd occasion that the operator has clicked new item straight after modifying
            // this field
            return;
        }

        CString branchIdString;
        m_branchIdEdt.GetWindowText(branchIdString);
		std::string branchId (branchIdString);

        try
        {
            // If the branchId has not changed then there is no point doing an update
            if (m_configItem->getAtsBranchId() == branchId)
            {
                m_branchIdEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
                FUNCTION_EXIT;
                return;
            }

            m_configItem->setAtsBranchId( branchId );
            m_branchIdEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database","The Ats Branch Track could not be found in the database");

            m_branchIdEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );

            FUNCTION_EXIT;
            return;
        }

	    m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
        refreshButtonAvailability();

        FUNCTION_EXIT;
    }

    void AtsBranchTrackDetailView::onKillfocusComboTrack() 
    {
        FUNCTION_ENTRY("onKillfocusEditTrack");
        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        if (!m_configItem) 
        {
            // For the odd occasion that the operator has clicked new item straight after modifying
            // this field
            return;
        }

        CString trackString;
		
		if (m_trackCombo.GetCurSel() >= 0)
		{
			m_trackCombo.GetLBText(m_trackCombo.GetCurSel(), trackString);
		}
		else
		{
			trackString = "";
		}

        try
        {
            // If the track string has not changed then there is no point doing an update
            if (trackString == m_configItem->getTrack().c_str())
            {
                FUNCTION_EXIT;
                return;
            }

            m_configItem->setTrack( (LPCTSTR) trackString );
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database","The Ats Branch Track could not be found in the database");

            FUNCTION_EXIT;
            return;
        }

	    m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
        refreshButtonAvailability();

        FUNCTION_EXIT;
    }


    void AtsBranchTrackDetailView::onButtonReset() 
    {
        FUNCTION_ENTRY("onButtonReset");

        resetButtonPressed(m_configItem);

        FUNCTION_EXIT;
    }


    void AtsBranchTrackDetailView::onButtonApply() 
    {
	    FUNCTION_ENTRY("onButtonApply");

        // Under normal circumstances, the onKillfocus*() methods should be called to update the underlying 
        // IConfig objects.  However, when the operator presses the 'Enter' key, this method is called without
        // first calling onKillfocus*() for the previous widget.  This following line should force the implicit
        // call to the onKillfocus*() method.  
        m_applyBtn.SetFocus();

        if ( !m_branchIdEdt.isTextValid() )
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


    /////////////////////////////////////////////////////////////////////////////
    // AtsBranchTrackDetailView diagnostics

    #ifdef _DEBUG
    void AtsBranchTrackDetailView::AssertValid() const
    {
	    AbstractDetailView::AssertValid();
    }

    void AtsBranchTrackDetailView::Dump(CDumpContext& dc) const
    {
	    AbstractDetailView::Dump(dc);
    }
    #endif //_DEBUG

}
