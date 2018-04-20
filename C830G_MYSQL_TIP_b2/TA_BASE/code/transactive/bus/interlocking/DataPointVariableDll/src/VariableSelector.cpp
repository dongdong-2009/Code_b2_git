/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/interlocking/DataPointVariableDll/src/VariableSelector.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
// VariableSelector.cpp : implementation file
//

#include "stdafx.h"
///	\ add additional includes here"
#pragma warning (disable : 4786)
#pragma warning (disable : 4284)

#include "VariableSelector.h"
#include "ScadaDatabaseAccessor.h"

#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

#include "bus/scada/scada_tree/src/EntityTreeItem.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "bus/interlocking/InterlockingConfig/src/InterlockingRuleDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace
{
    const std::string ENTITY_TYPE( "DataPoint" );
}


namespace TA_Base_Bus
{
    /////////////////////////////////////////////////////////////////////////////
    // VariableSelector dialog


    VariableSelector::VariableSelector(CWnd* pParent /*=NULL*/)
	    : m_scadaTree(new ScadaDatabaseAccessor()),
		CDialog(VariableSelector::IDD, pParent)
    {
	    //{{AFX_DATA_INIT(VariableSelector)
		    // NOTE: the ClassWizard will add member initialization here
	    //}}AFX_DATA_INIT
    }


    void VariableSelector::DoDataExchange(CDataExchange* pDX)
    {
	    CDialog::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(VariableSelector)
		DDX_Control(pDX, IDC_Selector, m_scadaTree);
	    //}}AFX_DATA_MAP
    }


    BEGIN_MESSAGE_MAP(VariableSelector, CDialog)
	    //{{AFX_MSG_MAP(VariableSelector)
	    ON_NOTIFY(NM_DBLCLK, IDC_Selector, onDblclkTree)
		ON_NOTIFY(TVN_SELCHANGED, IDC_Selector, onSelchangedTree)
	    ON_BN_CLICKED(IDCLOSE, OnCancel)
		ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
    END_MESSAGE_MAP()

    /**
     * Destructor
     */
	VariableSelector::~VariableSelector()
	{
	}

	void VariableSelector::cleanScadaTree()
	{
		if (&m_scadaTree != NULL)
		{
			m_scadaTree.DeleteAllItems();
		}
	}

    /////////////////////////////////////////////////////////////////////////////
    // VariableSelector message handlers

    BOOL VariableSelector::OnInitDialog() 
    {
	    CDialog::OnInitDialog();

        // We don't need to load all items as this is done automatically in our tree. When the tree is
        // created the items are loaded.
		m_entityName="";
	    
	    return TRUE;  // return TRUE unless you set the focus to a control
	                  // EXCEPTION: OCX Property Pages should return FALSE
    }

	
	void VariableSelector::OnShowWindow(BOOL bShow, UINT nStatus) 
	{		
		
		EnableWindow(true);
		SetWindowText("Datapoint Selector"); // Fix for TD 11435
		m_scadaTree.ShowWindow(SW_SHOW);
		m_scadaTree.SetFocus();

		CDialog::OnShowWindow(bShow, nStatus);
		try
		{
			// TD 11934 the dynamic_cast may cause excess violation when user did some unexpected actions like
			// keyed in garbage or crazy clicking. So cache the entity name and reuse it
			if (m_entityName.empty() == true)
			{
				m_entityName = dynamic_cast <InterlockingRuleDialog*> (m_adder)->getEntityName();
			}
			// expand the tree and set the initial selection to the current entity
			m_scadaTree.setInitialDataPoint( m_entityName );
			// Enable the close button based on m_scadaTree's emptiness
			GetDlgItem(IDCLOSE)->EnableWindow( m_scadaTree.getSelectedItem() != NULL );

		}
		catch (...)
		{
			// in case there is problem with casting or entity name, and the scada tree can't set the initial point
			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
				"VariableSelector.OnShowWindow: Failed to get the entityName from m_adder.");
			// Display a message box to inform user of error
			TransActiveMessage userMsg;
			userMsg << std::string("Datapoint Selector as failed to cast the InterlockingRuleDialog.\n Try again");
			userMsg.showMsgBox(IDS_UE_010002);
			
			// close the dialog
			m_scadaTree.ShowWindow(SW_HIDE);
			ShowWindow(SW_HIDE);
		}

	}

    void VariableSelector::OnCancel() 
    {
        CObjectTreeItem* item = m_scadaTree.getSelectedItem();
        if (item != NULL)
        {
			try
			{
				addItemToVariableAdder(item);
			}
			catch (...)
			{
				// in case there is problem with the casting or getting the entity item
				// ignore the selection and close the selector
				LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
					"VariableSelector.OnCancel: Failed to select entity from SCADA tree.");
				m_scadaTree.ShowWindow(SW_HIDE);
				ShowWindow(SW_HIDE);
			}
        }

		m_scadaTree.ShowWindow(SW_HIDE);
		ShowWindow(SW_HIDE);
    }


    void VariableSelector::onSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult) 
    {
	    NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

        GetDlgItem(IDCLOSE)->EnableWindow( m_scadaTree.getSelectedItem() != NULL );
	    
	    *pResult = 0;
    }

    void VariableSelector::onDblclkTree(NMHDR* pNMHDR, LRESULT* pResult) 
    {
        CObjectTreeItem* item = m_scadaTree.getSelectedItem();
        if (item == NULL)
        {
            return;
        }
		
		try
		{
			addItemToVariableAdder(item);
		}
		catch (...)
		{
			// in case there is problem with the casting or getting the entity item
			// ignore the selection and return
			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
				 "VariableSelector.onDblclkTree: Failed to select entity from SCADA tree.");
			return;
		}
	    
	    *pResult = 0;
    }

	void VariableSelector::addItemToVariableAdder(CObjectTreeItem* item)
	{
		TA_Assert(item != NULL);

		TA_Base_Bus::CEntityTreeItem* entityItem = dynamic_cast<TA_Base_Bus::CEntityTreeItem*>(item);
		TA_Assert(entityItem != NULL);

		std::string dataType = entityItem->getEntityData()->getType();
		if (dataType.compare(ENTITY_TYPE) == 0 )
		{
			// unsigned long key = entityItem->getEntityData()->getKey();
			std::string name = entityItem->getEntityData()->getName();
			TA_Base_Core::DataPointEntityData* pData = 
				dynamic_cast<TA_Base_Core::DataPointEntityData*>(entityItem->getEntityData());
			TA_Assert(pData!=NULL);
			std::string type = pData->getDataPointType() + ENTITY_TYPE;            
			TA_Assert(m_adder != NULL);
			m_adder->addVariable( name, name, type );
		}
	}


}//namespace

