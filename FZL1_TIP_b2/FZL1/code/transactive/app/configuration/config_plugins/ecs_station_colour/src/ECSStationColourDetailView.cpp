/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/configuration/config_plugins/ecs_station_colour/src/ECSStationColourDetailView.cpp $
  * @author:  Andy Siow
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view allows the user to configure a ECSStationColour's details.
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/ecs_station_colour/src/StdAfx.h"
#include "app/configuration/config_plugins/ecs_station_colour/src/ECSStationColourDetailView.h"

#include "app/configuration/config_plugins/ecs_station_colour/src/ECSStationColourComponent.h"
#include "app/configuration/config_plugins/ecs_station_colour/src/ECSStationColourDatabaseAccessor.h"
#include "app/configuration/config_plugins/ecs_station_colour/src/ECSStationColourListView.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/MessageSender.h"

#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"

#include "bus/generic_gui/src/TransactiveMessage.h" // TD14164
#include "core/data_access_interface/src/IConfigECSStationColourData.h"

#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"

// 7196-Start
#include "core/data_access_interface/src/DatabaseKey.h"
// 7196-End

#include "core/utilities/src/DateFormat.h" // TD12474

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::RunParams;
using TA_Base_Core::DebugUtil;
using  namespace TA_Base_App::TA_Configuration;

namespace TA_IRS_App
{
    // These match the restrictions in the database for how long certain fields can be.
    // Hopefully in the future these could be read from the database.
    const int ECSStationColourDetailView::NAME_FIELD_LENGTH        = 80;
    

    IMPLEMENT_DYNCREATE(ECSStationColourDetailView, CFormView)


    ECSStationColourDetailView::ECSStationColourDetailView() : 
        m_currentECSStationColour(NULL),
        m_listView(NULL),
        m_databaseAccessor(NULL),
        m_editBackgroundBrush(NULL),
        AbstractDetailView(ECSStationColourDetailView::IDD)
    {
	    //{{AFX_DATA_INIT(ECSStationColourDetailView)
	//}}AFX_DATA_INIT
    }


    ECSStationColourDetailView::~ECSStationColourDetailView()
    {
    }


