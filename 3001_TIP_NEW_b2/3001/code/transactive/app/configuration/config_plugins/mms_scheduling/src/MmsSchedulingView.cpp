/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/configuration/config_plugins/mms_scheduling/src/MmsSchedulingView.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * This is the main view for configuring MMS Scheduling
  * This is the view that will display all scheduling information
  * and allow it to be configured.
  */

#pragma warning(disable:4786 4284)

#include "app/configuration/config_plugins/mms_scheduling/src/stdafx.h"
#include "app/configuration/config_plugins/mms_scheduling/src/MmsSchedulingView.h"
#include "app/configuration/config_plugins/mms_scheduling/src/MmsSchedulingComponent.h"
#include "app/configuration/config_plugins/mms_scheduling/src/MmsSchedulingDatabaseAccessor.h"

#include "app/configuration/configuration_editor/src/IProgressStatusBar.h"

#include "core/data_access_interface/mms_dai/src/ConfigMmScheduling.h"

#include "bus/generic_gui/src/TransactiveMessage.h" // TD14164
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "MMSSchedulingUtilityDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::DebugUtil;
using TA_Base_Core::RunParams;

namespace TA_IRS_App
{
    // This is the maximum number of items we should show in a list in an error message
    const int MmsSchedulingView::MAX_NUMBER_OF_ITEMS_TO_LIST = 10;

    IMPLEMENT_DYNCREATE(MmsSchedulingView, CFormView)

    MmsSchedulingView::MmsSchedulingView()
	    : m_component(NULL),
          CFormView(MmsSchedulingView::IDD),
          m_scheduleUtility(0)
    {
    }


    MmsSchedulingView::~MmsSchedulingView()
    {
    }


    void MmsSchedulingView::DoDataExchange(CDataExchange* pDX)
    {
	    CFormView::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(MmsSchedulingView)
	    DDX_Control(pDX, IDC_SUBSYSTEM_LIST, m_schedulingList);
	    //}}AFX_DATA_MAP
    }



BEGIN_MESSAGE_MAP(MmsSchedulingView, CFormView)
	//{{AFX_MSG_MAP(MmsSchedulingView)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, onButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_RESET, onButtonReset)
    ON_BN_CLICKED(IDC_BUTTON_UTILITY, onButtonSchedulingUtility)
    ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    void MmsSchedulingView::OnInitialUpdate() 
    {
	    CFormView::OnInitialUpdate();
	    
    }

    void MmsSchedulingView::setComponentInterface(MmsSchedulingComponent* component)
    {
        TA_ASSERT(component != NULL, "The component passed in was NULL");
        
        m_component = component;

        loadItems();
    }

    
    void MmsSchedulingView::refreshButtonAvailability()
    {
        bool isAvailable = false;

        if (0 != m_component && 0 != m_component->getDatabaseAccessor())
        {
            std::vector<std::string> changedItems;
            isAvailable = m_component->getDatabaseAccessor()->areCurrentChangesPending(changedItems);
        }

        refreshButtonAvailability(isAvailable);
    }

    
    void MmsSchedulingView::refreshButtonAvailability(bool isAvailable)
    {
        GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(isAvailable);
        GetDlgItem(IDC_BUTTON_APPLY)->EnableWindow(isAvailable);
    }


