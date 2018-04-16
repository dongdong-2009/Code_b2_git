/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/configuration/config_plugins/ecs_master_mode/src/ECSMasterModeDetailView.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * This view allows the user to configure an ECS Master Mode's details.
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/ecs_master_mode/src/StdAfx.h"
#include "app/configuration/config_plugins/ecs_master_mode/src/ECSMasterModeDetailView.h"
#include "app/configuration/config_plugins/ecs_master_mode/src/ECSMasterModeDatabaseAccessor.h"
#include "app/configuration/config_plugins/ecs_master_mode/src/ECSMasterModeListView.h"
#include "app/configuration/config_plugins/ecs_master_mode/src/ECSMasterModeComponent.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/ChangeDialog.h"
#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"
#include "core/data_access_interface/ecs/src/IConfigECSMasterMode.h"
#include "core/data_access_interface/ecs/src/ECSStationModeAccessFactory.h"
#include "core/data_access_interface/src/ILocation.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"

#include "bus/generic_gui/src/TransactiveMessage.h" // TD14164
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"
#include "core/data_access_interface/ecs/src/IECSStationMode.h"

// 7196-Start
#include "core/data_access_interface/src/DatabaseKey.h"
// 7196-End

#include "core/utilities/src/DateFormat.h" // TD12474

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include <algorithm>

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
    // The titles for the change dialog box
    const CString ECSMasterModeDetailView::CHANGE_ECS_ZONE_DIALOG_TITLE = "Change ECS Zone";
    
    const int ECSMasterModeDetailView::MASTER_MODE_FIELD_LENGTH  = 4;
    const int ECSMasterModeDetailView::CONDITION_FIELD_LENGTH    = 40;
    const int ECSMasterModeDetailView::ECS_ZONE_FIELD_LENGTH     = 80;
    const int ECSMasterModeDetailView::EVACUATION_FIELD_LENGTH   = 20;
    const int ECSMasterModeDetailView::AIRFLOW_FIELD_LENGTH      = 20;
    const int ECSMasterModeDetailView::DISPLAY_FIELD_LENGTH		 = 20;
	const int ECSMasterModeDetailView::VERIFICATION_FIELD_LENGTH = 40;
	const int ECSMasterModeDetailView::SITUATION_FIELD_LENGTH = 40;

	const int ECSMasterModeDetailView::STATION_MODE_KEY_COLUMN = 0;
	const int ECSMasterModeDetailView::LOCATION_COLUMN = 1;
	const int ECSMasterModeDetailView::STATION_MODE_COLUMN = 2;
	const int ECSMasterModeDetailView::OPERATION_COLUMN	   = 3;
	const int ECSMasterModeDetailView::TRACK_COLUMN		   = 4;

	const int ECSMasterModeDetailView::STATION_MODE_KEY_COLUMN_WIDTH = 0;
	const int ECSMasterModeDetailView::LOCATION_COLUMN_WIDTH = 70;
	const int ECSMasterModeDetailView::STATION_MODE_COLUMN_WIDTH = 98;
	const int ECSMasterModeDetailView::OPERATION_COLUMN_WIDTH	 = 125;
	const int ECSMasterModeDetailView::TRACK_COLUMN_WIDTH		 = 54;

	const CString ECSMasterModeDetailView::STATION_MODE_KEY_COLUMN_NAME = "Key";
	const CString ECSMasterModeDetailView::LOCATION_COLUMN_NAME = "Location";
	const CString ECSMasterModeDetailView::STATION_MODE_COLUMN_NAME = "Station Mode";
	const CString ECSMasterModeDetailView::OPERATION_COLUMN_NAME = "Operation";
	const CString ECSMasterModeDetailView::TRACK_COLUMN_NAME = "Track";
	

    
        
    IMPLEMENT_DYNCREATE(ECSMasterModeDetailView, CFormView)


    ECSMasterModeDetailView::ECSMasterModeDetailView() : 
        m_configItem(NULL),
        m_listView(NULL),
        m_databaseAccessor(NULL),
        AbstractDetailView(ECSMasterModeDetailView::IDD),
		m_numberOfLocations(0)
    {
	    //{{AFX_DATA_INIT(ECSMasterModeDetailView)
	//}}AFX_DATA_INIT

    }


    ECSMasterModeDetailView::~ECSMasterModeDetailView()
    {

    }


    void ECSMasterModeDetailView::DoDataExchange(CDataExchange* pDX)
    {
	    CFormView::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(ECSMasterModeDetailView)
	DDX_Control(pDX, IDC_EDIT_STATION_BUTTON, m_editStationButton);
	DDX_Control(pDX, IDC_STATION_LIST, m_stationListCtrl);
	DDX_Control(pDX, IDC_BUTTON_APPLY, m_applyBtn);
	DDX_Control(pDX, IDC_EDIT_MASTER_MODE, m_masterModeEdt);
	DDX_Control(pDX, IDC_EDIT_CONDITION, m_conditionEdt);
	DDX_Control(pDX, IDC_EDIT_ECS_ZONE, m_ecsZoneEdt);
	DDX_Control(pDX, IDC_EDIT_EVACUATION, m_evacuationEdt);
	DDX_Control(pDX, IDC_EDIT_AIRFLOW, m_airflowEdt);
	DDX_Control(pDX, IDC_EDIT_DISPLAY, m_displayEdt);
	DDX_Control(pDX, IDC_EDIT_VERIFICATION_MSG, m_verificationMsgEdt);
	DDX_Control(pDX, IDC_EDIT_SITUATION, m_situationEdt);
	DDX_Control(pDX, IDC_BUTTON_ECS_ZONE, m_ecsZoneBtn);
	DDX_Control(pDX, IDC_STATIC_MODIFYDATE, m_modifyDate);
	DDX_Control(pDX, IDC_STATIC_CREATEDATE, m_createDate);
    DDX_Control(pDX, IDC_BITMAP1, m_bitmap1);
    DDX_Control(pDX, IDC_BITMAP2, m_bitmap2);
    DDX_Control(pDX, IDC_BITMAP3, m_bitmap3);
    DDX_Control(pDX, IDC_BITMAP4, m_bitmap4);
    DDX_Control(pDX, IDC_BITMAP5, m_bitmap5);
	DDX_Control(pDX, IDC_STATIC_TYPE, m_type);
	//}}AFX_DATA_MAP
    }


