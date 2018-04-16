/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/configuration/config_plugins/radio_itsi_groups/src/ITSIGroupsDetailView.cpp $
  * @author:  Derrick Liew
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view allows the user to configure a ITSI Group's details.
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/radio_itsi_groups/src/StdAfx.h"
#include "app/configuration/config_plugins/radio_itsi_groups/src/ITSIGroupsDetailView.h"
#include "app/configuration/config_plugins/radio_itsi_groups/src/ITSIGroupsDatabaseAccessor.h"
#include "app/configuration/config_plugins/radio_itsi_groups/src/ITSIGroupsListView.h"

#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/ChangeDialog.h"

#include "bus/generic_gui/src/TransActiveMessage.h"

#include "core/data_access_interface/radio/src/IITSIGroup.h"
#include "core/data_access_interface/radio/src/ConfigITSIGroup.h"
#include "core/data_access_interface/src/ProfileGroupHelper.h"

#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"

#include "core/utilities/src/DateFormat.h" // TD12474

// ITSIGroupsDetailView.cpp : implementation file
//

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::RunParams;
using TA_Base_Core::DebugUtil;
using namespace TA_Base_App::TA_Configuration;

/////////////////////////////////////////////////////////////////////////////
// ITSIGroupsDetailView dialog

namespace TA_IRS_App
{
    // These match the restrictions in the database for how long certain fields can be.
    // Hopefully in the future these could be read from the database.
    const int ITSIGroupsDetailView::NAME_FIELD_LENGTH        = 80;

    IMPLEMENT_DYNCREATE(ITSIGroupsDetailView, CFormView)

    ITSIGroupsDetailView::ITSIGroupsDetailView() : 
        m_curITSIGp(NULL),
		m_listView(NULL),
        m_databaseAccessor(NULL),
        AbstractDetailView(ITSIGroupsDetailView::IDD)
    {
        //{{AFX_DATA_INIT(ITSIGroupsDetailView)
	m_usedText = _T("");
	//}}AFX_DATA_INIT
    }


    ITSIGroupsDetailView::~ITSIGroupsDetailView()
    {
    }


    void ITSIGroupsDetailView::DoDataExchange(CDataExchange* pDX)
    {
	    CFormView::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(ITSIGroupsDetailView)
		DDX_Control(pDX, IDC_STATIC_USED, m_staticUsed);
		DDX_Control(pDX, IDC_STATIC_MODIFYDATE, m_modifyDate);
		DDX_Control(pDX, IDC_STATIC_CREATEDATE, m_createDate);
		DDX_Control(pDX, IDC_EDIT_NAME, m_name);
		DDX_Control(pDX, IDC_NAME_BITMAP, m_nameBitmap);
		DDX_Text(pDX, IDC_STATIC_USED, m_usedText);
		//}}AFX_DATA_MAP
    }


	BEGIN_MESSAGE_MAP(ITSIGroupsDetailView, AbstractDetailView)
		//{{AFX_MSG_MAP(ITSIGroupsDetailView)
		ON_BN_CLICKED(IDC_BUTTON_RESET, onButtonReset)
		ON_BN_CLICKED(IDC_BUTTON_APPLY, onButtonApply)
		ON_EN_KILLFOCUS(IDC_EDIT_NAME, onKillfocusEditName)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	
	void ITSIGroupsDetailView::OnInitialUpdate() 
    {
        FUNCTION_ENTRY("OnInitialUpdate");

	    CFormView::OnInitialUpdate();
	    
        // Limit the number of characters that can be entered into fields so that they match the database schema.
        m_name.SetLimitText(NAME_FIELD_LENGTH);

        // Hide all the ITSI details until we are told to populate.
	    showItemDetails(false);

        FUNCTION_EXIT;
    }


