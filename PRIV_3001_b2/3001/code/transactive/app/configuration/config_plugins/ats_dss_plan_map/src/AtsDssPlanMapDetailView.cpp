/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/configuration/config_plugins/ats_dss_plan_map/src/AtsDssPlanMapDetailView.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * This view allows the user to configure an DSS Plan Map's details.
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/ats_dss_plan_map/src/StdAfx.h"
#include "app/configuration/config_plugins/ats_dss_plan_map/src/AtsDssPlanMapDetailView.h"
#include "app/configuration/config_plugins/ats_dss_plan_map/src/AtsDssPlanMapDatabaseAccessor.h"
#include "app/configuration/config_plugins/ats_dss_plan_map/src/AtsDssPlanMapListView.h"
#include "app/configuration/config_plugins/ats_dss_plan_map/src/AtsDssPlanMapComponent.h"
#include "app/configuration/config_plugins/ats_dss_plan_map/src/ChangePlanDialog.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/ChangeDialog.h"

#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"

#include "bus/generic_gui/src/TransactiveMessage.h" // TD14164
#include "core/data_access_interface/ats/src/IConfigAtsDssPlanMap.h"

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
    const int AtsDssPlanMapDetailView::PLAN_FIELD_LENGTH = 256;
    const int AtsDssPlanMapDetailView::DSSEVENT_FIELD_LENGTH = 10;
    
    IMPLEMENT_DYNCREATE(AtsDssPlanMapDetailView, CFormView)

    AtsDssPlanMapDetailView::AtsDssPlanMapDetailView() : 
        m_currentAtsDssPlanMap(NULL),
        m_listView(NULL),
        m_databaseAccessor(NULL),
        AbstractDetailView(AtsDssPlanMapDetailView::IDD)
    {
	    //{{AFX_DATA_INIT(AtsDssPlanMapDetailView)
	//}}AFX_DATA_INIT
    }


    AtsDssPlanMapDetailView::~AtsDssPlanMapDetailView()
    {
    }


    void AtsDssPlanMapDetailView::DoDataExchange(CDataExchange* pDX)
    {
	    CFormView::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(AtsDssPlanMapDetailView)
	DDX_Control(pDX, IDC_EDIT_PLAN, m_plan);
	DDX_Control(pDX, IDC_EDIT_DSSEVENT, m_dssEvent);
	DDX_Control(pDX, IDC_PLAN_BITMAP, m_bitmapPlan);
	DDX_Control(pDX, IDC_DSSEVENT_BITMAP, m_bitmapDssEvent);
	DDX_Control(pDX, IDC_STATIC_TYPE, m_type);
	//}}AFX_DATA_MAP
    }


