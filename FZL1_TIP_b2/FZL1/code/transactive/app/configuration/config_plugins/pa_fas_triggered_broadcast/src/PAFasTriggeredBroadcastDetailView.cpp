/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3002_TIP/3002/transactive/app/configuration/config_plugins/pa_fas_triggered_broadcast/src/PAFasTriggeredBroadcastDetailView.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2009/10/01 16:36:38 $
  * Last modified by:  $Author: grace.koh $
  * 
  * This view allows the user to configure an PA FAS Triggered Broadcast's details.
  */

#pragma warning(disable:4786 4284)

#include "StdAfx.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/ChangeDialog.h"
#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"
#include "core/data_access_interface/pa/src/IConfigPaFasTriggeredBroadcast.h"

#include "bus/generic_gui/src/TransactiveMessage.h" // TD14164
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"

#include "core/data_access_interface/src/DatabaseKey.h"
#include "bus/mfc_extensions/src/selection_helpers/SelectionHelper.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/FieldValidityChecker.h"

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
    // The titles for the change dialog box
    const CString PaFasTriggeredBroadcastDetailView::CHANGE_LOCATION_DIALOG_TITLE = "选择车站";
    const CString PaFasTriggeredBroadcastDetailView::SELECT_ZONE_GROUP_TITLE = "选择广播区域组";
    const CString PaFasTriggeredBroadcastDetailView::SELECT_DVA_MESSAGE_TITLE = "选择预存广播消息";
    const CString PaFasTriggeredBroadcastDetailView::SELECT_FAS_ALARM_ENTITY_TITLE = "选择火灾报警点";
    const unsigned int MIN_PA_CYLIC_TIMES = 1;
    const unsigned int MAX_PA_CYLIC_TIMES = 255;
 
    const int PaFasTriggeredBroadcastDetailView::LOCATION_FIELD_LENGTH           = 40;

    IMPLEMENT_DYNCREATE(PaFasTriggeredBroadcastDetailView, CFormView)

    PaFasTriggeredBroadcastDetailView::PaFasTriggeredBroadcastDetailView() : 
        AbstractDetailView(PaFasTriggeredBroadcastDetailView::IDD),
        m_configItem(NULL),
        m_listView(NULL),
        m_databaseAccessor(NULL),
        m_applyBtn(),
        m_locationEdt(true),
        m_zoneGroupEdt(true),
        m_cyclicTimesEdit(),
        m_dvaMessageEdt(true),
        m_fasAlarmEntityEdt(true)
    {
	    //{{AFX_DATA_INIT(PaFasTriggeredBroadcastDetailView)
	    //}}AFX_DATA_INIT
    }      

    PaFasTriggeredBroadcastDetailView::~PaFasTriggeredBroadcastDetailView()
    {
    }


    void PaFasTriggeredBroadcastDetailView::DoDataExchange(CDataExchange* pDX)
    {
	    CFormView::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(PaFasTriggeredBroadcastDetailView)
    	DDX_Control(pDX, IDC_BUTTON_APPLY, m_applyBtn);
    	DDX_Control(pDX, IDC_EDIT_LOCATION, m_locationEdt);    	
    	DDX_Control(pDX, IDC_EDIT_ZONE_GROUP, m_zoneGroupEdt);
    	DDX_Control(pDX, IDC_EDIT_CYCLIC_TIMES, m_cyclicTimesEdit);        
        DDX_Control(pDX, IDC_EDIT_STATION_DVA_MESSAGE, m_dvaMessageEdt);        
        DDX_Control(pDX, IDC_EDIT_FAS_ALARM_ENTITY, m_fasAlarmEntityEdt);        
        DDX_Control(pDX, IDC_CHECK_CYCLIC, m_cyclicCheck);
        DDX_Control(pDX, IDC_CHECK_IS_ENABLED, m_isEnabledCheck);
        DDX_Control(pDX, IDC_STATIC_MODIFYDATE, m_modifyDate);
    	DDX_Control(pDX, IDC_STATIC_CREATEDATE, m_createDate);
        DDX_Control(pDX, IDC_STATIC_TYPE, m_type);
        DDX_Control(pDX, IDC_LOCATION_BITMAP, m_transparentImageLocation);
    	DDX_Control(pDX, IDC_ZONE_GROUP_BITMAP, m_transparentImageZoneGroup);
    	DDX_Control(pDX, IDC_DVA_MESSAGE_BITMAP, m_transparentImageDvaMessage);
        DDX_Control(pDX, IDC_FAS_ALARM_ENTITY_BITMAP, m_transparentImageFasAlarmEntity);
        DDX_Control(pDX, IDC_SPIN_CYCLIC_TIMES, m_cylicTimesSpin);
    	//}}AFX_DATA_MAP
    }


