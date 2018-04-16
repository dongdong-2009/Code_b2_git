/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/incidents/incident_manager/src/AssetDialog.cpp $
  * @author:  Wu Min Tao
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */

#include <algorithm>
#include "app/incidents/incident_manager/src/stdafx.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/TransactiveException.h"

#include "bus/scada/scada_tree/src/EntityTreeItem.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"

#include "app/incidents/incident_manager/src/AssetDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AssetDialog dialog

namespace TA_IRS_App
{
	
	AssetDialog::AssetDialog(bool editable, AssetType& asset, CWnd* pParent/*=NULL*/)
		: CDialog(AssetDialog::IDD,pParent), 
		m_AssetTree(NULL, false, false), //TD15806 - initialize Asset Tree to not display the datapoints
		m_editable(editable),
		m_currentAsset(asset)
	{
		FUNCTION_ENTRY( "Constructor" );
		FUNCTION_EXIT;
	}
	
	
	void AssetDialog::DoDataExchange(CDataExchange* pDX)
	{
		CDialog::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(AssetDialog)
		DDX_Control(pDX, IDOK, m_okBtn);
		DDX_Control(pDX, IDC_ASSET_TYPE_TREE, m_AssetTree);
		//}}AFX_DATA_MAP
	}
	
	
	BEGIN_MESSAGE_MAP(AssetDialog, CDialog)
		//{{AFX_MSG_MAP(AssetDialog)
		ON_NOTIFY(NM_CLICK, IDC_ASSET_TYPE_TREE, OnClickAssetTypeTree)
		//}}AFX_MSG_MAP
		END_MESSAGE_MAP()
		
		/////////////////////////////////////////////////////////////////////////////
		// AssetDialog message handlers
		
		AssetType AssetDialog::getSelectedAsset() const
	{
		FUNCTION_ENTRY( "getSelectedAsset" );
		//TA_ASSERT(!m_currentAsset.subsystem.empty(), "No Asset has been selected!");
		FUNCTION_EXIT;
		return m_currentAsset;
	}
	
	void AssetDialog::OnOK() 
	{
		FUNCTION_ENTRY( "OnOK" );
		CObjectTreeItem* item = m_AssetTree.getSelectedItem();
		if (item == NULL)
		{
			FUNCTION_EXIT;
			return;
		}
		
		TA_Base_Bus::CEntityTreeItem* entityItem = dynamic_cast<TA_Base_Bus::CEntityTreeItem*>(item);
		if (entityItem == NULL)
		{
			FUNCTION_EXIT;
			return;
		}
		
		try
		{
			m_currentAsset.id = entityItem->getEntityData()->getKey();
			//TD15803 - should display the subsystem name
			//m_currentAsset.subsystem = entityItem->getEntityData()->getSubsystemName();
			m_currentAsset.subsystem = entityItem->getEntityData()->getPhysicalSubsystemName();
			if (m_currentAsset.subsystem.empty())
			{
				m_currentAsset.subsystem =  entityItem->getEntityData()->getSubsystemName();
			}
			 
			//TD15806 - should return the value of the assetname
			//m_currentAsset.description = entityItem->getEntityData()->getDescription();
			std::string objectName = entityItem->getEntityData()->getName();
			TA_Base_Core::IData* assetData = TA_Base_Core::EntityAccessFactory::getInstance().getParameter(objectName, "AssetName");
			if (assetData != NULL && (assetData->getNumRows() == 1))
			{
				m_currentAsset.description = assetData->getStringData(0,"VALUE");
				// the following code for logging will crash the program, temporarily disable it for further investigation
				/*LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
					"AssetDialog::OnOK() - entityid: %ld, subsystem: %s, assetname: %s",
					m_currentAsset.id,
					m_currentAsset.subsystem,
					m_currentAsset.description);    
					*/

			}
			else
			{
				// just supply the current entityName if the Asset name is not available		
				m_currentAsset.description = entityItem->getEntityData()->getName();
				std::replace(m_currentAsset.description.begin(),m_currentAsset.description.end(),'.','/');					
			}
		}
		catch (TA_Base_Core::TransactiveException& e)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
				"Transactive Exception: %s caught in AssetDialog::OnOK()!", e.what());    
		}
		catch (...)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
				"Unknown Exception caught in AssetDialog::OnOK()!");    
		}
		CDialog::OnOK();
		FUNCTION_EXIT;
	}
	
	void AssetDialog::OnCancel() 
	{	
		FUNCTION_ENTRY( "OnCancel" );
		CDialog::OnCancel();
		FUNCTION_EXIT;
	}
	
	
	void AssetDialog::OnClickAssetTypeTree(NMHDR* pNMHDR, LRESULT* pResult) 
	{
		FUNCTION_ENTRY( "OnClickTreePlan" );
		NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
		GetDlgItem(IDOK)->EnableWindow( m_AssetTree.getSelectedItem() != NULL );
		
		//*pResult = 0; // make this a don't-care parameter // lizettejl
		FUNCTION_EXIT;
	}
	
	
	BOOL AssetDialog::OnInitDialog() 
	{
		FUNCTION_ENTRY( "OnInitDialog" );
		CDialog::OnInitDialog();
		
		if (!m_currentAsset.subsystem.empty())
		{
			m_AssetTree.setInitialDataPoint(m_currentAsset.subsystem);
		}
		
		GetDlgItem(IDOK)->EnableWindow( m_AssetTree.getSelectedItem() != NULL );
		
		FUNCTION_EXIT;
		
		return TRUE;  // return TRUE unless you set the focus to a control
		// EXCEPTION: OCX Property Pages should return FALSE
	}
	}// end TA_IRS_App
