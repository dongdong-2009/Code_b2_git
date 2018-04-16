/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/configuration/config_plugins/tr_train/src/TrTrainDetailView.cpp $
  * @author:  Lucky Cuizon
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2014/01/15 8:15:14 $
  * Last modified by:  $Author: Lucky Cuizon $
  * 
  * This view allows the user to configure an Train's details.
  */

#pragma warning(disable:4786 4284)

#include "StdAfx.h"

#include "core/data_access_interface/TrainStateDai/src/ITrain.h"
#include "core/data_access_interface/src/DatabaseKey.h"

#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"

#include "bus/mfc_extensions/src/selection_helpers/SelectionHelper.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/FieldValidityChecker.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/ChangeDialog.h"
#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"
#include "app/configuration/config_plugins/tr_train/src/TrTrainDetailView.h"
#include "app/configuration/config_plugins/tr_train/src/TrTrainDatabaseAccessor.h"
#include "app/configuration/config_plugins/tr_train/src/TrTrainListView.h"

#include "core/utilities/src/DateFormat.h"
#include "core/data_access_interface/TrainStateDai/src/TrainAccessFactory.h"
#include "bus/generic_gui/src/TransActiveMessage.h"

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
 
    const int TrTrainDetailView::RADIO_NUM_FIELD_LENGTH = 7;
	
    IMPLEMENT_DYNCREATE(TrTrainDetailView, CFormView)

    TrTrainDetailView::TrTrainDetailView() : 
        m_configItem(NULL),
        m_listView(NULL),
        m_databaseAccessor(NULL),
        AbstractDetailView(TrTrainDetailView::IDD),
        m_primaryTsi(true),
        m_secondaryTsi(true)
    {
	    //{{AFX_DATA_INIT(TrTrainDetailView)
	    //}}AFX_DATA_INIT
    }      

    TrTrainDetailView::~TrTrainDetailView()
    {
    }


    void TrTrainDetailView::DoDataExchange(CDataExchange* pDX)
    {
	    CFormView::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(TrTrainDetailView)
    	DDX_Control(pDX, IDC_BUTTON_APPLY, m_applyBtn);

    	DDX_Control(pDX, IDC_EDIT_TRAIN_ID, m_trainId);    	
        DDX_Control(pDX, IDC_EDIT_PRIMARY_EMUID, m_primaryTsi);
    	DDX_Control(pDX, IDC_EDIT_SECONDARY_EMUID, m_secondaryTsi);
	
		DDX_Control(pDX, IDC_STATIC_MODIFYDATE, m_modifyDate);
		DDX_Control(pDX, IDC_STATIC_CREATEDATE, m_createDate);
        DDX_Control(pDX, IDC_STATIC_TYPE, m_type);        

        DDX_Control(pDX, IDC_TRAIN_ID_BITMAP, m_transparentImageTrainId);
        DDX_Control(pDX, IDC_PRIMARY_EMUID_BITMAP, m_transparentImagePrimaryTsi);
    	DDX_Control(pDX, IDC_SECONDARY_EMUID_BITMAP, m_transparentImageSecondaryTsi);    	
    	
    	//}}AFX_DATA_MAP
    }