BEGIN_MESSAGE_MAP(ECSMasterModeDetailView, AbstractDetailView)
	//{{AFX_MSG_MAP(ECSMasterModeDetailView)
	ON_BN_CLICKED(IDC_BUTTON_RESET, onButtonReset)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, onButtonApply)
    ON_BN_CLICKED(IDC_BUTTON_ECS_ZONE, onButtonECSZone)
	ON_EN_KILLFOCUS(IDC_EDIT_MASTER_MODE, onKillfocusEditMasterMode)
    ON_EN_KILLFOCUS(IDC_EDIT_CONDITION, onKillfocusEditCondition)
    ON_EN_KILLFOCUS(IDC_EDIT_ECS_ZONE, onKillfocusEditECSZone)
    ON_EN_KILLFOCUS(IDC_EDIT_EVACUATION, onKillfocusEditEvacuation)
    ON_EN_KILLFOCUS(IDC_EDIT_AIRFLOW, onKillfocusEditAirflow)
    ON_EN_KILLFOCUS(IDC_EDIT_DISPLAY, onKillfocusEditDisplay)
	ON_EN_KILLFOCUS(IDC_EDIT_VERIFICATION_MSG, onKillfocusEditVerification)
	ON_EN_KILLFOCUS(IDC_EDIT_SITUATION, onKillfocusEditSituation)
	ON_BN_CLICKED(IDC_EDIT_STATION_BUTTON, OnEditStationButton)
	ON_NOTIFY(NM_CLICK, IDC_STATION_LIST, OnClickStationList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    void ECSMasterModeDetailView::OnInitialUpdate() 
    {
        FUNCTION_ENTRY("OnInitialUpdate");

	    CFormView::OnInitialUpdate();
	    
        m_masterModeEdt.SetLimitText(MASTER_MODE_FIELD_LENGTH);
        m_conditionEdt.SetLimitText(CONDITION_FIELD_LENGTH);
        m_ecsZoneEdt.SetLimitText(ECS_ZONE_FIELD_LENGTH);
        m_evacuationEdt.SetLimitText(EVACUATION_FIELD_LENGTH);
        m_airflowEdt.SetLimitText(AIRFLOW_FIELD_LENGTH);
        m_displayEdt.SetLimitText(DISPLAY_FIELD_LENGTH);
		m_verificationMsgEdt.SetLimitText(VERIFICATION_FIELD_LENGTH);
		m_situationEdt.SetLimitText(SITUATION_FIELD_LENGTH);
		

        // Hide all the ECS Master Mode details until we are told to populate.
	    showItemDetails(false);

		m_stationListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP | LVS_EX_GRIDLINES);
		m_stationListCtrl.InsertColumn(STATION_MODE_KEY_COLUMN, STATION_MODE_KEY_COLUMN_NAME, LVCFMT_LEFT, STATION_MODE_KEY_COLUMN_WIDTH);
		m_stationListCtrl.InsertColumn(LOCATION_COLUMN, LOCATION_COLUMN_NAME, LVCFMT_LEFT, LOCATION_COLUMN_WIDTH);
		m_stationListCtrl.InsertColumn(STATION_MODE_COLUMN, STATION_MODE_COLUMN_NAME, LVCFMT_LEFT, STATION_MODE_COLUMN_WIDTH);
		m_stationListCtrl.InsertColumn(OPERATION_COLUMN, OPERATION_COLUMN_NAME, LVCFMT_LEFT, OPERATION_COLUMN_WIDTH);
		m_stationListCtrl.InsertColumn(TRACK_COLUMN, TRACK_COLUMN_NAME, LVCFMT_LEFT, TRACK_COLUMN_WIDTH);

		try
		{
			std::vector<TA_Base_Core::ILocation*> allLocations = TA_Base_Core::LocationAccessFactory::getInstance().getAllLocations();
			m_numberOfLocations = allLocations.size() - 1; // Minus 1 because <Unallocated> is not a valid station location

			for(unsigned int i=0; i<allLocations.size(); i++)
			{	
				// don't bother with <unallocated> location.
				if(allLocations[i]->getKey() != 0)
				{
					m_allLocationKeysVector[allLocations[i]->getKey()] = allLocations[i]->getName();
				}
			}

			// Now we're done with the location objects;

			for(unsigned int i=0; i<allLocations.size(); i++)
			{
				delete allLocations[i];
				allLocations[i] = 0;
			}
		    
		}
		catch(...)
		{

		}
        FUNCTION_EXIT;
    }


    void ECSMasterModeDetailView::showItemDetails(bool isShown)
    {
        // Show or hide all the controls from the view

		
        m_masterModeEdt.ShowWindow(isShown);
        m_conditionEdt.ShowWindow(isShown);
        m_ecsZoneEdt.ShowWindow(isShown);
        m_ecsZoneBtn.ShowWindow(isShown);
        m_evacuationEdt.ShowWindow(isShown);
        m_airflowEdt.ShowWindow(isShown);
        m_displayEdt.ShowWindow(isShown);
		m_verificationMsgEdt.ShowWindow(isShown);
		m_situationEdt.ShowWindow(isShown);
        m_modifyDate.ShowWindow(isShown);
        m_createDate.ShowWindow(isShown);
        m_type.ShowWindow(isShown);
		m_stationListCtrl.ShowWindow(isShown);
		//m_assocStationStatic.ShowWindow(isShown);
		m_applyBtn.ShowWindow(isShown);
		m_editStationButton.ShowWindow(isShown);

		GetDlgItem(IDC_ASSOC_STATION_STATIC)->ShowWindow(isShown);
		GetDlgItem(IDC_MASTER_MODE_STATIC)->ShowWindow(isShown);
		GetDlgItem(IDC_CONDITION_STATIC)->ShowWindow(isShown);
		GetDlgItem(IDC_SITUATION_STATIC)->ShowWindow(isShown);
		GetDlgItem(IDC_ZONE_STATIC)->ShowWindow(isShown);	
		GetDlgItem(IDC_EVAC_DIR_STATIC)->ShowWindow(isShown);	
		GetDlgItem(IDC_AIRFLOW_STATIC)->ShowWindow(isShown);	
		GetDlgItem(IDC_DISPLAY_STATIC)->ShowWindow(isShown);	
		GetDlgItem(IDC_VERIFY_MSG_STATIC)->ShowWindow(isShown);
		GetDlgItem(IDC_DATE_CREATE_STATIC)->ShowWindow(isShown);	
		GetDlgItem(IDC_DATE_MOD_STATIC)->ShowWindow(isShown);	

		GetDlgItem(IDC_MASTER_MODE_BITMAP)->ShowWindow(isShown);
		GetDlgItem(IDC_BUTTON_RESET)->ShowWindow(isShown);	
		GetDlgItem(IDC_DISPLAY_BITMAP)->ShowWindow(isShown);	
		GetDlgItem(IDC_AIRFLOW_BITMAP)->ShowWindow(isShown);
		GetDlgItem(IDC_EVAC_BITMAP)->ShowWindow(isShown);	
		GetDlgItem(IDC_ZONE_BITMAP)->ShowWindow(isShown);	
		GetDlgItem(IDC_CONDITION_BITMAP)->ShowWindow(isShown);	
    }


    void ECSMasterModeDetailView::populateItemDetails(TA_Base_Core::IConfigItem* currentItem)
    {
        FUNCTION_ENTRY("populateItemDetails");

        if (m_databaseAccessor == NULL || m_listView == NULL)
        {
            m_databaseAccessor = dynamic_cast<ECSMasterModeDatabaseAccessor*>(getDatabaseAccessor());
            m_listView = dynamic_cast<ECSMasterModeListView*>(getListView());

            TA_ASSERT(m_databaseAccessor != NULL, "setDatabaseAccessor() must be called before this method");
            TA_ASSERT(m_listView != NULL, "The list view has not been set yet or is not a proper ECSMasterModeDetailView");
        }


        m_configItem = dynamic_cast<TA_IRS_Core::IConfigECSMasterMode*>(currentItem);

        if (m_configItem == NULL)
        { 
            showItemDetails(false);
            FUNCTION_EXIT;
            return;
        }

        try
        {
            //  Now populate all the fields with the data retrieved from the IConfigECSMasterMode* object
            CString buffer;

            if (m_configItem->getMasterMode() == 0)
            {
                m_masterModeEdt.SetWindowText( "" );
                m_masterModeEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );                
            }
            else
            {          
                buffer.Format("%d", m_configItem->getMasterMode());
                m_masterModeEdt.SetWindowText( buffer );
                m_masterModeEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
            }

            if (m_configItem->getCondition() == "")
            {
                m_conditionEdt.SetWindowText( "" );
                m_conditionEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
            }
            else
            {
                m_conditionEdt.SetWindowText( m_configItem->getCondition().c_str() );
                m_conditionEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );            
            }

            m_ecsZoneEdt.SetWindowText( m_databaseAccessor->getZoneName(m_configItem->getECSZoneId()).c_str());
            m_ecsZoneEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );

            if (m_configItem->getEvacuationDirection() == "")
            {
                m_evacuationEdt.SetWindowText( "" );
                m_evacuationEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
            }
            else
            {
                m_evacuationEdt.SetWindowText( m_configItem->getEvacuationDirection().c_str() );
                m_evacuationEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );            
            }

            if (m_configItem->getAirflowDirection() == "")
            {
                m_airflowEdt.SetWindowText( "" );
                m_airflowEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
            }
            else
            {
                m_airflowEdt.SetWindowText( m_configItem->getAirflowDirection().c_str() );
                m_airflowEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );            
            }

            if (m_configItem->getDisplayDirection() == "")
            {
                m_displayEdt.SetWindowText( "" );
                m_displayEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
            }
            else
            { 
                m_displayEdt.SetWindowText( m_configItem->getDisplayDirection().c_str() );
                m_displayEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );            
            }  
 
            if (m_configItem->getVerificationMessage() == "")
            {
                m_verificationMsgEdt.SetWindowText( "" );
                m_verificationMsgEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
            } 
            else
            {
                m_verificationMsgEdt.SetWindowText( m_configItem->getVerificationMessage().c_str() );
                m_verificationMsgEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );            
            }

			if (m_configItem->getSituation() == "")
			{
				m_situationEdt.SetWindowText( "" );
				m_situationEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
			}
			else
			{
				m_situationEdt.SetWindowText( m_configItem->getSituation().c_str() );
				m_situationEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );            
			}


            if( m_configItem->isNew() )
            {

                // This is a new ECS Master Mode or at least a copy of an ECS Master Mode so we indicate this in the type
                // field and make sure the modify and create dates are blank.

                m_type.SetWindowText(NEW_LABEL);
                m_modifyDate.SetWindowText(NO_VALUE);
	            m_createDate.SetWindowText(NO_VALUE);

                // Disable the reset button as this is a new item so reset has no functionality
                GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(FALSE);
            }
            else
            {

                m_type.SetWindowText( NO_VALUE );

				TA_Base_Core::DateFormat dateFormat; // TD12474

                // Retrieve the modify time and date. If this is zero then one hasn't been set yet
                // so leave the modified field as blank
                time_t modify = m_configItem->getDateModified();
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

                // The create time should always be set so we don't need to perform the same checks as the
                // modify date.
                COleDateTime dateCreated ( m_configItem->getDateCreated() );
				// TD12474 ++
				//createDate.SetWindowText( dateCreated.Format(TA_Base_App::TA_Configuration::DATE_FORMAT) );
				std::string dateFormatStr = dateFormat.getDateFormat(TA_Base_Core::DateFormat::DateTimeFormat6);
				m_createDate.SetWindowText( dateCreated.Format(dateFormatStr.c_str()) );
				// ++ TD12474

                // Enable the reset button for the user
                GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(TRUE);
            }
            
			showItemDetails(true);
            refreshButtonAvailability();
			// 8134-Start
			//Find the key from the map if the getKey returns 0
			//populateStationList(m_configItem->getKey());
			unsigned long lKey = m_configItem->getKey();
			if(lKey == 0)
				lKey = m_listView->getCopiedItemKey(m_configItem);
			populateStationList(lKey);
			// 8134-End
			
        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Cannot retrieve ECS Master Mode details to display");
            
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
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Cannot retrieve ECS Master Mode details to display");

            //UE-010001
            CString errorMsg("Database cannot be accessed by the ");
            errorMsg += RunParams::getInstance().get(RPARAM_APPNAME).c_str();
            errorMsg += ".";
			// TD14164 ++
            /*MessageBox(errorMsg,RunParams::getInstance().get(RPARAM_APPNAME).c_str(),MB_OK|MB_ICONSTOP);*/
			TA_Base_Bus::TransActiveMessage userMsg;
			CString fieldName = "ECS Master Mode";
			userMsg << fieldName;
			userMsg << errorMsg;
			userMsg.showMsgBox(IDS_UE_030067);
			// ++ TD14164
        }

        FUNCTION_EXIT;
    }


	void ECSMasterModeDetailView::populateStationList(unsigned long masterModeKey)
	{
		// TO DO: Add code to refresh data in the list.
		m_stationListCtrl.DeleteAllItems();

	
		TA_IRS_Core::IECSStationModes stationModes;
		try
		{
			stationModes = TA_IRS_Core::ECSStationModeAccessFactory::getInstance().getECSStationModesByMasterModeKey(masterModeKey);

			// Use a reverse iterator to preserve order as new elements are inserted at the top of the list.
			for(std::map<unsigned long, std::string>::reverse_iterator mapItr = m_allLocationKeysVector.rbegin(); mapItr != m_allLocationKeysVector.rend(); mapItr++)
			{
				bool isFound = false;
				int i = 0;
				for(std::vector<TA_IRS_Core::IECSStationMode*>::iterator itr = stationModes.begin(); itr != stationModes.end(); itr++)
				{
					if((*itr)->getLocationKey() == mapItr->first)
					{
						int pos = m_stationListCtrl.InsertItem(i,"",0);
						std::ostringstream stationModeKeyOstr;
						std::ostringstream stationModeOstr;

						stationModeKeyOstr << (*itr)->getKey();
						stationModeOstr << (*itr)->getStationMode();

						m_stationListCtrl.SetItemText(pos, STATION_MODE_KEY_COLUMN, (stationModeKeyOstr.str()).c_str());
						m_stationListCtrl.SetItemText(pos, LOCATION_COLUMN, (m_allLocationKeysVector[(*itr)->getLocationKey()]).c_str());
						m_stationListCtrl.SetItemText(pos, STATION_MODE_COLUMN, (stationModeOstr.str()).c_str());
						m_stationListCtrl.SetItemText(pos, OPERATION_COLUMN, ((*itr)->getOperation()).c_str());
						m_stationListCtrl.SetItemText(pos, TRACK_COLUMN, ((*itr)->getTrack()).c_str());
						isFound = true;
					}

				}

				if(!isFound)
				{

					int pos = m_stationListCtrl.InsertItem(i,"",0);
					std::ostringstream locationStr;
					locationStr <<mapItr->first;

					m_stationListCtrl.SetItemText(pos, LOCATION_COLUMN, (mapItr->second).c_str());
					m_stationListCtrl.SetItemText(pos, STATION_MODE_COLUMN, "");
					m_stationListCtrl.SetItemText(pos, OPERATION_COLUMN, "");
					m_stationListCtrl.SetItemText(pos, TRACK_COLUMN, "");
				}

				i++;
			}

			
			
			// Now clean up pointers when we're done with it.
			for(unsigned int i=0; i<stationModes.size(); i++)
			{
				if(stationModes[i] != NULL)
				{
					delete stationModes[i];
					stationModes[i] = 0;
				}
			}
		}
		catch(TA_Base_Core::DataException&)
		{
			LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Cannot retrieve corresponding Station Mode details");
			
			for(unsigned int i=0; i<stationModes.size(); i++)
			{
				if(stationModes[i] != NULL)
				{
					delete stationModes[i];
					stationModes[i] = 0;
				}
			}

			// TD14164 ++
			/*AfxMessageBox("Failed to retrieve associated station modes from the database!");*/
			TA_Base_Bus::TransActiveMessage userMsg;
			userMsg.showMsgBox(IDS_UE_520008);
			// ++ TD14164
		}
		catch(TA_Base_Core::DatabaseException&)
		{
			LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Cannot retrieve ECS Station Mode details");
			
			for(unsigned int i=0; i<stationModes.size(); i++)
			{
				if(stationModes[i] != NULL)
				{
					delete stationModes[i];
					stationModes[i] = 0;
				}
			}

			// TD14164 ++
			/*AfxMessageBox("Failed to retrieve associated station modes from the database!");*/
			TA_Base_Bus::TransActiveMessage userMsg;
			userMsg.showMsgBox(IDS_UE_520008);
			// ++ TD14164
		}
		catch( ... )
		{
			LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Unknown exception");

			for(unsigned int i=0; i<stationModes.size(); i++)
			{
				if(stationModes[i] != NULL)
				{
					delete stationModes[i];
					stationModes[i] = 0;
				}
			}

			TA_Base_Bus::TransActiveMessage userMsg;
			userMsg.showMsgBox(IDS_UE_520008);
		}

	}


    void ECSMasterModeDetailView::refreshButtonAvailability()
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

    void ECSMasterModeDetailView::onButtonECSZone() 
    {
        FUNCTION_ENTRY("onButtonECSZone");
	    
        // Retrieve the current ecsZone this ECS Master Mode is using
        CString currentECSZone("");
        m_ecsZoneEdt.GetWindowText( currentECSZone );

        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        TA_Base_App::ChangeDialog::ItemsForSelection items = m_databaseAccessor->getAllECSZones();

        // Now construct the dialog and then wait for the user to finish with it
        TA_Base_App::ChangeDialog dlg(items, CHANGE_ECS_ZONE_DIALOG_TITLE, currentECSZone, this);
        int response = dlg.DoModal();

        if (response != IDOK)
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "User cancelled the selection of an ECS Zone");
            FUNCTION_EXIT;
            return;
        }

        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "User selected a new ECS Zone which is %s", dlg.getSelectedName());

        try
        {
            TA_ASSERT(m_configItem !=  NULL, "m_configItem is NULL.");

            if (currentECSZone != dlg.getSelectedName())
            {
                // If this is a different status datapoint then set it in our database object.
                m_configItem->setECSZoneId( dlg.getSelectedKey() );

                // Now update the entity type the details view is showing
                m_ecsZoneEdt.SetWindowText( dlg.getSelectedName() );
                m_ecsZoneEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );

    	        m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
            }
        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Could not retrieve record so cannot update existing");
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Could not retrieve record so cannot update existing");
        }

        refreshButtonAvailability();
        FUNCTION_EXIT;
    }

    void ECSMasterModeDetailView::onKillfocusEditMasterMode() 
    {
        FUNCTION_ENTRY("onKillfocusEditMasterMode");
        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        if (!m_configItem) 
        {
            // For the odd occasion that the operator has clicked new item straight after modifying
            // this field
            return;
        }

        CString masterModeString;
        m_masterModeEdt.GetWindowText(masterModeString);
        unsigned int masterMode = atoi((LPCTSTR)masterModeString);

        try
        {
            // If the masterMode has not changed then there is no point doing an update
            if (masterMode == m_configItem->getMasterMode())
            {
                m_masterModeEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
                FUNCTION_EXIT;
                return;
            }

            m_configItem->setMasterMode( masterMode );
            m_masterModeEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database","The Master Mode field could not be set");

            m_masterModeEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );

            FUNCTION_EXIT;
            return;
        }

	    m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
        refreshButtonAvailability();

        FUNCTION_EXIT;
    }

    void ECSMasterModeDetailView::onKillfocusEditCondition() 
    {
        FUNCTION_ENTRY("onKillfocusEditCondition");
        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        if (!m_configItem) 
        {
            // For the odd occasion that the operator has clicked new item straight after modifying
            // this field
            return;
        }

        CString conditionString;
        m_conditionEdt.GetWindowText(conditionString);

        try
        {
            // If the condition string has not changed then there is no point doing an update
            if (conditionString == m_configItem->getCondition().c_str())
            {
                
                m_conditionEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
                FUNCTION_EXIT;
                return;
            }

            m_configItem->setCondition( (LPCTSTR) conditionString );
            m_conditionEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database","The Condition field could not be set");

            m_conditionEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );

            FUNCTION_EXIT;
            return;
        }

	    m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
        refreshButtonAvailability();

        FUNCTION_EXIT;
    }
    
    void ECSMasterModeDetailView::onKillfocusEditECSZone() 
    {
        FUNCTION_ENTRY("onKillfocusEditECSZone");
        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        if (!m_configItem) 
        {
            // For the odd occasion that the operator has clicked new item straight after modifying
            // this field
            return;
        }

        CString ecsZoneString;
        m_ecsZoneEdt.GetWindowText(ecsZoneString);
        long ecsZoneKey = m_databaseAccessor->getZoneKey((LPCTSTR)ecsZoneString);

        // Note - ecsZoneKey should be -1 for a null entry (which is valid)
        if (ecsZoneKey == -2)
        {
            m_ecsZoneEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
            return;
        }

        try
        {
            // If the ecsZone string has not changed then there is no point doing an update
            if ( ecsZoneKey == m_configItem->getECSZoneId())
            {
                m_ecsZoneEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
                FUNCTION_EXIT;
                return;
            }
            
            m_configItem->setECSZoneId( ecsZoneKey );
            m_ecsZoneEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database","The ECS Zone field could not be set");

            m_ecsZoneEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );

            FUNCTION_EXIT;
            return;
        }

	    m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
        refreshButtonAvailability();

        FUNCTION_EXIT;
    }

    void ECSMasterModeDetailView::onKillfocusEditEvacuation() 
    {
        FUNCTION_ENTRY("onKillfocusEditEvacuation");
        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        if (!m_configItem) 
        {
            // For the odd occasion that the operator has clicked new item straight after modifying
            // this field
            return;
        }

        CString evacuationString;
        m_evacuationEdt.GetWindowText(evacuationString);

        try
        {
            // If the evacuation string has not changed then there is no point doing an update
            if (evacuationString == m_configItem->getEvacuationDirection().c_str())
            {
                
                m_evacuationEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
                FUNCTION_EXIT;
                return;
            }

            m_configItem->setEvacuationDirection( (LPCTSTR) evacuationString );
            m_evacuationEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database","The Evacuation Direction field could not be set");

            m_evacuationEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );

            FUNCTION_EXIT;
            return;
        }

	    m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
        refreshButtonAvailability();

        FUNCTION_EXIT;
    }

    void ECSMasterModeDetailView::onKillfocusEditAirflow() 
    {
        FUNCTION_ENTRY("onKillfocusEditAirflow");
        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        if (!m_configItem) 
        {
            // For the odd occasion that the operator has clicked new item straight after modifying
            // this field
            return;
        }

        CString airflowString;
        m_airflowEdt.GetWindowText(airflowString);

        try
        {
            // If the airflow string has not changed then there is no point doing an update
            if (airflowString == m_configItem->getAirflowDirection().c_str())
            {
                
                m_airflowEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
                FUNCTION_EXIT;
                return;
            }

            m_configItem->setAirflowDirection( (LPCTSTR) airflowString );
            m_airflowEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database","The Airflow Direction field could not be set");

            m_airflowEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );

            FUNCTION_EXIT;
            return;
        }

	    m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
        refreshButtonAvailability();

        FUNCTION_EXIT;
    }

    void ECSMasterModeDetailView::onKillfocusEditDisplay() 
    {
        FUNCTION_ENTRY("onKillfocusEditDisplay");
        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        if (!m_configItem) 
        {
            // For the odd occasion that the operator has clicked new item straight after modifying
            // this field
            return;
        }

        CString displayString;
        m_displayEdt.GetWindowText(displayString);

        try
        {
            // If the display string has not changed then there is no point doing an update
            if (displayString == m_configItem->getDisplayDirection().c_str())
            {
                
                m_displayEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
                FUNCTION_EXIT;
                return;
            }

            m_configItem->setDisplayDirection( (LPCTSTR) displayString );
            m_displayEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database","The Display Direction field could not be set");

            m_displayEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );

            FUNCTION_EXIT;
            return;
        }

	    m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
        refreshButtonAvailability();

        FUNCTION_EXIT;
    }


   void ECSMasterModeDetailView::onKillfocusEditVerification() 
    {
        FUNCTION_ENTRY("onKillfocusEditVerification");
        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        if (!m_configItem) 
        {
            // For the odd occasion that the operator has clicked new item straight after modifying
            // this field
            return;
        }

        CString displayString;
        m_verificationMsgEdt.GetWindowText(displayString);

        try
        {
            // If the display string has not changed then there is no point doing an update
            if (displayString == m_configItem->getVerificationMessage().c_str())
            {
                m_verificationMsgEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
                FUNCTION_EXIT;
                return;
            }

            m_configItem->setVerificationMessage( (LPCTSTR) displayString );
            m_verificationMsgEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database","The Display Direction field could not be set");

            m_verificationMsgEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );

            FUNCTION_EXIT;
            return;
        }

	    m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
        refreshButtonAvailability();

        FUNCTION_EXIT;
    }

   void ECSMasterModeDetailView::onKillfocusEditSituation() 
   {
	   FUNCTION_ENTRY("onKillfocusEditSituation");
	   TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

	   if (!m_configItem) 
	   {
		   // For the odd occasion that the operator has clicked new item straight after modifying
		   // this field
		   return;
	   }

	   CString situationString;
	   m_situationEdt.GetWindowText(situationString);

	   try
	   {
		   // If the condition string has not changed then there is no point doing an update
		   if (situationString == m_configItem->getSituation().c_str())
		   {

			   m_situationEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
			   FUNCTION_EXIT;
			   return;
		   }

		   m_configItem->setSituation( (LPCTSTR) situationString );
		   m_situationEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
	   }
	   catch( ... )
	   {
		   LOG_EXCEPTION_CATCH(SourceInfo, "Database","The Condition field could not be set");

		   m_situationEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );

		   FUNCTION_EXIT;
		   return;
	   }

	   m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
	   refreshButtonAvailability();

	   FUNCTION_EXIT;
   }

    void ECSMasterModeDetailView::onButtonReset() 
    {
        FUNCTION_ENTRY("onButtonReset");

        resetButtonPressed(m_configItem);

        FUNCTION_EXIT;
    }


    void ECSMasterModeDetailView::onButtonApply() 
    {
	    FUNCTION_ENTRY("onButtonApply");

        // Under normal circumstances, the onKillfocus*() methods should be called to update the underlying 
        // IConfig objects.  However, when the operator presses the 'Enter' key, this method is called without
        // first calling onKillfocus*() for the previous widget.  This following line should force the implicit
        // call to the onKillfocus*() method.  
        m_applyBtn.SetFocus();

        if ( !m_masterModeEdt.isTextValid() || 
             !m_conditionEdt.isTextValid()  || 
             !m_ecsZoneEdt.isTextValid()    || 
             !m_evacuationEdt.isTextValid() ||
             !m_airflowEdt.isTextValid()    ||  
             !m_displayEdt.isTextValid()    ||
			 !m_situationEdt.isTextValid())  
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
		
		// 8134-Start
		// Code hack.even when apply button is pressed the function applyButtonPressed() doesnt
		// return the error code.hence check if the key has been really set to a non-zero before deleting
		if(m_configItem->getKey() != 0)
			m_listView->DeleteCopiedItemKeyEntry(m_configItem);
		// 8134-End

		// 7196-Start
		// Send online Update Message
		sendUpdateMessageForChangeItem();
		// 7196-End

        FUNCTION_EXIT;
    }

		// 7196-Start
		// This function sends an update message when there is a change in the table data
		// The ECS Manager has to recieve this update message and update the display
		void ECSMasterModeDetailView::sendUpdateMessageForChangeItem()
		{
			FUNCTION_ENTRY("sendUpdateMessageForChangeItem");
			
			UpdateData(TRUE);
			CString strWindowText;
			TA_Base_App::MessageSender::UpdateDetails updateDetails;        
			TA_Base_Core::Values values;
			updateDetails.isNewItem = m_configItem->isNew();
			updateDetails.isDelete  = false;        
			updateDetails.keyOfChangedItem = TA_Base_Core::DatabaseKey::getInvalidKey();
			updateDetails.configUpdateMessage = getConfigMessageType();		
		
			
			// Master Mode
			m_masterModeEdt.GetWindowText(strWindowText);
			values.newValue								 =  LPCSTR(strWindowText);
			updateDetails.changes["Master Mode"]		 = values;
			values.newValue								 = "";
			strWindowText								 = "";
			// Operating Condition
			m_conditionEdt.GetWindowText(strWindowText);
			values.newValue						 =  LPCSTR(strWindowText);		
			updateDetails.changes["Operating Condition"] = values;
			values.newValue								 =  "";
			strWindowText								 = "";
			// ECS Zone		
			m_ecsZoneEdt.GetWindowText(strWindowText);		
			values.newValue							     =  LPCSTR(strWindowText);		
			updateDetails.changes["ECS Zone"]			 = values;
			values.newValue								 =  "";
			strWindowText								 = "";
			// Evacuvation Direction		
			m_evacuationEdt.GetWindowText(strWindowText);
			values.newValue								 =  LPCSTR(strWindowText);		
			updateDetails.changes["Evacuation Direction"]= values;
			values.newValue								 =  "";
			strWindowText								 = "";
			// AirFlow Direction		
			m_airflowEdt.GetWindowText(strWindowText);
			values.newValue								 = LPCSTR(strWindowText);		
			updateDetails.changes["Airflow Direction"]   = values;
			values.newValue							     =  "";
			strWindowText								 = "";
			// Display Direction		
			m_displayEdt.GetWindowText(strWindowText);
			values.newValue								 = LPCSTR(strWindowText);				
			updateDetails.changes["Display Direction"]   = values;
			values.newValue							     =  "";
			strWindowText								 = "";
			//Verification Message		
			m_verificationMsgEdt.GetWindowText(strWindowText);
			values.newValue								 =  LPCSTR(strWindowText);						
			updateDetails.changes["Verification Message"]= values;
			strWindowText								 = "";
			
			//Condition		
			m_situationEdt.GetWindowText(strWindowText);
			values.newValue								 =  LPCSTR(strWindowText);						
			updateDetails.changes["Situation"]= values;
			strWindowText					  = "";

			// We update the config details with the new values.
			// Send Online Update Message now
			getMessageSender()->sendMessagesForTheChange( updateDetails );   

			FUNCTION_EXIT;
		}
		// 7196-End

	void ECSMasterModeDetailView::removeListItem()
	{
		POSITION pos = m_stationListCtrl.GetFirstSelectedItemPosition();
		if (pos == NULL)
		{
			 return;
		}

		int index = m_stationListCtrl.GetNextSelectedItem(pos);
		
		CString oldStationModeKey = m_stationListCtrl.GetItemText(index, STATION_MODE_KEY_COLUMN);
		
		
		if(!oldStationModeKey.IsEmpty())
		{
			m_stationListCtrl.SetItemText(index, STATION_MODE_KEY_COLUMN, "");
			m_stationListCtrl.SetItemText(index, STATION_MODE_COLUMN, "");
			m_stationListCtrl.SetItemText(index, OPERATION_COLUMN, "");
			m_stationListCtrl.SetItemText(index, TRACK_COLUMN, "");

			m_configItem->setAssociatedStationMode(atoi(oldStationModeKey), false);

			GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(true);
			GetDlgItem(IDC_BUTTON_APPLY)->EnableWindow(true);

		}		
		
		// 8134-Start
		// Here they are just interested in deleting the entry from the list control
		// hence delete from our temporary map too.
		m_listView->DeleteCopiedItemKeyEntry(m_configItem);
		// 8134-End
	}


	void ECSMasterModeDetailView::setListItem(unsigned long stationModePKey)
	{
		try
		{
			// First update the list control..

			//AstationMode = TA_IRS_Core::ECSStationModeAccessFactory::getInstance().getECSStationModesWhere(true, locationKey, stationModeKey, "", "", "");
			std::auto_ptr<TA_IRS_Core::IECSStationMode> stationMode(TA_IRS_Core::ECSStationModeAccessFactory::getInstance().getECSStationMode(stationModePKey, true));

			POSITION pos = m_stationListCtrl.GetFirstSelectedItemPosition();
			if (pos == NULL)
			{
				 return;
			}

			int index = m_stationListCtrl.GetNextSelectedItem(pos);
			std::ostringstream stationModeOstr;
			std::ostringstream stationModeKeyOstr;

			stationModeOstr << stationMode->getStationMode();
			stationModeKeyOstr << stationMode->getKey();
			
			CString oldStationModeKey = m_stationListCtrl.GetItemText(index, STATION_MODE_KEY_COLUMN);
			
			m_stationListCtrl.SetItemText(index, STATION_MODE_KEY_COLUMN, (stationModeKeyOstr.str()).c_str());
			//m_stationListCtrl.SetItemText(index, LOCATION_COLUMN, (m_allLocationKeysVector[stationMode->getLocationKey()]).c_str());
			m_stationListCtrl.SetItemText(index, STATION_MODE_COLUMN, (stationModeOstr.str()).c_str());
			m_stationListCtrl.SetItemText(index, OPERATION_COLUMN, (stationMode->getOperation()).c_str());
			m_stationListCtrl.SetItemText(index, TRACK_COLUMN, (stationMode->getTrack()).c_str());

			if(!oldStationModeKey.IsEmpty())
			{
				m_configItem->setAssociatedStationMode(atoi(oldStationModeKey), false);
			}
			m_configItem->setAssociatedStationMode(stationModePKey, true);

			refreshButtonAvailability();

		}
		catch(TA_Base_Core::DataException&)
		{
			LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Cannot retrieve corresponding Station Mode details");
			
			// TD14164 ++
			/*AfxMessageBox("Failed to retrieve associated station modes from the database!");*/
			TA_Base_Bus::TransActiveMessage userMsg;
			userMsg.showMsgBox(IDS_UE_520008);
			// ++ TD14164

		}
		catch(TA_Base_Core::DatabaseException&)
		{
			LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Cannot retrieve ECS Station Mode details");
	
			// TD14164 ++
			/*AfxMessageBox("Failed to retrieve associated station modes from the database!");*/
			TA_Base_Bus::TransActiveMessage userMsg;
			userMsg.showMsgBox(IDS_UE_520008);
			// ++ TD14164
		}
		catch(...)
		{
			//Perhaps the auto ptr is NULL
			// TD14164 ++
			/*AfxMessageBox("Configuration error! No station mode retrieved for the given station ID.\n Aborting operation");*/
			TA_Base_Bus::TransActiveMessage userMsg;
			userMsg.showMsgBox(IDS_UE_520009);
			// ++ TD14164
		}

		
	}
  
 
	void ECSMasterModeDetailView::OnEditStationButton() 
	{
		// start

		POSITION pos = m_stationListCtrl.GetFirstSelectedItemPosition();
		if (pos == NULL)
		{
			 return; 
		}

        // Retrieve the current ecsZone this ECS Master Mode is using
		CString dialogTitle = "Station Selection ";
		//int locationKey = m_numberOfLocations - (m_stationListCtrl.GetNextSelectedItem(pos));
		int locationKey = (m_stationListCtrl.GetNextSelectedItem(pos))+1; //20180

		CString currentLocation = (m_allLocationKeysVector[locationKey]).c_str();
		dialogTitle += currentLocation;
		

        TA_Base_App::ChangeDialog::ItemsForSelection items;

		TA_IRS_Core::IECSStationModes stationModesForGivenLocation;
		try
		{
			stationModesForGivenLocation = TA_IRS_Core::ECSStationModeAccessFactory::getInstance().getECSStationModesWhere(true, locationKey, -1, "", "","");
			
			if(stationModesForGivenLocation.size() == 0)
			{
				// TD14164 ++
				/*AfxMessageBox("There are no station mode associations configured for this location.");*/
				TA_Base_Bus::TransActiveMessage userMsg;
				userMsg.showMsgBox(IDS_UE_520010);
				// ++ TD14164
				m_editStationButton.EnableWindow();
				return;
			}

			// Have to order stationModes by location here.. descending order.
			unsigned int size = stationModesForGivenLocation.size();
			for(unsigned int i=0; i<size; i++)
			{
				std::string listItem;
				listItem += "Station Mode: ";
				
				unsigned long stationModeKey = stationModesForGivenLocation[i]->getStationMode();
				unsigned long stationModePKey = stationModesForGivenLocation[i]->getKey();
				std::ostringstream stationModeOstr;
				stationModeOstr << stationModeKey;
				listItem += stationModeOstr.str();
				listItem += " Operation: ";
				listItem += stationModesForGivenLocation[i]->getOperation();
				listItem += " Track: ";
				listItem += stationModesForGivenLocation[i]->getTrack();

				items[stationModePKey] = listItem.c_str();

			}
			// We want this to appear as the last entry in the list box.
				items[99999999] = "<Remove Station Association>";

		}
		catch(TA_Base_Core::DataException&)
		{
			LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Cannot retrieve corresponding Station Mode details");
			
			for(unsigned int i=0; i<stationModesForGivenLocation.size(); i++)
			{
				if(stationModesForGivenLocation[i] != NULL)
				{
					delete stationModesForGivenLocation[i];
					stationModesForGivenLocation[i] = 0;
				}
			}

			// TD14164 ++
			/*AfxMessageBox("Failed to retrieve associated station modes from the database!");*/
			TA_Base_Bus::TransActiveMessage userMsg;
			userMsg.showMsgBox(IDS_UE_520008);
			// ++ TD14164
			return;
		}
		catch(TA_Base_Core::DatabaseException&)
		{
			LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Cannot retrieve ECS Station Mode details");
			
			for(unsigned int i=0; i<stationModesForGivenLocation.size(); i++)
			{
				if(stationModesForGivenLocation[i] != NULL)
				{
					delete stationModesForGivenLocation[i];
					stationModesForGivenLocation[i] = 0;
				}
			}

			// TD14164 ++
			/*AfxMessageBox("Failed to retrieve associated station modes from the database!");*/
			TA_Base_Bus::TransActiveMessage userMsg;
			userMsg.showMsgBox(IDS_UE_520008);
			// ++ TD14164
			return;
		}
		catch( ... )
		{
			LOG( SourceInfo, DebugUtil::ExceptionCatch, "UnknownException", "Cannot retrieve ECS Station Mode details");

			for(unsigned int i=0; i<stationModesForGivenLocation.size(); i++)
			{
				if(stationModesForGivenLocation[i] != NULL)
				{
					delete stationModesForGivenLocation[i];
					stationModesForGivenLocation[i] = 0;
				}
			}

			TA_Base_Bus::TransActiveMessage userMsg;
			userMsg.showMsgBox(IDS_UE_520008);
			return;
		}

		// We're now done with it, free memory.
		for(unsigned int i=0; i<stationModesForGivenLocation.size(); i++)
		{
			if(stationModesForGivenLocation[i] != NULL)
			{
				delete stationModesForGivenLocation[i];
				stationModesForGivenLocation[i] = 0;
			}
		}

        // Now construct the dialog and then wait for the user to finish with it
        TA_Base_App::ChangeDialog dlg(items, dialogTitle, currentLocation, this);
        int response = dlg.DoModal();

        if (response != IDOK)
        {
			m_editStationButton.EnableWindow(false);
            return;
        }


		if(dlg.getSelectedKey() == 99999999)
		{
			removeListItem();
		}
		else
		{
			setListItem(dlg.getSelectedKey());
		}

		m_editStationButton.EnableWindow(false);

	}

	void ECSMasterModeDetailView::OnClickStationList(NMHDR* pNMHDR, LRESULT* pResult) 
	{
		// TODO: Add your control notification handler code here
		m_editStationButton.EnableWindow();
		*pResult = 0;
	}

    /////////////////////////////////////////////////////////////////////////////
    // ECSMasterModeDetailView diagnostics

    #ifdef _DEBUG
    void ECSMasterModeDetailView::AssertValid() const
    {
	    AbstractDetailView::AssertValid();
    }

    void ECSMasterModeDetailView::Dump(CDumpContext& dc) const
    {
	    AbstractDetailView::Dump(dc);
    }
    #endif //_DEBUG

}