BEGIN_MESSAGE_MAP(PaFasTriggeredBroadcastDetailView, AbstractDetailView)
	//{{AFX_MSG_MAP(PaFasTriggeredBroadcastDetailView)
	ON_BN_CLICKED(IDC_BUTTON_RESET, onButtonReset)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, onButtonApply)
    ON_BN_CLICKED(IDC_BUTTON_LOCATION, onButtonLocation)
    ON_BN_CLICKED(IDC_BUTTON_ZONE_GROUP, onButtonZoneGroup)
    ON_BN_CLICKED(IDC_BUTTON_STATION_DVA_MESSAGE, onButtonStationDvaMessage)
    ON_BN_CLICKED(IDC_BUTTON_FAS_ALARM_ENTITY, onButtonFasAlarmEntity)
    ON_BN_CLICKED(IDC_CHECK_IS_ENABLED, onButtonIsEnabled)
    ON_BN_CLICKED(IDC_CHECK_CYCLIC, onButtonCyclicEnable)
    ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_CYCLIC_TIMES, onDeltaposSpinCyclicTimes)

    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

    void PaFasTriggeredBroadcastDetailView::OnInitialUpdate() 
    {
        FUNCTION_ENTRY("OnInitialUpdate");

	    CFormView::OnInitialUpdate();
	    
        m_locationEdt.SetLimitText(LOCATION_FIELD_LENGTH);
        m_cylicTimesSpin.SetBuddy(GetDlgItem(IDC_EDIT_CYCLIC_TIMES));
        m_cylicTimesSpin.SetRange( MIN_PA_CYLIC_TIMES, MAX_PA_CYLIC_TIMES );

        // Hide all the PA FAS Triggered Broadcast details until we are told to populate.
	    showItemDetails(false);

        FUNCTION_EXIT;
    }


    void PaFasTriggeredBroadcastDetailView::showItemDetails(bool isShown)
    {        
        // Show or hide all the controls from the view
        CWnd* nextChild = GetWindow(GW_CHILD);

        while (0 != nextChild)
        {
            nextChild->ShowWindow(isShown);
    
            nextChild = nextChild->GetWindow(GW_HWNDNEXT);
        }
    }


    void PaFasTriggeredBroadcastDetailView::populateItemDetails(TA_Base_Core::IConfigItem* currentItem)
    {
        // This can take a while sometimes (mainly populating zone list)..
        CWaitCursor wait;

        FUNCTION_ENTRY("populateItemDetails");

        if (m_databaseAccessor == NULL || m_listView == NULL)
        {
            m_databaseAccessor = dynamic_cast<PaFasTriggeredBroadcastDatabaseAccessor*>(getDatabaseAccessor());
            m_listView = dynamic_cast<PaFasTriggeredBroadcastListView*>(getListView());

            TA_ASSERT(m_databaseAccessor != NULL, "setDatabaseAccessor() must be called before this method");
            TA_ASSERT(m_listView != NULL, "The list view has not been set yet or is not a proper PaFasTriggeredBroadcastDetailView");
        }


        m_configItem = dynamic_cast<TA_Base_Core::IConfigPaFasTriggeredBroadcast*>(currentItem);

        if (m_configItem == NULL)
        { 
            showItemDetails(false);
            FUNCTION_EXIT;
            return;
        }

        try
        {   
            PaFasTriggeredBroadcastDatabaseAccessor::CfgPaFasTriggeredBroadcastRecord record =
                            m_databaseAccessor->getRecordFrom(*m_configItem);

            //  Now populate all the fields with the data retrieved from the IConfigPaFasTriggeredBroadcast* object
            m_locationEdt.SetWindowText((LPCTSTR)record.location);
            m_locationEdt.setTextType(m_locationEdt.GetWindowTextLength() > 0 ? 
                                TA_Base_Bus::TextColourEdit::VALID : TA_Base_Bus::TextColourEdit::INVALID);

            refreshZoneGroupEdit();
            refreshDvaMessageEdit();
            refreshFasAlarmEntityEdit();

            m_isEnabledCheck.SetCheck(m_configItem->getIsEnabledParameter() ? BST_CHECKED : BST_UNCHECKED);
            m_cyclicCheck.SetCheck(( m_configItem->getPaCyclicTimes() > 0 ) ? BST_CHECKED : BST_UNCHECKED );
            BOOL bEnableCyclic = ( m_configItem->getPaCyclicTimes() > 0 ) ? TRUE : FALSE;
            if ( bEnableCyclic )
            {
                m_cylicTimesSpin.SetPos( m_configItem->getPaCyclicTimes() );
            }
            else
            {
                m_cylicTimesSpin.SetPos( MIN_PA_CYLIC_TIMES );
            }
            m_cylicTimesSpin.EnableWindow( bEnableCyclic );
            m_cyclicTimesEdit.EnableWindow( bEnableCyclic );

            if( m_configItem->isNew() )
            {
                // This is a new PA FAS Triggered Broadcast or at least a copy of an PA FAS Triggered Broadcast so we indicate this in the type
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
					std::string dateFormatStr = dateFormat.getDateFormat(TA_Base_Core::DateFormat::DateTimeFormat6);
					m_modifyDate.SetWindowText( dateModified.Format(dateFormatStr.c_str()) );
                }

                // The create time should always be set so we don't need to perform the same checks as the
                // modify date.
                COleDateTime dateCreated ( m_configItem->getDateCreated() );
				std::string dateFormatStr = dateFormat.getDateFormat(TA_Base_Core::DateFormat::DateTimeFormat6);
				m_createDate.SetWindowText( dateCreated.Format(dateFormatStr.c_str()) );

                // Enable the reset button for the user
                GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(TRUE);
            }
            
            showItemDetails(true);
            refreshButtonAvailability();
        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Cannot retrieve PA FAS Triggered Broadcast details to display");

			TA_Base_Bus::TransActiveMessage userMsg;
			userMsg.showMsgBox(IDS_UE_030001);
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Cannot retrieve PA FAS Triggered Broadcast details to display");

            CString errorMsg("Database cannot be accessed by the ");
            errorMsg += RunParams::getInstance().get(RPARAM_APPNAME).c_str();
            errorMsg += ".";
			TA_Base_Bus::TransActiveMessage userMsg;
			CString fieldName = "PA FAS Triggered Broadcast";
			userMsg << fieldName;
			userMsg << errorMsg;
			userMsg.showMsgBox(IDS_UE_030067);
        }

        FUNCTION_EXIT;
    }


    void PaFasTriggeredBroadcastDetailView::refreshButtonAvailability()
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


    void PaFasTriggeredBroadcastDetailView::onButtonLocation() 
    {        
        FUNCTION_ENTRY("onButtonLocation");
	    
        if (displayLocationDialog(m_locationEdt))
        {
            onLocationChanged();
        }
    }

    bool PaFasTriggeredBroadcastDetailView::displayLocationDialog(TA_Base_Bus::TextColourEdit& locationEdit)
    {
        FUNCTION_ENTRY("displayLocationDialog");

        // Retrieve the current location this PA FAS Triggered Broadcast is using
        CString currentLocation("");
        locationEdit.GetWindowText( currentLocation );

        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        TA_Base_App::ChangeDialog::ItemsForSelection items = m_databaseAccessor->getAllLocations();

        // Now construct the dialog and then wait for the user to finish with it
        TA_Base_App::ChangeDialog dlg(items, CHANGE_LOCATION_DIALOG_TITLE, currentLocation, this);
        dlg.setSortItems( false );
        int response = dlg.DoModal();

        if (response != IDOK)
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "User cancelled the selection of a location");
            FUNCTION_EXIT;
            return false;
        }

        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "User selected a new location which is %s", dlg.getSelectedName());

        bool changesMade = false;

        try
        {
            TA_ASSERT(m_configItem !=  NULL, "m_configItem is NULL.");

            if (currentLocation != dlg.getSelectedName())
            {
                // Special case handling for config item update
                if (&locationEdit == &m_locationEdt)
                {
                    m_configItem->setLocationKey( dlg.getSelectedKey() );
                }
                else
                {
                    TA_ASSERT(FALSE, "Invalid control specified");
                }

                changesMade = true;

                // Now update the entity type the details view is showing
                locationEdit.SetWindowText( dlg.getSelectedName() );
                locationEdit.setTextType( TA_Base_Bus::TextColourEdit::VALID );

    	        m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
            }
        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Could not retrieve status datapoint key so cannot update existing");
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Could not retrieve status datapoint key so cannot update existing");
        }

        refreshButtonAvailability();
        FUNCTION_EXIT;

        return changesMade;
    }

    void PaFasTriggeredBroadcastDetailView::onButtonZoneGroup()
    {
        // Display zone group selection, location specific
        doSelectionDialog(PaFasTriggeredBroadcastDatabaseAccessor::DATA_ZONE_GROUP, true);        
    }

    void PaFasTriggeredBroadcastDetailView::onButtonStationDvaMessage()
    {
        // Display list of messages for selection, location specific
        doSelectionDialog(PaFasTriggeredBroadcastDatabaseAccessor::DATA_DVA_MESSAGE, true);        
    }

    void PaFasTriggeredBroadcastDetailView::onButtonFasAlarmEntity()
    {
        // Display list of events to choose from
        doSelectionDialog(PaFasTriggeredBroadcastDatabaseAccessor::DATA_FAS_ALARM_ENTITY, true);        
    }

    void PaFasTriggeredBroadcastDetailView::doSelectionDialog
    (
        PaFasTriggeredBroadcastDatabaseAccessor::EDataSet dataSet,
        bool filterBySourceLocation
    )
    {
        TA_ASSERT(0 != m_databaseAccessor, "DatabaseAccessor is null");
        TA_ASSERT(0 != m_configItem, "ConfigItem is null");
        
        unsigned long locationKey = TA_Base_Core::DatabaseKey::getInvalidKey();
        if (filterBySourceLocation)
        {
            locationKey = m_configItem->getLocationKey();
        }
        TA_Base_App::MultipleChangeDialog::ItemsForSelection selectionData = 
                        m_databaseAccessor->getDataForSelection(dataSet, locationKey);
        
        // Now by default we should select the item that is currently set
        // for this config item
        unsigned long currentKey = -1;
        CString windowTitle = "";

        switch (dataSet)
        {
        case PaFasTriggeredBroadcastDatabaseAccessor::DATA_ZONE_GROUP:
            currentKey = m_configItem->getPaZoneGroupKey();
            windowTitle = SELECT_ZONE_GROUP_TITLE;
            break;
        case PaFasTriggeredBroadcastDatabaseAccessor::DATA_DVA_MESSAGE:
            currentKey = m_configItem->getPaDvaMessageKey();
            windowTitle = SELECT_DVA_MESSAGE_TITLE;
            break;
        case PaFasTriggeredBroadcastDatabaseAccessor::DATA_FAS_ALARM_ENTITY:
            currentKey = m_configItem->getFasAlarmEntityKey();
            windowTitle = SELECT_FAS_ALARM_ENTITY_TITLE;
            break;
        default:
            TA_ASSERT(FALSE, "Unrecognized enum input");
        };

        std::list<unsigned long> initialSel;
        if (TA_Base_Core::DatabaseKey::isValidKey(currentKey))
        {
            initialSel.push_back(currentKey);
        }
    
        bool singleItemSelection = true;

        TA_Base_App::MultipleChangeDialog dlg(selectionData, 
                                    windowTitle, 
                                    m_databaseAccessor->getDataSetHeaderProperties(dataSet),
                                    initialSel, 
                                    this,
                                    singleItemSelection);
        
        if (IDOK == dlg.DoModal() && 1 == dlg.getSelectedKeys().size())
        {                     
            // Go ahead and update the relevant text field with the new key
            unsigned long newKeyValue = dlg.getSelectedKeys().front();
            
            bool changesMade = false;            

            switch (dataSet)
            {
            case PaFasTriggeredBroadcastDatabaseAccessor::DATA_ZONE_GROUP: 
                if (m_configItem->getPaZoneGroupKey() != newKeyValue)
                {
                    changesMade = true;
                    m_configItem->setPaZoneGroupKey(newKeyValue);
                    refreshZoneGroupEdit();     
                }
                break;
            case PaFasTriggeredBroadcastDatabaseAccessor::DATA_DVA_MESSAGE:
                {
                    if (m_configItem->getPaDvaMessageKey() != newKeyValue)
                    {
                        changesMade = true;
                        m_configItem->setPaDvaMessageKey(newKeyValue);
                        
                        refreshDvaMessageEdit();
                    }
                }
                break;
            case PaFasTriggeredBroadcastDatabaseAccessor::DATA_FAS_ALARM_ENTITY:                
                if (m_configItem->getFasAlarmEntityKey() != newKeyValue)
                {
                    changesMade = true;
                    onFasAlarmEntityChanged( newKeyValue );
                }
                break;           
            default:
                TA_ASSERT(FALSE, "Unrecognized enum input");
            }
            
            // Refresh required
            if (changesMade)
            {
                m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
                refreshButtonAvailability();
            }
        }        
    }

    void PaFasTriggeredBroadcastDetailView::onButtonIsEnabled()
    {    
        TA_ASSERT(m_configItem != NULL, "Config item null");
        
        if (m_isEnabledCheck.GetCheck() & BST_CHECKED)
        {
            m_configItem->setIsEnabledParameter(true);
        }
        else
        {
            m_configItem->setIsEnabledParameter(false);
        }

        // Is Event Group status changed
        m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
        refreshButtonAvailability();
    }

    void PaFasTriggeredBroadcastDetailView::onButtonCyclicEnable()
    {    
        TA_ASSERT(m_configItem != NULL, "Config item null");

        if (m_cyclicCheck.GetCheck() & BST_CHECKED)
        {
            m_cyclicTimesEdit.EnableWindow( TRUE );
            m_cylicTimesSpin.EnableWindow( TRUE );

            unsigned long ulCyclicTimes = static_cast<unsigned long>(m_cylicTimesSpin.GetPos32());
            m_configItem->setPaCyclicTimes(ulCyclicTimes);
        }
        else
        {
            m_cyclicTimesEdit.EnableWindow( FALSE );
            m_cylicTimesSpin.EnableWindow( FALSE );
            m_configItem->setPaCyclicTimes(0);
        }

        // Is Event Group status changed
        m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
        refreshButtonAvailability();
    }
    
    void PaFasTriggeredBroadcastDetailView::onButtonReset() 
    {
        FUNCTION_ENTRY("onButtonReset");

        resetButtonPressed(m_configItem);

        FUNCTION_EXIT;
    }

    void PaFasTriggeredBroadcastDetailView::onButtonApply() 
    {
	    FUNCTION_ENTRY("onButtonApply");

        // Under normal circumstances, the onKillfocus*() methods should be called to update the underlying 
        // IConfig objects.  However, when the operator presses the 'Enter' key, this method is called without
        // first calling onKillfocus*() for the previous widget.  This following line should force the implicit
        // call to the onKillfocus*() method.  
        m_applyBtn.SetFocus();
        
        if ( !m_locationEdt.isTextValid()       || 
             !m_zoneGroupEdt.isTextValid()      ||
             !m_dvaMessageEdt.isTextValid()     ||
             !m_fasAlarmEntityEdt.isTextValid())
        {
			TA_Base_Bus::TransActiveMessage userMsg;
			CString reason = "invalid names still exist in the fields";
			userMsg << reason;
			userMsg.showMsgBox(IDS_UE_030068);
            return;
        }
        applyButtonPressed(m_configItem);

        // Force this call so our status column updates
        m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(), "");

        FUNCTION_EXIT;
    }


    void PaFasTriggeredBroadcastDetailView::onLocationChanged()
    {   
        // Whenever the operator changes the location, it may affect
        // validity of the zone list associations - so refresh their status
        // (OCC terminal can view all, stations can only associate with local zones)
        CWaitCursor wait;

        // Refresh those text fields whose validity is location dependent
        refreshZoneGroupEdit();
        refreshDvaMessageEdit();
        refreshFasAlarmEntityEdit();
    }

    /////////////////////////////////////////////////////////////////////////////
    // PaFasTriggeredBroadcastDetailView diagnostics

    #ifdef _DEBUG
    void PaFasTriggeredBroadcastDetailView::AssertValid() const
    {
	    AbstractDetailView::AssertValid();
    }

    void PaFasTriggeredBroadcastDetailView::Dump(CDumpContext& dc) const
    {
	    AbstractDetailView::Dump(dc);
    }
    #endif //_DEBUG

    
    void PaFasTriggeredBroadcastDetailView::refreshZoneGroupEdit()
    {
        unsigned long key = m_configItem->getPaZoneGroupKey();

        if (TA_Base_Core::DatabaseKey::isInvalidKey(key))
        {
            m_zoneGroupEdt.SetWindowText("");
            m_zoneGroupEdt.setTextType(TA_Base_Bus::TextColourEdit::INVALID);
        }
        else
        {
            unsigned long locKey;

            // Use this key 
            m_zoneGroupEdt.SetWindowText((LPCTSTR) m_databaseAccessor->getZoneGroupName(key, &locKey) );
            m_zoneGroupEdt.setTextType(m_configItem->getLocationKey() == locKey ?
                                            TA_Base_Bus::TextColourEdit::VALID : TA_Base_Bus::TextColourEdit::INVALID);
        }
    }

    void PaFasTriggeredBroadcastDetailView::refreshDvaMessageEdit()
    {
        unsigned long key = m_configItem->getPaDvaMessageKey();

        if (TA_Base_Core::DatabaseKey::isInvalidKey(key))
        {
            m_dvaMessageEdt.SetWindowText("");
            m_dvaMessageEdt.setTextType(TA_Base_Bus::TextColourEdit::INVALID);
        }
        else
        {   
            unsigned long locKey;
            
            // Use this key 
            m_dvaMessageEdt.SetWindowText((LPCTSTR) m_databaseAccessor->getDvaMessageName(key, &locKey));
            // Only valid if key of dva message matches local key
            m_dvaMessageEdt.setTextType(m_configItem->getLocationKey() == locKey ?
                                            TA_Base_Bus::TextColourEdit::VALID : TA_Base_Bus::TextColourEdit::INVALID);                                       
        }  
    }


    void PaFasTriggeredBroadcastDetailView::refreshFasAlarmEntityEdit()
    {        
        unsigned long key = m_configItem->getFasAlarmEntityKey();

        if (TA_Base_Core::DatabaseKey::isInvalidKey(key))
        {
            m_fasAlarmEntityEdt.SetWindowText("");
            m_fasAlarmEntityEdt.setTextType(TA_Base_Bus::TextColourEdit::INVALID);
        }
        else
        {
            unsigned long locKey;
            // Use this key 
            m_fasAlarmEntityEdt.SetWindowText((LPCTSTR) m_databaseAccessor->getFasAlarmEntityName(key, &locKey) );
            m_fasAlarmEntityEdt.setTextType(m_configItem->getLocationKey() == locKey ?
                TA_Base_Bus::TextColourEdit::VALID : TA_Base_Bus::TextColourEdit::INVALID);
        }
    }

    void PaFasTriggeredBroadcastDetailView::onDeltaposSpinCyclicTimes(NMHDR* pNMHDR, LRESULT* pResult) 
    {
        NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
        *pResult = 0;

        int nNextPos = MIN_PA_CYLIC_TIMES;
        if ( NULL != pNMUpDown )
        {
            nNextPos = pNMUpDown->iPos + pNMUpDown->iDelta;
        }

        // Wrapped
        if ( MIN_PA_CYLIC_TIMES > nNextPos )
        {
            nNextPos = MAX_PA_CYLIC_TIMES;
        }
        else if ( MAX_PA_CYLIC_TIMES < nNextPos )
        {
            nNextPos = MIN_PA_CYLIC_TIMES;
        }

        unsigned long ulCyclicTimes = static_cast<unsigned long>(nNextPos);
        m_configItem->setPaCyclicTimes(ulCyclicTimes);

        // Is Event Group status changed
        m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
        refreshButtonAvailability();
    }

    void PaFasTriggeredBroadcastDetailView::onFasAlarmEntityChanged( unsigned long ulNewKey )
    {
        m_configItem->setFasAlarmEntityKey(ulNewKey);
        m_databaseAccessor->getFasAlarmEntityName(ulNewKey);

        unsigned long ulFasAlarmLocation = 0;
        // Get the fas alarm location and update the location of config item
        m_databaseAccessor->getFasAlarmEntityName(ulNewKey, &ulFasAlarmLocation);
        unsigned long ulOldLocationKey = m_configItem->getLocationKey();

        if ( ulOldLocationKey != ulFasAlarmLocation )
        {
            m_configItem->setLocationKey( ulFasAlarmLocation );
            m_locationEdt.SetWindowText( m_databaseAccessor->getLocDisplayName( ulFasAlarmLocation ).c_str() );
            m_locationEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
            onLocationChanged();
        }
        else
        {
            refreshFasAlarmEntityEdit();
        }
    }
}