BEGIN_MESSAGE_MAP(TrTrainDetailView, AbstractDetailView)
	//{{AFX_MSG_MAP(TrTrainDetailView)
	ON_BN_CLICKED(IDC_BUTTON_RESET, onButtonReset)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, onButtonApply)

	ON_EN_KILLFOCUS(IDC_EDIT_PRIMARY_EMUID, OnKillfocusEditPrimaryTsi)
	ON_EN_KILLFOCUS(IDC_EDIT_SECONDARY_EMUID, OnKillfocusEditSecondaryTsi)
	ON_EN_KILLFOCUS(IDC_EDIT_TRAIN_ID, OnKillfocusEditTrainId)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

    void TrTrainDetailView::OnInitialUpdate() 
    {
        FUNCTION_ENTRY("OnInitialUpdate");

	    CFormView::OnInitialUpdate();
	    
       
        // Hide all the tr_train details until we are told to populate.
	    showItemDetails(false);

        FUNCTION_EXIT;
    }


    void TrTrainDetailView::showItemDetails(bool isShown)
    {        
        // Show or hide all the controls from the view
        CWnd* nextChild = GetWindow(GW_CHILD);

        while (0 != nextChild)
        {
            nextChild->ShowWindow(isShown);
    
            nextChild = nextChild->GetWindow(GW_HWNDNEXT);
        }
    }


    void TrTrainDetailView::populateItemDetails(TA_Base_Core::IConfigItem* currentItem)
    {
		FUNCTION_ENTRY("populateItemDetails");

        // This can take a while sometimes (mainly populating zone list)..
        CWaitCursor wait;

        if (m_databaseAccessor == NULL || m_listView == NULL)
        {
            m_databaseAccessor = dynamic_cast<TrTrainDatabaseAccessor*>(getDatabaseAccessor());
            m_listView = dynamic_cast<TrTrainListView*>(getListView());

            TA_ASSERT(m_databaseAccessor != NULL, "setDatabaseAccessor() must be called before this method");
            TA_ASSERT(m_listView != NULL, "The list view has not been set yet or is not a proper TrTrainDetailView");
        }


        m_configItem = dynamic_cast<TA_IRS_Core::ITrain*>(currentItem);

        if (m_configItem == NULL)
        { 
            showItemDetails(false);
            FUNCTION_EXIT;
            return;
        }

        try
        {  
			CString buffer;

			//Check the validity of train
			if ( TA_Base_Core::DatabaseKey::isInvalidKey(m_configItem->getTrainId()))
			{
				m_trainId.SetWindowText("");
				m_trainId.setTextType(TA_Base_Bus::TextColourEdit::INVALID);
			}
			else
			{
				buffer.Format("%ld", m_configItem->getTrainId());
				m_trainId.SetWindowText(buffer);

				if ( TA_IRS_Core::TrainAccessFactory::getInstance().isTrainIDExist(m_configItem->getTrainId()) &&
					 m_configItem->isNew() )
				{
					m_trainId.setTextType(TA_Base_Bus::TextColourEdit::INVALID);
				}
				else
					m_trainId.setTextType(TA_Base_Bus::TextColourEdit::VALID);
			}

            TrTrainDatabaseAccessor::CfgTrTrainRecord record =
                            m_databaseAccessor->getRecordFrom(*m_configItem);

            //  Now populate all the fields with the data retrieved from the ITrain* object
			//m_trainId.SetWindowText(record.trainId);
			m_primaryTsi.SetWindowText(record.primaryTsi);
			m_primaryTsi.setTextType(m_primaryTsi.GetWindowTextLength() > 0? 
							TA_Base_Bus::TextColourEdit::VALID : TA_Base_Bus::TextColourEdit::INVALID);
			m_secondaryTsi.SetWindowText(record.secondaryTsi);
			m_secondaryTsi.setTextType(m_secondaryTsi.GetWindowTextLength() > 0? 
							TA_Base_Bus::TextColourEdit::VALID : TA_Base_Bus::TextColourEdit::INVALID);
			

            if( m_configItem->isNew() )
            {
                // This is a new PA ATS Triggered Broadcast or at least a copy of an PA ATS Triggered Broadcast so we indicate this in the type
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
				//m_createDate.SetWindowText( dateCreated.Format(TA_Base_App::TA_Configuration::DATE_FORMAT) );
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
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Cannot retrieve TR Train details to display");
            
            //UE-030001
            MessageBox("Unable to retrieve configuration information. Please try again, and if problem persists, contact support.",
                        RunParams::getInstance().get(RPARAM_APPNAME).c_str(),MB_OK|MB_ICONSTOP);
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Cannot retrieve TR Train details to display");

            //UE-010001
            CString errorMsg("Database cannot be accessed by the ");
            errorMsg += RunParams::getInstance().get(RPARAM_APPNAME).c_str();
            errorMsg += ".";
            MessageBox(errorMsg,RunParams::getInstance().get(RPARAM_APPNAME).c_str(),MB_OK|MB_ICONSTOP);
        }

        FUNCTION_EXIT;
    }


    void TrTrainDetailView::refreshButtonAvailability()
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


	void TrTrainDetailView::OnKillfocusEditTrainId()
	{
		FUNCTION_ENTRY("OnKillfocusEditTrainId");
        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");
		
        if (!m_configItem) 
        {
            // For the odd occasion that the operator has clicked new item straight after modifying
            // this field
            return;
        }
		
        CString trainIdCString;
        m_trainId.GetWindowText(trainIdCString);
		
		unsigned long trainId =  atoi(trainIdCString) ;	

		TA_Base_App::FieldValidityChecker fvc;
		if (0 == trainIdCString.GetLength() || !fvc.isValidInteger(trainIdCString.GetBuffer(0)))
		{
			// Must enter a valid (non-empty) key value
			m_trainId.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
			return;
		}
		
        try
        {
			if (trainId <= 0)
			{
				// Only allow positive valued keys
				m_trainId.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
				return;           
			}

			m_configItem->setTrainId( trainId );

			// make sure that there is not trainID exist
			if (TA_IRS_Core::TrainAccessFactory::getInstance().isTrainIDExist(trainId) )
			{
				m_trainId.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
			}
			else
			{
				m_trainId.setTextType( TA_Base_Bus::TextColourEdit::VALID );
			}

        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database","The TSI could not be found in the database");
			
            m_trainId.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
			
            FUNCTION_EXIT;
            return;
        }
		
		m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),""); //TODO
        refreshButtonAvailability();
		
        FUNCTION_EXIT;	
	}

	void TrTrainDetailView::OnKillfocusEditPrimaryTsi()
	{
		FUNCTION_ENTRY("OnKillfocusEditEmuId1");
        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");
		
        if (!m_configItem) 
        {
            // For the odd occasion that the operator has clicked new item straight after modifying
            // this field
            return;
        }
		
        CString primaryTsiCstr;
        m_primaryTsi.GetWindowText(primaryTsiCstr);

		CT2CA strConvert(primaryTsiCstr);
		//Convert to string
		std::string _primaryTsi(strConvert);
		
			
        try
        {
            // If the branchId has not changed then there is no point doing an update
            if (0 == m_configItem->getPrimaryTsi().compare(_primaryTsi) )
            {
                m_primaryTsi.setTextType( TA_Base_Bus::TextColourEdit::VALID );
                FUNCTION_EXIT;
                return;
            }
			
            m_configItem->setPrimaryTsi( _primaryTsi );
            m_primaryTsi.setTextType( TA_Base_Bus::TextColourEdit::VALID );
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database","The TSI could not be found in the database");
			
            m_primaryTsi.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
			
            FUNCTION_EXIT;
            return;
        }
		
		m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
		//m_listView->itemDetailsChanged(m_configItem->getTrainId(),"");
        refreshButtonAvailability();
		
        FUNCTION_EXIT;	
	}

	void TrTrainDetailView::OnKillfocusEditSecondaryTsi()
	{
		FUNCTION_ENTRY("OnKillfocusEditEmuId2");
        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");
		
        if (!m_configItem) 
        {
            // For the odd occasion that the operator has clicked new item straight after modifying
            // this field
            return;
        }
		
		CString secondaryTsiCstr;
		m_secondaryTsi.GetWindowText(secondaryTsiCstr);

		CT2CA strConvert(secondaryTsiCstr);
		//Convert to string
		std::string _secondaryTsi(strConvert);
		
        try
        {
            // If the branchId has not changed then there is no point doing an update
            if ( 0 == m_configItem->getSecondaryTsi().compare(_secondaryTsi))
            {
                m_secondaryTsi.setTextType( TA_Base_Bus::TextColourEdit::VALID );
                FUNCTION_EXIT;
                return;
            }
			
            m_configItem->setSecondaryTsi(_secondaryTsi) ;
            m_secondaryTsi.setTextType( TA_Base_Bus::TextColourEdit::VALID );
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database","The TSI could not be found in the database");
			
            m_secondaryTsi.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
			
            FUNCTION_EXIT;
            return;
        }
		
		m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
		//m_listView->itemDetailsChanged(m_configItem->getTrainId(),"");
        refreshButtonAvailability();
		
        FUNCTION_EXIT;	
	}
	
    void TrTrainDetailView::onButtonApply() 
    {
	    FUNCTION_ENTRY("onButtonApply");

        // Under normal circumstances, the onKillfocus*() methods should be called to update the underlying 
        // IConfig objects.  However, when the operator presses the 'Enter' key, this method is called without
        // first calling onKillfocus*() for the previous widget.  This following line should force the implicit
        // call to the onKillfocus*() method.  
        m_applyBtn.SetFocus();

        if ( !m_primaryTsi.isTextValid() || !m_secondaryTsi.isTextValid() || !m_trainId.isTextValid() )
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
		 //m_listView->itemDetailsChanged(m_configItem->getTrainId(), "");

        FUNCTION_EXIT;
    }


    /////////////////////////////////////////////////////////////////////////////
    // TrTrainDetailView diagnostics

    #ifdef _DEBUG
    void TrTrainDetailView::AssertValid() const
    {
	    AbstractDetailView::AssertValid();
    }

    void TrTrainDetailView::Dump(CDumpContext& dc) const
    {
	    AbstractDetailView::Dump(dc);
    }
    #endif //_DEBUG

    
	void TrTrainDetailView::refreshPrimaryTsi()
	{
		FUNCTION_ENTRY("refreshPrimaryTsi");

		CString str(m_configItem->getPrimaryTsi().c_str());
		
		m_primaryTsi.SetWindowText(str.GetBuffer(str.GetLength()));
		m_primaryTsi.setTextType(TA_Base_Bus::TextColourEdit::VALID);	

		FUNCTION_EXIT;
	}
	void TrTrainDetailView::refreshSecondaryTsi()
	{
		FUNCTION_ENTRY("refreshSecondaryTsi");
	
		CString str(m_configItem->getSecondaryTsi().c_str());
		
		m_secondaryTsi.SetWindowText(str.GetBuffer(str.GetLength()));
		m_secondaryTsi.setTextType(TA_Base_Bus::TextColourEdit::VALID);	

		FUNCTION_EXIT;
	} 

	void TrTrainDetailView::onButtonReset()
	{
		FUNCTION_ENTRY("onButtonReset");
		
        resetButtonPressed(m_configItem);
		
        FUNCTION_EXIT;
	}
}

