/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/configuration/config_plugins/pa_train_dva_message/src/PaTrainDvaMessageGlobalView.cpp $
  * @author:  Jade Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * This view contains global parameter settings for Train DVA MEssages
  */

#pragma warning(disable:4786 4284)

#include "StdAfx.h"

#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/FieldValidityChecker.h"
#include "bus/resources/resource.h"

#include "bus/generic_gui/src/TransactiveMessage.h" // TD14164 
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/data_access_interface/src/DatabaseKey.h"
#include "core/data_access_interface/pa_4669/src/PaTrainDvaMessageAccessFactory.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_Core; using namespace TA_IRS_Core;

using  namespace TA_Base_App::TA_Configuration;

namespace TA_IRS_App
{ 
    IMPLEMENT_DYNCREATE(PaTrainDvaMessageGlobalView, CFormView)

    PaTrainDvaMessageGlobalView::PaTrainDvaMessageGlobalView()
    :
    CFormView(PaTrainDvaMessageGlobalView::IDD),
    m_existingVersion(TA_Base_Core::DatabaseKey::getInvalidKey()),
    m_detailView(0)
    {
    }


    PaTrainDvaMessageGlobalView::~PaTrainDvaMessageGlobalView()
    {
    }


    void PaTrainDvaMessageGlobalView::setDetailView(TA_Base_App::IDetailView* view)
    {
        m_detailView = dynamic_cast<PaTrainDvaMessageDetailView*>(view);
        if (0 == m_detailView)
        {
			// TD14164 ++
            /*AfxMessageBox("Error linking global and detail views");*/
			TA_Base_Bus::TransActiveMessage userMsg;
			userMsg.showMsgBox(IDS_UE_610015);
			// ++ TD14164 
        }
    }

    void PaTrainDvaMessageGlobalView::DoDataExchange(CDataExchange* pDX)
    {
	    CFormView::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(PaTrainDvaMessageGlobalView)
    	//DDX_Control(pDX, IDC_EDIT_LIBRARY_VERSION, m_versionEdit);
        DDX_Control(pDX, IDC_DVA_VERSIONS, m_versionEdit );
        DDX_Control(pDX, IDAPPLY, m_applyButton);
        DDX_Control(pDX, IDRESET, m_resetButton);
    	//}}AFX_DATA_MAP
    }

BEGIN_MESSAGE_MAP(PaTrainDvaMessageGlobalView, CFormView)
	//{{AFX_MSG_MAP(PaTrainDvaMessageGlobalView)
	ON_WM_CREATE()
    ON_WM_SIZE()
  
    ON_BN_CLICKED(IDRESET, onButtonReset)
	ON_BN_CLICKED(IDAPPLY, onButtonApply)
    //ON_EN_CHANGE(IDC_EDIT_LIBRARY_VERSION, onVersionChanged)
    ON_NOTIFY(IPN_FIELDCHANGED, IDC_DVA_VERSIONS, onVersionChanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    int PaTrainDvaMessageGlobalView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
    {
 	    if (CFormView::OnCreate(lpCreateStruct) == -1)
		    return -1;
	    
	    return 0;
    }


    void PaTrainDvaMessageGlobalView::OnInitialUpdate() 
    {
	    CFormView::OnInitialUpdate();
        
        // Set up initial version information
        m_existingVersion = PaTrainDvaMessageAccessFactory::getInstance().getLibraryVersion();
        //CString buffer;
        //buffer.Format("%ld", m_existingVersion);
        //m_versionEdit.SetWindowText((LPCTSTR) buffer);
        m_versionEdit.SetAddress( m_existingVersion );
        
        onVersionChanged(NULL, NULL);                
    }

    void PaTrainDvaMessageGlobalView::OnSize(UINT nType, int cx, int cy) 
    {
        CFormView::OnSize(nType, cx, cy);
        // Call the CFormView::ONSize instead of the one in the abstract class above
    }


    void PaTrainDvaMessageGlobalView::OnDraw(CDC* pDC)
    {
	    CDocument* pDoc = GetDocument();
    }


    void PaTrainDvaMessageGlobalView::onButtonReset()
    {
        // Reset text field to existing version setting
        //CString buffer;
        //buffer.Format("%ld", m_existingVersion);
        //m_versionEdit.SetWindowText((LPCTSTR) buffer);
        m_versionEdit.SetAddress( m_existingVersion );

        onVersionChanged(NULL, NULL);
    }


    void PaTrainDvaMessageGlobalView::onButtonApply()
    {
        try
        {
            PaTrainDvaMessageAccessFactory::getInstance().setLibraryVersion(
                        m_enteredVersion.getValue());

            // Now send out a changed notification, using an invalid key value as
            // special flag to indicate it's the global version that's changed
            if (0 != m_detailView)
            {
                m_detailView->sendOutGlobalTrainVersionChangeNotification();
            }
            else
            {
				// TD14164 ++
                /*AfxMessageBox("Error sending out version change notification");*/
				TA_Base_Bus::TransActiveMessage userMsg;
				userMsg.showMsgBox(IDS_UE_610016);
				// ++ TD14164 
            }

            // Update the existing version to reflect one we just changed to
            m_existingVersion = m_enteredVersion.getValue();
            
            // Refresh dialog
            onVersionChanged(NULL, NULL);
        }
        catch (const TA_Base_Core::TransactiveException& e)
        {
            std::ostringstream ostr;
            ostr << "Error updating the Train DVA Message Set library version:\n"
                 << e.what();
                 
			// TD14164 ++
            /*AfxMessageBox(ostr.str().c_str());*/
			TA_Base_Bus::TransActiveMessage userMsg;
			userMsg << e.what();
			userMsg.showMsgBox(IDS_UE_610017);
			// ++ TD14164 
        }
    }


    void PaTrainDvaMessageGlobalView::onVersionChanged(NMHDR* pNMHDR, LRESULT* pResult)
    {
        //CString versionString;
        //m_versionEdit.GetWindowText(versionString);

        unsigned long ulNewVersion = 0;
        int nNonBlankField = m_versionEdit.GetAddress( ulNewVersion );

        TA_Base_App::FieldValidityChecker fvc;

        // Only allow values >= 0
        //long keyValue = atol(versionString.GetBuffer(0));

        //if (0 == versionString.GetLength() || 
        //    !fvc.isValidInteger(versionString.GetBuffer(0)) ||
        //    keyValue < 0)
        if ( nNonBlankField < 4 )
        {
            //m_versionEdit.setTextType(TA_Base_Bus::TextColourEdit::INVALID);
            m_enteredVersion.resetValue();
        }
        else
        {
            //m_versionEdit.setTextType(TA_Base_Bus::TextColourEdit::VALID);
            m_enteredVersion.setValue( ulNewVersion );
        }

        refreshApplyResetButtons();
    }        


    void PaTrainDvaMessageGlobalView::refreshApplyResetButtons()
    {
        // Only enable apply button when version data is both changed and valid
        if (!m_enteredVersion.hasBeenSetToValue(m_existingVersion) && m_enteredVersion.hasBeenSet())
        {
            m_applyButton.EnableWindow(TRUE);
        }
        else
        {
            m_applyButton.EnableWindow(FALSE);
        }
        
        if (!m_enteredVersion.hasBeenSetToValue(m_existingVersion))
        {
            m_resetButton.EnableWindow(TRUE);
        }
        else
        {        
            m_resetButton.EnableWindow(FALSE);
        }
    }
}