    void ECSStationColourDetailView::DoDataExchange(CDataExchange* pDX)
    {
	    CFormView::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(ECSStationColourDetailView)
	DDX_Control(pDX, IDC_NAME_BITMAP, m_nameBitmap);
	DDX_Control(pDX, IDC_COLOUR_BITMAP2, m_colourBitmap);
	DDX_Control(pDX, IDC_STATIC_TYPE, m_type);
    DDX_Control(pDX, IDC_STATIC_MODIFYDATE, m_modifyDate);
    DDX_Control(pDX, IDC_STATIC_CREATEDATE, m_createDate);
	DDX_Control(pDX, IDC_EDIT_NAME, m_name);
    DDX_Control(pDX, IDC_EDIT_COLOUR, m_colour);
	//}}AFX_DATA_MAP
    }


BEGIN_MESSAGE_MAP(ECSStationColourDetailView, AbstractDetailView)
	//{{AFX_MSG_MAP(ECSStationColourDetailView)
	ON_BN_CLICKED(IDC_BUTTON_RESET, onButtonReset)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, onButtonApply)
    ON_BN_CLICKED(IDC_BUTTON_COLOUR, onButtonColour)
	ON_EN_KILLFOCUS(IDC_EDIT_NAME, onKillfocusEditName)
    ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    void ECSStationColourDetailView::OnInitialUpdate() 
    {
        FUNCTION_ENTRY("OnInitialUpdate");

	    CFormView::OnInitialUpdate();
	    
        // Limit the number of characters that can be entered into fields so that they match the database schema.
        m_name.SetLimitText(NAME_FIELD_LENGTH);

        // Hide all the ECSStationColour details until we are told to populate.
	    showItemDetails(false);

        FUNCTION_EXIT;
    }


    void ECSStationColourDetailView::showItemDetails(bool isShown)
    {
        // Show or hide all the controls from the view
        GetDlgItem(IDC_STATIC_TYPE)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_NAME)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_COLOUR)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_CREATEDATE)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_MODIFYDATE)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_DATE_CREATED)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_DATE_MODIFIED)->ShowWindow(isShown);
        GetDlgItem(IDC_EDIT_NAME)->ShowWindow(isShown);
        GetDlgItem(IDC_EDIT_COLOUR)->ShowWindow(isShown);
        GetDlgItem(IDC_BUTTON_COLOUR)->ShowWindow(isShown);
        GetDlgItem(IDC_NAME_BITMAP)->ShowWindow(isShown);
        GetDlgItem(IDC_COLOUR_BITMAP2)->ShowWindow(isShown);
        GetDlgItem(IDC_BUTTON_RESET)->ShowWindow(isShown);
        GetDlgItem(IDC_BUTTON_APPLY)->ShowWindow(isShown);
    }


    void ECSStationColourDetailView::refreshButtonAvailability()
    {
        if (m_currentECSStationColour == NULL)
        {
            return;
        }

        if (m_currentECSStationColour->isNew() )
        {
            GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(false);
            GetDlgItem(IDC_BUTTON_APPLY)->EnableWindow(true);
            return;
        }
        
        // We have an old item
        if (m_currentECSStationColour->hasChanged())
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

    
    void ECSStationColourDetailView::populateItemDetails(TA_Base_Core::IConfigItem* currentECSStationColour)
    {
        FUNCTION_ENTRY("populateItemDetails");

        if (m_databaseAccessor == NULL || m_listView == NULL)
        {
            m_databaseAccessor = dynamic_cast<ECSStationColourDatabaseAccessor*>(getDatabaseAccessor());
            m_listView = dynamic_cast<ECSStationColourListView*>(getListView());

            TA_ASSERT(m_databaseAccessor != NULL, "setDatabaseAccessor() must be called before this method");
            TA_ASSERT(m_listView != NULL, "The list view has not been set yet or is not a proper ECSStationColourDetailView");
        }


        m_currentECSStationColour = dynamic_cast<TA_Base_Core::IConfigECSStationColourData*>(currentECSStationColour);

        if (m_currentECSStationColour == NULL)
        { 
            showItemDetails(false);
            FUNCTION_EXIT;
            return;
        }

        try
        {
            //  Now populate all the fields with the data retrieved from the IConfigECSStationColourData* object

	        m_name.SetWindowText( m_currentECSStationColour->getName().c_str() );

            m_type.SetWindowText(NO_VALUE);

           // std::string soundFilePath = m_currentECSStationColour->getSoundFilePath();
           // m_sound.SetWindowText( soundFilePath.c_str() );

            // Initialise the colour bar to the value in the DB
            COLORREF colour = static_cast<COLORREF>(
                m_currentECSStationColour->getStationColourKey());

            setColourEditBackground(colour);

			TA_Base_Core::DateFormat dateFormat; // TD12474

            // Retrieve the modify time and date.
            // If this is zero then one hasn't been set yet
            // so leave the modified field as blank
            time_t modify = m_currentECSStationColour->getDateModified();
            if (modify == 0)
            {
                m_modifyDate.SetWindowText(NO_VALUE);
            }
            else
            {
                COleDateTime dateModified( modify );
				// TD12474 ++
                //m_modifyDate.SetWindowText( dateModified.Format(TA_Base_App::TA_Configuration::DATE_FORMAT) );
				std::string dateFormatStr = dateFormat.getDateFormat(TA_Base_Core::DateFormat::DateTimeFormat6);
				m_modifyDate.SetWindowText( dateModified.Format(dateFormatStr.c_str()) );
				// ++ TD12474
            }

            // The create time should always be set so we don't need to
            // perform the same checks as the modify date.
            COleDateTime dateCreated ( m_currentECSStationColour->getDateCreated() );
			// TD12474 ++
			//m_createDate.SetWindowText( dateCreated.Format(TA_Base_App::TA_Configuration::DATE_FORMAT) );
			std::string dateFormatStr = dateFormat.getDateFormat(TA_Base_Core::DateFormat::DateTimeFormat6);
			m_createDate.SetWindowText( dateCreated.Format(dateFormatStr.c_str()) );
			// ++ TD12474

            showItemDetails(true);

            refreshButtonAvailability();
        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Cannot retrieve ECSStationColour details to display");
            
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
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Cannot retrieve ECSStationColour details to display");

            //UE-010001
            CString errorMsg("Database cannot be accessed by the ");
            errorMsg += RunParams::getInstance().get(RPARAM_APPNAME).c_str();
            errorMsg += ".";
			// TD14164 ++
            /*MessageBox(errorMsg,RunParams::getInstance().get(RPARAM_APPNAME).c_str(),MB_OK|MB_ICONSTOP);*/
			TA_Base_Bus::TransActiveMessage userMsg;
			CString fieldName = "ECSStationColour";
			userMsg << fieldName;
			userMsg << errorMsg;
			userMsg.showMsgBox(IDS_UE_030067);
			// ++ TD14164
        }

        FUNCTION_EXIT;
    }


    void ECSStationColourDetailView::onKillfocusEditName() 
    {
        FUNCTION_ENTRY("onKillfocusEditName");
        /*
        this is a read only field

        // When the user changes the name we must update the IConfigECSStationColourData* object so that it
        // has a record of all changes when the user chooses to apply.
        if (m_currentECSStationColour != NULL )
        {
            try
            {
                CString name;
                m_name.GetWindowText(name);
                m_currentECSStationColour->setName( std::string(name) );

                if (m_listView!=NULL)
                {
                    std::string name = m_currentECSStationColour->getName();
                    m_listView->itemDetailsChanged( m_currentECSStationColour->getUniqueIdentifier(), name.c_str() );
                }
            }
            catch ( const TA_Base_Core::DataException& )
            {
                LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Cannot retrieve alarm plan association details to display");
            
                //UE-030001
                MessageBox("Unable to retrieve configuration information. Please try again, and if problem persists, contact support.",
                            RunParams::getInstance().get(RPARAM_APPNAME).c_str(),MB_OK|MB_ICONSTOP);
            }
            catch ( const TA_Base_Core::DatabaseException& )
            {
                LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Cannot retrieve alarm plan association details to display");

                //UE-010001
                CString errorMsg("Database cannot be accessed by the ");
                errorMsg += RunParams::getInstance().get(RPARAM_APPNAME).c_str();
                errorMsg += ".";
                MessageBox(errorMsg,RunParams::getInstance().get(RPARAM_APPNAME).c_str(),MB_OK|MB_ICONSTOP);
            }
        }
        
        refreshButtonAvailability();
        */
            
        FUNCTION_EXIT;
    }


    void ECSStationColourDetailView::onButtonReset() 
    {
        FUNCTION_ENTRY("onButtonReset");

        resetButtonPressed(m_currentECSStationColour);

        refreshButtonAvailability();
            
        FUNCTION_EXIT;
    }


    void ECSStationColourDetailView::onButtonApply() 
    {
	    FUNCTION_ENTRY("onButtonApply");
        
        GetDlgItem(IDC_BUTTON_APPLY)->SetFocus();

        applyButtonPressed(m_currentECSStationColour);

		// 7196-Start
		// Send online Update Message
		sendUpdateMessageForChangeItem();
		// 7196-End

        FUNCTION_EXIT;
    }
	
	// 7196-Start
	// This function sends an update message when there is a change in the table data
	// The ECS Manager has to recieve this update message and update the display
	void ECSStationColourDetailView::sendUpdateMessageForChangeItem()
	{
		FUNCTION_ENTRY("sendUpdateMessageForChangeItem");
		
		UpdateData(TRUE);
		CString strWindowText;
		TA_Base_App::MessageSender::UpdateDetails updateDetails;        
		TA_Base_Core::Values values;
        updateDetails.isNewItem = false;
        updateDetails.isDelete  = false;        
        updateDetails.keyOfChangedItem	  = TA_Base_Core::DatabaseKey::getInvalidKey();

		// TODO:Currently the config message type returns the ECS_STATION_COLOUR
		// however the colour type has been moved to Gloabal Parameter.need to update
		// config update message type to reflect the gloabal parameter value.
		updateDetails.configUpdateMessage = getConfigMessageType();	
	
		// Name
		m_name.GetWindowText(strWindowText);
		values.newValue								 =  LPCSTR(strWindowText);
		updateDetails.changes["Name"]				 = values;
		values.newValue								 = "";
		strWindowText								 = "";
		// Color
		// TD17827 - marc_bugfix_start
		// Prevent crash in ECS Station Colour plugin
		LOGBRUSH logBrush;
		m_editBackgroundBrush->GetLogBrush(&logBrush);
		CString strColour;
		strColour.Format("%d",logBrush.lbColor);
		// TD17827 - marc_bugfix_end
		values.newValue								 = LPCSTR(strColour); 		
		updateDetails.changes["Colour"]				 = values;
		values.newValue								 =  "";
		strWindowText								 = "";
		
		// We update the config details with the new values.
		// Send Online Update Message now
		getMessageSender()->sendMessagesForTheChange( updateDetails );   

		FUNCTION_EXIT;
	}
	// 7196-End


    HBRUSH ECSStationColourDetailView::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
    {
        // This method is called when window controls are being painted.
        switch (nCtlColor)
        {
            // When the Edit fields are being painted
            case CTLCOLOR_EDIT:
                
                // Use the m_editBackgroundBrush for the Colour edit box
                if(pWnd->GetDlgCtrlID() == IDC_EDIT_COLOUR)
                {
                    return (HBRUSH)(m_editBackgroundBrush->GetSafeHandle());
                }

            // Use the default for everything else.
            default:
                return CFormView::OnCtlColor(pDC, pWnd, nCtlColor);
        }
    }



    void ECSStationColourDetailView::setColourEditBackground(COLORREF colour)
    {
        // Hide the edit, set its background color, then show it again.
        GetDlgItem(IDC_EDIT_COLOUR)->ShowWindow(false);

        delete m_editBackgroundBrush;
        m_editBackgroundBrush = new CBrush(colour);

        GetDlgItem(IDC_EDIT_COLOUR)->ShowWindow(true);
    }


    void ECSStationColourDetailView::onButtonColour()
    {
	    FUNCTION_ENTRY("onButtonColour");

        CColorDialog dialog;
        if(dialog.DoModal() == IDOK)
        {
            COLORREF colour = dialog.GetColor();
            setColourEditBackground(colour);

            try
            {
                // When the user changes the colour we must update the
                // IConfigAlarmType* object so that it has a record of all
                // changes when the user chooses to apply.
                if(m_currentECSStationColour != NULL)
                {
                    unsigned long colourUlong = static_cast<unsigned long>(colour);
                    m_currentECSStationColour->setStationColourKey(colourUlong);
                }
            }
            catch ( const TA_Base_Core::DataException& )
            {
                LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Cannot retrieve alarm plan association details to display");
            
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
                LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Cannot retrieve alarm plan association details to display");

                //UE-010001
                CString errorMsg("Database cannot be accessed by the ");
                errorMsg += RunParams::getInstance().get(RPARAM_APPNAME).c_str();
                errorMsg += ".";
				// TD14164 ++
                /*MessageBox(errorMsg,RunParams::getInstance().get(RPARAM_APPNAME).c_str(),MB_OK|MB_ICONSTOP);*/
				TA_Base_Bus::TransActiveMessage userMsg;
				CString fieldName = "alarm plan association";
				userMsg << fieldName;
				userMsg << errorMsg;
				userMsg.showMsgBox(IDS_UE_030067);
				// ++ TD14164
            }
        }

        refreshButtonAvailability();

        FUNCTION_EXIT;
    }


    /////////////////////////////////////////////////////////////////////////////
    // ECSStationColourDetailView diagnostics

    #ifdef _DEBUG
    void ECSStationColourDetailView::AssertValid() const
    {
	    AbstractDetailView::AssertValid();
    }

    void ECSStationColourDetailView::Dump(CDumpContext& dc) const
    {
	    AbstractDetailView::Dump(dc);
    }
    #endif //_DEBUG

}