    void MmsSchedulingView::loadItems()
    {
        FUNCTION_ENTRY("loadItems");

        TA_ASSERT(m_component != NULL, "The MmsSchedulingComponent must be set in this class before this method can be called");

        // Retrieve the progress bar and display it on the status bar
        CProgressCtrl& progress = m_component->getStatusBar()->displayProgressBar();
        progress.SetStep(1);

        // Use our dummy static control to retrieve the positioning for the grid and then destroy it
        CRect position;

        GetDlgItem(IDC_LIST_POSITION)->GetWindowRect(&position);
        GetDlgItem(IDC_LIST_POSITION)->DestroyWindow();

        // Now convert to client position rather than on the whole screen
        ScreenToClient(&position);

        // Invalidate and re-populate the details - if the user clicks on the plugin icon
        // for a second time - should force a refresh from database (to ensure all data is reloaded)
        m_component->getDatabaseAccessor()->invalidateData();

        // Now create the subsystem grid at the position given.
	    m_schedulingGrid.Create(position,
                    this, 
                    IDD_MAIN_GRID_ID, 
                    WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_BORDER);

        try
        {
            m_schedulingGrid.loadItems(m_component->getDatabaseAccessor(), progress);

            m_component->getStatusBar()->hideProgressBar();

            refreshButtonAvailability(false); // Disable buttons as no changes have been made yet
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            //UE-010001
            CString errorMsg("Database cannot be accessed by the ");
            errorMsg += TA_Base_Core::RunParams::getInstance().get(RPARAM_APPNAME).c_str();
            errorMsg += ".";
			// TD14164 ++
            /*MessageBox(errorMsg,TA_Base_Core::RunParams::getInstance().get(RPARAM_APPNAME).c_str(),MB_OK|MB_ICONSTOP);*/
			TA_Base_Bus::TransActiveMessage userMsg;
			userMsg << errorMsg;
			userMsg.showMsgBox(IDS_UE_030072);
			// ++ TD14164 

            m_component->getStatusBar()->hideProgressBar();
            FUNCTION_EXIT;
            return;
        }
        catch ( const TA_Base_Core::DataException& )
        {
            //UE-030001
			// TD14164 ++
            /*MessageBox("Unable to retrieve configuration information. Please try again, and if problem persists, contact support.",
                        TA_Base_Core::RunParams::getInstance().get(RPARAM_APPNAME).c_str(),
                        MB_OK|MB_ICONSTOP);*/
			TA_Base_Bus::TransActiveMessage userMsg;
			userMsg.showMsgBox(IDS_UE_030001);
			// TD14164 ++

            m_component->getStatusBar()->hideProgressBar();
            FUNCTION_EXIT;
            return;
        }


        FUNCTION_EXIT;
    }


    void MmsSchedulingView::onButtonApply() 
    {
	    FUNCTION_ENTRY("onButtonApply");

        GetDlgItem(IDC_BUTTON_APPLY)->SetFocus();

        TA_ASSERT(m_component != NULL, "The component stored is NULL");

        GetDlgItem(IDC_BUTTON_APPLY)->SetFocus();
        
        try
        {
            // Set up the message details that remain the same for every change notification we dispatch
            TA_Base_App::MessageSender::UpdateDetails updateDetails;
            updateDetails.isNewItem = false;
            updateDetails.isDelete = false;
            updateDetails.configUpdateMessage = TA_Base_App::MessageSender::MMS_SCHEDULE;

            CProgressCtrl& progress = m_component->getStatusBar()->displayProgressBar();
    
            // Retrieve the first lot of rights
            ScheduleMap schedules = m_component->getDatabaseAccessor()->getAllSchedules();

            progress.SetStep(1);
            progress.SetRange( 0, schedules.size() );

            for (ScheduleMap::iterator iter = schedules.begin(); iter != schedules.end(); ++iter)
            {
                TA_ASSERT(iter->second != NULL, "One of the schedules given to us from the MmsSchedulingDatabaseAccessor is NULL");

                // IF this particular set of rights has not changed then we continue onto the next one
                if (!iter->second->hasChanged())
                {
                    progress.StepIt();
                    continue;
                }

                updateDetails.nameOfChangedItem = iter->second->getSubsystemName();

                // The values (from/to) of the changed data doesn't matter
                //  - not posted by the configuration editor 
                // Have hardcoded the column name (works for now, but this should be changed in future!)
                std::string timeColumn = "TIME_OF_DAY";
                TA_Base_Core::ItemChanges changes;
                TA_Base_Core::Values values;
                values.newValue = "";
                values.oldValue = "";
                changes.insert( TA_Base_Core::ItemChanges::value_type( timeColumn, values ) );

                // We should query the changes form the config item, however the
                // dai hadn't been fully implemented (so bypass this mechanism)
                // This should probably be changed back at a later date
                // updateDetails.changes = iter->second->getAllItemChanges();
                updateDetails.changes = changes;
                updateDetails.keyOfChangedItem = iter->second->getKey();

                // Apply the changes to the database
                iter->second->applyChanges();

                m_component->getMessageSender().sendMessagesForTheChange( updateDetails );

                progress.StepIt();
            }

            refreshButtonAvailability(false); // Disable buttons as all changes have just been applied
        }
        catch ( const TA_Base_Core::DataConfigurationException& exception )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataConfigurationException","");

            //UE-030060
			// TD14164 ++
            /*CString errorMsg("The item could not be applied because the following mandatory attributes are missing:\n");*/
			CString errorMsg("the following mandatory attributes are missing:");
			// ++ TD14164 

            std::vector<std::string> missing = exception.getMissingFields();
            for (std::vector<std::string>::iterator iter = missing.begin(); iter != missing.end(); ++iter)
            {
                errorMsg += iter->c_str();
                errorMsg += "\n";
            }

			// TD14164 ++
            /*MessageBox(errorMsg,TA_Base_Core::RunParams::getInstance().get(RPARAM_APPNAME).c_str(),MB_OK|MB_ICONSTOP);*/
			TA_Base_Bus::TransActiveMessage userMsg;
			userMsg << errorMsg;
			userMsg.showMsgBox(IDS_UE_030068);
			// ++ TD14164 
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException","Could not apply the item changes");

            //UE-030060
			// TD14164 ++
            /*MessageBox("The item could not be applied because of a database error.",TA_Base_Core::RunParams::getInstance().get(RPARAM_APPNAME).c_str(),MB_OK|MB_ICONSTOP);*/
			TA_Base_Bus::TransActiveMessage userMsg;
			CString reason = "of a database error";
			userMsg << reason;
			userMsg.showMsgBox(IDS_UE_030068);
			// ++ TD14164 
        }
        catch ( const TA_Base_Core::DataException& exception)
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException","Could not apply the item changes");