BEGIN_MESSAGE_MAP(AtsDssPlanMapDetailView, AbstractDetailView)
	//{{AFX_MSG_MAP(AtsDssPlanMapDetailView)
	ON_BN_CLICKED(IDC_BUTTON_RESET, onButtonReset)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, onButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_PLAN, onButtonPlan)
	ON_EN_KILLFOCUS(IDC_EDIT_DSSEVENT, onKillfocusEditDssEvent)
	ON_EN_KILLFOCUS(IDC_EDIT_PLAN, onKillfocusEditPlan)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    void AtsDssPlanMapDetailView::OnInitialUpdate() 
    {
        FUNCTION_ENTRY("OnInitialUpdate");

	    CFormView::OnInitialUpdate();
	    
        m_dssEvent.SetLimitText(DSSEVENT_FIELD_LENGTH);
        m_plan.SetLimitText(PLAN_FIELD_LENGTH);

        // Hide all the DSS Plan Map details until we are told to populate.
	    showItemDetails(false);

        FUNCTION_EXIT;
    }


    void AtsDssPlanMapDetailView::showItemDetails(bool isShown)
    {
         // Show or hide all the controls from the view
        GetDlgItem(IDC_STATIC_TYPE)->ShowWindow(isShown);
         
		GetDlgItem(IDC_STATIC_DSSEVENT)->ShowWindow(isShown);
		GetDlgItem(IDC_EDIT_DSSEVENT)->ShowWindow(isShown);
		GetDlgItem(IDC_DSSEVENT_BITMAP)->ShowWindow(isShown);
        

		GetDlgItem(IDC_STATIC_PLAN)->ShowWindow(isShown);
        GetDlgItem(IDC_EDIT_PLAN)->ShowWindow(isShown);
        GetDlgItem(IDC_PLAN_BITMAP)->ShowWindow(isShown);
        GetDlgItem(IDC_BUTTON_PLAN)->ShowWindow(isShown);
         
		GetDlgItem(IDC_BUTTON_APPLY)->ShowWindow(isShown);
        GetDlgItem(IDC_BUTTON_RESET)->ShowWindow(isShown);
    }


    void AtsDssPlanMapDetailView::refreshButtonAvailability()
    {
        if (m_currentAtsDssPlanMap == NULL)
        {
            return;
        }

        if (m_currentAtsDssPlanMap->isNew() )
        {
            GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(false);
            GetDlgItem(IDC_BUTTON_APPLY)->EnableWindow(true);
            return;
        }
        
        // We have an old item
        if( !m_plan.isTextValid() || !m_dssEvent.isTextValid() )
        {
            // If any fields are invalid then enable the reset button but disable the apply button
            GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(true);
            GetDlgItem(IDC_BUTTON_APPLY)->EnableWindow(false);
        }
        else if (m_currentAtsDssPlanMap->hasChanged())
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

    
    void AtsDssPlanMapDetailView::populateItemDetails(TA_Base_Core::IConfigItem* currentItem)
    {
        FUNCTION_ENTRY("populateItemDetails");

        if (m_databaseAccessor == NULL || m_listView == NULL)
        {
            m_databaseAccessor = dynamic_cast<AtsDssPlanMapDatabaseAccessor*>(getDatabaseAccessor());
            m_listView = dynamic_cast<AtsDssPlanMapListView*>(getListView());

            TA_ASSERT(m_databaseAccessor != NULL, "setDatabaseAccessor() must be called before this method");
            TA_ASSERT(m_listView != NULL, "The list view has not been set yet or is not a proper AtsDssPlanMapDetailView");
        }


        m_currentAtsDssPlanMap = dynamic_cast<TA_IRS_Core::IConfigAtsDssPlanMap*>(currentItem);

        if (m_currentAtsDssPlanMap == NULL)
        { 
            showItemDetails(false);
            FUNCTION_EXIT;
            return;
        }

        try
        {
            //  Now populate all the fields with the data retrieved from the IConfigAtsDssPlanMap* object
			std::ostringstream dssEvent;
			dssEvent << m_currentAtsDssPlanMap->getDssEvent();
			m_dssEvent.SetWindowText( dssEvent.str().c_str() );
            m_dssEvent.setTextType( TA_Base_Bus::TextColourEdit::VALID );

            m_plan.SetWindowText( m_currentAtsDssPlanMap->getPlanPath().c_str() );
            m_plan.setTextType( TA_Base_Bus::TextColourEdit::VALID );

            showItemDetails(true);

            refreshButtonAvailability();
        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Cannot retrieve DSS Plan Map details to display");
            
            //UE-030001
			// TD14164 ++
            /*MessageBox("Unable to retrieve configuration information. Please try again, and if problem persists, contact support.",
                        RunParams::getInstance().get(RPARAM_APPNAME).c_str(),MB_OK|MB_IUnable to retrieve configuration information. Please try again, and if problem persists, contact support.CONSTOP);*/
			TA_Base_Bus::TransActiveMessage userMsg;
			userMsg.showMsgBox(IDS_UE_030001);
			// ++ TD14164 
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Cannot retrieve DSS Plan Map details to display");

            //UE-010001
            CString errorMsg("Database cannot be accessed by the ");
            errorMsg += RunParams::getInstance().get(RPARAM_APPNAME).c_str();
            errorMsg += ".";
            // TD14164 ++
			/*MessageBox(errorMsg,RunParams::getInstance().get(RPARAM_APPNAME).c_str(),MB_OK|MB_ICONSTOP);*/
			TA_Base_Bus::TransActiveMessage userMsg;
			CString fieldName = "DSS Plan Map";
			userMsg << fieldName;
			userMsg << errorMsg;
			userMsg.showMsgBox(IDS_UE_030067);
			// ++ TD14164
        }

        FUNCTION_EXIT;
    }

    void AtsDssPlanMapDetailView::onButtonPlan() 
    {
        FUNCTION_ENTRY("onButtonPlan");

        std::string currentPlanPath;

        try
        {
            // Retrieve the current plan this association is using
            currentPlanPath = m_currentAtsDssPlanMap->getPlanPath();
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", "Could not set the data");
            //UE-010001
            CString errorMsg("Database cannot be accessed by the ");
            errorMsg += RunParams::getInstance().get(RPARAM_APPNAME).c_str();
            errorMsg += ".";
			// TD14164 ++
			/*MessageBox(errorMsg,RunParams::getInstance().get(RPARAM_APPNAME).c_str(),MB_OK|MB_ICONSTOP);*/
			TA_Base_Bus::TransActiveMessage userMsg;
			userMsg << errorMsg;
			userMsg.showMsgBox(IDS_UE_030073);
			// ++ TD14164

            FUNCTION_EXIT;
            return;
        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", "Could not set the data");
            //UE-030001
			// TD14164 ++
            /*MessageBox("Unable to retrieve configuration information. Please try again, and if problem persists, contact support.",RunParams::getInstance().get(RPARAM_APPNAME).c_str(),MB_OK|MB_ICONSTOP);*/
			TA_Base_Bus::TransActiveMessage userMsg;
			userMsg.showMsgBox(IDS_UE_030001);
			// TD14164 ++

            FUNCTION_EXIT;
            return;
        }

        // Now construct the dialog and then wait for the user to finish with it. We must set the resource
        // handles up correctly so that the plan tree control can load icons correctly.
        HINSTANCE oldInstance = AfxGetResourceHandle();
        AfxSetResourceHandle( GetModuleHandle(AtsDssPlanMapComponent::DLL_NAME) );

        ChangePlanDialog dlg(currentPlanPath, this);
        int response = dlg.DoModal();

        AfxSetResourceHandle( oldInstance );

        if (response != IDOK)
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "User cancelled the selection of a new plan");
            FUNCTION_EXIT;
            return;
        }

        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "User selected a new plan which is %s", dlg.getSelectedPlanPath());

        try
        {
            if (m_currentAtsDssPlanMap == NULL)
            {
                return;
            }

            // If this is a different entity type then set it in our database object.
            m_currentAtsDssPlanMap->setPlanPath( std::string(dlg.getSelectedPlanPath()) );

            // Now update the entity type the details view is showing
            m_plan.SetWindowText( dlg.getSelectedPlanPath() );
            m_plan.setTextType( TA_Base_Bus::TextColourEdit::VALID );

    	    m_listView->itemDetailsChanged(m_currentAtsDssPlanMap->getUniqueIdentifier(),"");
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", "Could not set the data");
            //UE-010001
            CString errorMsg("Database cannot be accessed by the ");
            errorMsg += RunParams::getInstance().get(RPARAM_APPNAME).c_str();
            errorMsg += ".";
			// TD14164 ++
			/*MessageBox(errorMsg,RunParams::getInstance().get(RPARAM_APPNAME).c_str(),MB_OK|MB_ICONSTOP);*/
			TA_Base_Bus::TransActiveMessage userMsg;
			userMsg << errorMsg;
			userMsg.showMsgBox(IDS_UE_030073);
			// ++ TD14164

            FUNCTION_EXIT;
            return;
        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", "Could not set the data");
            //UE-030001
			// TD14164 ++
            /*MessageBox("Unable to retrieve configuration information. Please try again, and if problem persists, contact support.",RunParams::getInstance().get(RPARAM_APPNAME).c_str(),MB_OK|MB_ICONSTOP);*/
			TA_Base_Bus::TransActiveMessage userMsg;
			userMsg.showMsgBox(IDS_UE_030001);
			// TD14164 ++

            FUNCTION_EXIT;
            return;
        }

        refreshButtonAvailability();
        
        FUNCTION_EXIT;
    }

    void AtsDssPlanMapDetailView::onKillfocusEditDssEvent() 
    {
        FUNCTION_ENTRY("onKillfocusEditDssEvent");

        if (m_currentAtsDssPlanMap == NULL)
        {
            return;
        }

        CString dssEventCString;
        m_dssEvent.GetWindowText(dssEventCString);

        try
        {
            // If the Dss Event string has not changed then there is no point doing an update
			std::istringstream dssEventStream;
			unsigned long dssEvent = 0;
			std::string dssString = dssEventCString;
			dssEventStream.str(dssString);
			dssEventStream >> dssEvent;
		
			if (dssEvent == m_currentAtsDssPlanMap->getDssEvent())
            {
                m_dssEvent.setTextType( TA_Base_Bus::TextColourEdit::VALID );
            }
            else
            {
				m_currentAtsDssPlanMap->setDssEvent( dssEvent );

                m_dssEvent.setTextType( TA_Base_Bus::TextColourEdit::VALID );

                m_listView->itemDetailsChanged(m_currentAtsDssPlanMap->getUniqueIdentifier(),"");
            }
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", "Could not set the data");
            //UE-010001
            CString errorMsg("Database cannot be accessed by the ");
            errorMsg += RunParams::getInstance().get(RPARAM_APPNAME).c_str();
            errorMsg += ".";
			// TD14164 ++
            /*MessageBox(errorMsg,RunParams::getInstance().get(RPARAM_APPNAME).c_str(),MB_OK|MB_ICONSTOP);*/
			TA_Base_Bus::TransActiveMessage userMsg;
			userMsg << errorMsg;
			userMsg.showMsgBox(IDS_UE_030073);
			// ++ TD14164

            m_dssEvent.setTextType( TA_Base_Bus::TextColourEdit::INVALID );

            FUNCTION_EXIT;
            return;
        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", "The Dss Event specified could not be found in the database");

            m_dssEvent.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
            FUNCTION_EXIT;
            return;
        }

        refreshButtonAvailability();
        
        FUNCTION_EXIT;
    }


    void AtsDssPlanMapDetailView::onKillfocusEditPlan() 
    {
        FUNCTION_ENTRY("onKillfocusEditPlan");
	    
        if (m_currentAtsDssPlanMap == NULL)
        {
            return;
        }

        CString planString;
        m_plan.GetWindowText(planString);

        try
        {
            // If the plan path string has not changed then there is no point doing an update
            if (planString == m_currentAtsDssPlanMap->getPlanPath().c_str())
            {
                m_plan.setTextType( TA_Base_Bus::TextColourEdit::VALID );
            }
            else
            {
                // Attempt to set the path as entered by the operator. The path will be
                // validated to ensure that it denotes an existing plan.
                m_currentAtsDssPlanMap->setPlanPath( std::string(planString) );

                // Misformatting of the plan path by users is allowed to a limited extent
                // (e.g. a missing '/' at the beginning is OK).
                // Now that the path is validated we can reset the plan edit box to display
                // the path in the correct format.
                m_plan.SetWindowText(m_currentAtsDssPlanMap->getPlanPath().c_str());
                m_plan.setTextType( TA_Base_Bus::TextColourEdit::VALID );
                
                m_listView->itemDetailsChanged(m_currentAtsDssPlanMap->getUniqueIdentifier(),"");
            }
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", "Could not set the data");
            //UE-010001
            CString errorMsg("Database cannot be accessed by the ");
            errorMsg += RunParams::getInstance().get(RPARAM_APPNAME).c_str();
            errorMsg += ".";
			// TD14164 ++
            /*MessageBox(errorMsg,RunParams::getInstance().get(RPARAM_APPNAME).c_str(),MB_OK|MB_ICONSTOP);*/
			TA_Base_Bus::TransActiveMessage userMsg;
			userMsg << errorMsg;
			userMsg.showMsgBox(IDS_UE_030073);
			// ++ TD14164

            FUNCTION_EXIT;
            return;
        }
        catch( const TA_Base_Core::DataException& ex)
        {
            CString errorMsg("Invalid path entered for the plan. ");
            errorMsg += ex.what();

            LOG_EXCEPTION_CATCH(SourceInfo, "Database", (LPCTSTR)errorMsg);

            m_plan.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
        }

        refreshButtonAvailability();
        
        FUNCTION_EXIT;
    }

    
    void AtsDssPlanMapDetailView::onButtonReset() 
    {
        FUNCTION_ENTRY("onButtonReset");

        resetButtonPressed(m_currentAtsDssPlanMap);

        FUNCTION_EXIT;
    }


    void AtsDssPlanMapDetailView::onButtonApply() 
    {
	    FUNCTION_ENTRY("onButtonApply");

        GetDlgItem(IDC_BUTTON_APPLY)->SetFocus();

        if ( !m_dssEvent.isTextValid() || !m_plan.isTextValid() )
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
        applyButtonPressed(m_currentAtsDssPlanMap);

        // Force this call so our status column updates
        m_listView->itemDetailsChanged(m_currentAtsDssPlanMap->getUniqueIdentifier(), "");

        FUNCTION_EXIT;
    }


    /////////////////////////////////////////////////////////////////////////////
    // AtsDssPlanMapDetailView diagnostics

    #ifdef _DEBUG
    void AtsDssPlanMapDetailView::AssertValid() const
    {
	    AbstractDetailView::AssertValid();
    }

    void AtsDssPlanMapDetailView::Dump(CDumpContext& dc) const
    {
	    AbstractDetailView::Dump(dc);
    }
    #endif //_DEBUG

}