    void ITSIGroupsDetailView::showItemDetails(bool isShown)
    {
        // Show or hide all the controls from the view
        this->GetDlgItem(IDC_STATIC_GROUP)->ShowWindow(isShown);
        this->GetDlgItem(IDC_STATIC_NAME)->ShowWindow(isShown);
		this->GetDlgItem(IDC_STATIC_USED)->ShowWindow(isShown);
        GetDlgItem(IDC_EDIT_NAME)->ShowWindow(isShown);
        GetDlgItem(IDC_NAME_BITMAP)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_DATE_CREATED)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_DATE_MODIFIED)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_CREATEDATE)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_MODIFYDATE)->ShowWindow(isShown);
        GetDlgItem(IDC_BUTTON_APPLY)->ShowWindow(isShown);
        GetDlgItem(IDC_BUTTON_RESET)->ShowWindow(isShown);
    }


    void ITSIGroupsDetailView::updateReadOnlyFields()
    {
        FUNCTION_ENTRY("updateReadOnlyFields");

        //There is no any read only fields to set as the name field can be renamed
		//Update the usedText to show that this item has been used in ITSI table
		TA_ASSERT(m_curITSIGp != NULL, "The IConfigITSIGroup object is not initialised");

		// cast the ITSI group to IConfigITSIGroup to access the isGroupUsedInProfile function
		TA_Base_Core::ConfigITSIGroup* configPG = dynamic_cast<TA_Base_Core::ConfigITSIGroup*>(m_curITSIGp);
		bool isUsed = configPG->isGroupUsedInProfile( m_curITSIGp->getKey() );

		m_staticUsed.SetWindowText("");
		if (configPG->isNew() == true)
		{
			// this ITSI group is new
			m_usedText.LoadString(IDS_NOT_USED);
		}
		else if (isUsed == true)
		{
			// this ITSI group is used in ITSI table
			m_usedText.LoadString(IDS_IS_USED);
		}
		else if (isUsed == false)
		{
			// this ITSI group is not new but it is not used
			m_usedText.LoadString(IDS_NOT_USED);
		}
		m_staticUsed.SetWindowText(m_usedText);

        FUNCTION_EXIT;
    }


    void ITSIGroupsDetailView::refreshButtonAvailability()
    {
        if (m_curITSIGp == NULL)
        {
            return;
        }

        if (m_curITSIGp->isNew() )
        {
            GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(false);
            GetDlgItem(IDC_BUTTON_APPLY)->EnableWindow(true);
            return;
        }
        
        // We have an old item
        if (m_curITSIGp->hasChanged())
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


    void ITSIGroupsDetailView::populateItemDetails(TA_Base_Core::IConfigItem* currentItem)
    {
        FUNCTION_ENTRY("populateItemDetails");

        if (m_databaseAccessor == NULL || m_listView == NULL)
        {
            m_databaseAccessor = dynamic_cast<ITSIGroupsDatabaseAccessor*>(getDatabaseAccessor());
            m_listView = dynamic_cast<ITSIGroupsListView*>(getListView());

            TA_ASSERT(m_databaseAccessor != NULL, "setDatabaseAccessor() must be called before this method");
            TA_ASSERT(m_listView != NULL, "The list view has not been set yet or is not a proper ITSIGroupsDetailView");
        }


        m_curITSIGp = dynamic_cast<TA_Base_Core::IConfigITSIGroup*>(currentItem);

        if (m_curITSIGp == NULL)
        { 
            showItemDetails(false);
            FUNCTION_EXIT;
            return;
        }

        try
        {
            //  Now populate all the fields with the data retrieved from the IConfigITSI* object

	        m_name.SetWindowText( m_curITSIGp->getName().c_str() );

            if( m_curITSIGp->isNew() )
            {
                // This is a new ITSI or at least a copy of an ITSI so we indicate this in the type
                // field and make sure the modify and create dates are blank.
                m_modifyDate.SetWindowText(NO_VALUE);
	            m_createDate.SetWindowText(NO_VALUE);
            }
            else
            {
				TA_Base_Core::DateFormat dateFormat; // TD12474

                // Retrieve the modify time and date. If this is zero then one hasn't been set yet
                // so leave the modified field as blank
                time_t modify = m_curITSIGp->getDateModified();
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
                COleDateTime dateCreated ( m_curITSIGp->getDateCreated() );
				// TD12474 ++
				//m_createDate.SetWindowText( dateCreated.Format(TA_Base_App::TA_Configuration::DATE_FORMAT) );
				std::string dateFormatStr = dateFormat.getDateFormat(TA_Base_Core::DateFormat::DateTimeFormat6);
				m_createDate.SetWindowText( dateCreated.Format(dateFormatStr.c_str()) );
				// ++ TD12474				
            }
            
            updateReadOnlyFields();

            showItemDetails(true);

            refreshButtonAvailability();
        }

        catch ( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Cannot retrieve ITSI Group details to display");
            
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_030001);
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Cannot retrieve ITSI Group details to display");

            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_010001);
        }

        FUNCTION_EXIT;
    }


    void ITSIGroupsDetailView::onKillfocusEditName() 
    {
        FUNCTION_ENTRY("onKillfocusEditName");

        // When the user changes the name we must update the IConfigITSIGroup* object so that it
        // has a record of all changes when the user chooses to apply.
        if (m_curITSIGp != NULL )
        {
            try
            {
                CString name;
                m_name.GetWindowText(name);
                m_curITSIGp->setName( std::string(name) );

                if (m_listView!=NULL)
                {
                    m_listView->itemDetailsChanged( m_curITSIGp->getUniqueIdentifier(), m_curITSIGp->getName().c_str() );
                }
            }
            catch ( const TA_Base_Core::DatabaseException& )
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", "Could not set the data");
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg.showMsgBox(IDS_UE_010001);

                FUNCTION_EXIT;
                return;
            }
            catch ( const TA_Base_Core::DataException& )
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "DataException", "Could not set the data");
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg.showMsgBox(IDS_UE_030001);

                FUNCTION_EXIT;
                return;
            }
        }   

        refreshButtonAvailability();

        FUNCTION_EXIT;
    }

    void ITSIGroupsDetailView::onButtonReset() 
    {
        FUNCTION_ENTRY("onButtonReset");

        resetButtonPressed(m_curITSIGp);

        // We invalidate in case this reset does not cause a redraw. We need to ensure the display
        // bitmap is repainted on top.
        Invalidate();

        FUNCTION_EXIT;
    }


    void ITSIGroupsDetailView::onButtonApply() 
    {
	    FUNCTION_ENTRY("onButtonApply");

        GetDlgItem(IDC_BUTTON_APPLY)->SetFocus();

        applyButtonPressed(m_curITSIGp);

        // We invalidate in case this reset does not cause a redraw. We need to ensure the display
        // bitmap is repainted on top.
        Invalidate();

        FUNCTION_EXIT;
    }


    /////////////////////////////////////////////////////////////////////////////
    // ITSIGroupsDetailView diagnostics

    #ifdef _DEBUG
    void ITSIGroupsDetailView::AssertValid() const
    {
	    AbstractDetailView::AssertValid();
    }

    void ITSIGroupsDetailView::Dump(CDumpContext& dc) const
    {
	    AbstractDetailView::Dump(dc);
    }
    #endif //_DEBUG

}