            //UE-030060
            /*CString errorMessageFormatString("The item could not be applied because %s.");*/ // TD14164 
            CString errorMessage;

            switch(exception.getFailType())
            {
                case (TA_Base_Core::DataException::NOT_UNIQUE):
                {
                    CString reason("the field(s) ");
                    reason += exception.getWhichData().c_str();
                    reason += " must be unique in the system";
					// TD14164 ++
                    /*errorMessage.Format(errorMessageFormatString,reason);*/ 
					errorMessage = reason;
					// TD14164 ++
                    break;
                } 

                case (TA_Base_Core::DataException::WRONG_TYPE):
                {
                    CString invalidAttributes = "the following attributes are invalid:\n";
                    invalidAttributes += exception.getWhichData().c_str();
					// TD14164 ++
                    /*errorMessage.Format(errorMessageFormatString,invalidAttributes);*/
					errorMessage = invalidAttributes;
					// TD14164 ++
                    break;
                }
            
                case (TA_Base_Core::DataException::MISSING_MANDATORY):
                {
                    CString missingAttributes = "the following mandatory attributes are missing:\n";
                    missingAttributes += exception.getWhichData().c_str();
					// TD14164 ++
                    /*errorMessage.Format(errorMessageFormatString,missingAttributes);*/
					errorMessage = missingAttributes;
					// TD14164 ++
                    break;
                }

                default:
                {
					// TD14164 ++
                    /*errorMessage.Format(errorMessageFormatString,"of an unknown reason");*/
					errorMessage = "of an unknown reason";
					// TD14164 ++
                    break;
                }
            }

