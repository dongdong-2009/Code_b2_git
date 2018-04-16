/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/security/DutyManager/src/RegionSelectionDlg.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This dialog enables the operator to select from a list of regions for which
  * they currently hold region duties.
  *
  */

#include "StdAfx.h"
#include "app/security/DutyManager/src/DutyManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace
{
    const int REGION_COLUMN(0);
    const CString REGION_COLUMN_HEADING("");
    const int REGION_COLUMN_WIDTH(208);
}

using TA_Base_Bus::DataCache;
using TA_Base_Bus::SessionCache;
using TA_Base_Bus::RegionDutyMatrix;
using TA_Base_Bus::RegionDutyMatrixElement;
using TA_Base_Bus::RegionKeyList;

using namespace TA_Base_App;
using namespace TA_Base_Bus;

/////////////////////////////////////////////////////////////////////////////
// RegionSelectionDlg dialog

RegionSelectionDlg::RegionSelectionDlg(CWnd* pParent /*=NULL*/) :
	CDialog(RegionSelectionDlg::IDD, pParent),
    m_dataCache(DutyDataStore::getDataCache()),
    m_sessionCache(DutyDataStore::getSessionCache()),
    m_regionDutyMatrix(DutyDataStore::getRegionDutyMatrix())
{
	//{{AFX_DATA_INIT(RegionSelectionDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


RegionSelectionDlg::~RegionSelectionDlg()
{
    m_selectedRegionKeys.clear();
	//DutyDataStore::cleanUp();
}


TA_Base_Bus::RegionKeyList RegionSelectionDlg::getSelectedRegionKeys() const
{
    return m_selectedRegionKeys;
}


void RegionSelectionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(RegionSelectionDlg)
	DDX_Control(pDX, IDC_ASSIGNED_RGN_LIST, m_assignedRegionList);
	//}}AFX_DATA_MAP
}




/////////////////////////////////////////////////////////////////////////////
// RegionSelectionDlg message handlers

BEGIN_MESSAGE_MAP(RegionSelectionDlg, CDialog)
	//{{AFX_MSG_MAP(RegionSelectionDlg)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_ASSIGNED_RGN_LIST, OnRegionListItemChanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL RegionSelectionDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
    if (TA_Base_Core::RunParams::getInstance().isSet(RPARAM_USELOCATIONLABEL)) 
    {
        SetWindowText(_T("Select locations"));
    }

    m_assignedRegionList.SetExtendedStyle(m_assignedRegionList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);
    m_assignedRegionList.InsertColumn(REGION_COLUMN, _T(REGION_COLUMN_HEADING), LVCFMT_LEFT, REGION_COLUMN_WIDTH);

    GetDlgItem(IDOK)->EnableWindow(FALSE);

    try
    {
        // Insert into the list box the regions for which the operator has duty.
        RegionKeyList regionKeys(m_regionDutyMatrix.getColumnIdsByDuties(m_sessionCache.getCurrentSessionId()));

        for (RegionKeyList::const_iterator regionIt = regionKeys.begin(); regionIt != regionKeys.end(); regionIt++)
        {
            try
            {
                std::string regionName(m_dataCache.getRegionName(*regionIt));

                int item(m_assignedRegionList.InsertItem(m_assignedRegionList.GetItemCount(), _T(regionName.c_str())));
                m_assignedRegionList.SetItemData(item, *regionIt);
            }
            catch (...)
            { 
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Ignoring invalid region [r: %lu]", *regionIt);
            }
        }
 
        // Disable the list if there are no regions for which the operator has duty.
        if (m_assignedRegionList.GetItemCount() == 0)
        {
            m_assignedRegionList.EnableWindow(FALSE);

            if (TA_Base_Core::RunParams::getInstance().isSet(RPARAM_USELOCATIONLABEL)) 
            {
                m_assignedRegionList.InsertItem(0, _T("No assigned locations."));
            }
            else
            {
                m_assignedRegionList.InsertItem(0, _T("No assigned regions."));
            }
        }
    }
    catch (...)
    {
        EndDialog(IDABORT);
        throw;
    }
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void RegionSelectionDlg::OnRegionListItemChanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = reinterpret_cast<NM_LISTVIEW*>(pNMHDR);

	if ((pNMListView->uNewState & LVIS_SELECTED) == LVIS_SELECTED ||
        (pNMListView->uOldState & LVIS_SELECTED) == LVIS_SELECTED)
    {
        GetDlgItem(IDOK)->EnableWindow(m_assignedRegionList.GetSelectedCount() > 0);
    }
	
	*pResult = 0;
}


void RegionSelectionDlg::OnOK() 
{
    m_selectedRegionKeys.reserve(m_assignedRegionList.GetSelectedCount());

    int item(LV_ERR);
    while ((item = m_assignedRegionList.GetNextItem(item, LVNI_SELECTED)) != LV_ERR)
    {
        m_selectedRegionKeys.push_back(m_assignedRegionList.GetItemData(item));
    }

	CDialog::OnOK();
}