			// TD14164 ++
            /*MessageBox(errorMessage,TA_Base_Core::RunParams::getInstance().get(RPARAM_APPNAME).c_str(),MB_OK|MB_ICONSTOP);*/
			TA_Base_Bus::TransActiveMessage userMsg;
			CString action = "apply";
			userMsg << action;
			userMsg << errorMessage;
			userMsg.showMsgBox(IDS_UE_030060);
			// TD14164 ++
        }
        

        m_component->getStatusBar()->hideProgressBar();

        FUNCTION_EXIT;
    }


    void MmsSchedulingView::onButtonReset() 
    {
        FUNCTION_ENTRY("onButtonReset");

        std::vector<std::string> rightsNotApplied; // This is not actually needed but we must pass it to the method call below
        if (m_component->getDatabaseAccessor()->areCurrentChangesPending(rightsNotApplied))
        {
            //UW-010003
			// TD14164 ++
            /*int response = MessageBox("Are you sure you want to reset and lose all changes?",
                                TA_Base_Core::RunParams::getInstance().get(RPARAM_APPNAME).c_str(),
                                MB_YESNO|MB_ICONEXCLAMATION);*/
			TA_Base_Bus::TransActiveMessage userMsg;
			UINT response = userMsg.showMsgBox(IDS_UW_190003);
			// ++ TD14164 

            if (response == IDNO)
            {
                LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "User cancelled the reset");
                FUNCTION_EXIT;
                return;
            }
        }

        TA_ASSERT(m_component != NULL, "Component object stored is NULL"); 

        // Invalidate and re-populate the details. This will ensure all data is reloaded from the database.
        m_component->getDatabaseAccessor()->invalidateData();

        CProgressCtrl& progress = m_component->getStatusBar()->displayProgressBar();

        // Now tell the grids to refresh their data
        try
        {
            m_schedulingGrid.loadItems(m_component->getDatabaseAccessor(), progress);
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            //UE-010001
            CString errorMsg("Database cannot be accessed by the ");
            errorMsg += TA_Base_Core::RunParams::getInstance().get(RPARAM_APPNAME).c_str();
            errorMsg += ".";
			// TD14164 ++
            /*MessageBox(errorMsg,TA_Base_Core::RunParams::getInstance().get(RPARAM_APPNAME).c_str(),MB_OK|MB_ICONSTOP);*/
			TA_Base_Bus::TransActiveMessage userMsg;
			userMsg << errorMsg;
			userMsg.showMsgBox(IDS_UE_030072);
			// ++ TD14164 

            m_component->getStatusBar()->hideProgressBar();
            FUNCTION_EXIT;
            return;
        }
        catch ( const TA_Base_Core::DataException& )
        {
            //UE-030001
			// TD14164 ++	
            /*MessageBox("Unable to retrieve configuration information. Please try again, and if problem persists, contact support.",TA_Base_Core::RunParams::getInstance().get(RPARAM_APPNAME).c_str(),MB_OK|MB_ICONSTOP);*/
			TA_Base_Bus::TransActiveMessage userMsg;
			userMsg.showMsgBox(IDS_UE_030001);
			// TD14164 ++

            m_component->getStatusBar()->hideProgressBar();
            FUNCTION_EXIT;
            return;
        }

        refreshButtonAvailability(false); // Disable buttons as all changes have just been applied
        
        m_component->getStatusBar()->hideProgressBar();

        FUNCTION_EXIT;
    }
   

    void MmsSchedulingView::onButtonSchedulingUtility()
    {   
        // Display a new scheduling utility (if one already exists, restore that one)
        if (0 == m_scheduleUtility)
        {
            m_scheduleUtility = new MMSSchedulingUtilityDlg();
            m_scheduleUtility->Create(IDD_MMSSCHEDULINGUTILITY_DIALOG, this);
            m_scheduleUtility->ShowWindow(SW_SHOW);
        }
        else
        {        
            // Restore the existing utility..
            m_scheduleUtility->ShowWindow(SW_SHOW);
            m_scheduleUtility->SetFocus();
        }
    }

    
    void MmsSchedulingView::OnDestroy()
    {
        if (0 != m_scheduleUtility)
        {
            m_scheduleUtility->DestroyWindow();
            delete m_scheduleUtility;
            m_scheduleUtility = 0;
        }
    }
    /////////////////////////////////////////////////////////////////////////////
    // MmsSchedulingView diagnostics

    #ifdef _DEBUG
    void MmsSchedulingView::AssertValid() const
    {
	    CFormView::AssertValid();
    }

    void MmsSchedulingView::Dump(CDumpContext& dc) const
    {
	    CFormView::Dump(dc);
    }
    #endif //_DEBUG
}
