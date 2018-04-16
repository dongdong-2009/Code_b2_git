// SelectSesSubDutyDlg.cpp : implementation file
//

#include "stdafx.h"
//#include "dutymanager.h"
#include "app/security/DutyManager/src/DutyManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace
{
    const int SES_OPERATOR_COLUMN(0);
    const int SES_PROFILE_COLUMN(1);
    const int SES_WORKSTATION_COLUMN(2);
	
    const CString SES_OPERATOR_COLUMN_HEADING("Operator");
    const CString SES_PROFILE_COLUMN_HEADING("Profile");
    const CString SES_WORKSTATION_COLUMN_HEADING("Workstation");
	
    //const int REGION_COLUMN_WIDTH(120);
    //const int OPERATOR_COLUMN_WIDTH(180);
	const int SES_OPERATOR_COLUMN_WIDTH(160);
    const int SES_PROFILE_COLUMN_WIDTH(110);
    const int SES_WORKSTATION_COLUMN_WIDTH(80);
}
/////////////////////////////////////////////////////////////////////////////
// SelectSesSubDutyDlg dialog
using TA_Base_Bus::DutyAgentAccess;
using TA_Base_Bus::DataCache;
using TA_Base_Bus::SessionCache;
using TA_Base_Bus::RegionKeyList;
using TA_Base_Bus::TransferableRegionList;
using TA_Base_Core::DebugUtil;

using namespace TA_Base_App;
using namespace TA_Base_Bus;
/////////////////////////////////////////////////////////////////////////////
// SelectSesSubDutyDlg dialog


SelectSesSubDutyDlg::SelectSesSubDutyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(SelectSesSubDutyDlg::IDD, pParent),
	m_regionDutyMatrix(DutyDataStore::getRegionDutyMatrix()),
	m_dataCache(DutyDataStore::getDataCache()),
    m_sessionCache(DutyDataStore::getSessionCache())
{
	//{{AFX_DATA_INIT(SelectSesSubDutyDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_selectedSessionInfo.SessionId = "";
}


void SelectSesSubDutyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SelectSesSubDutyDlg)
	DDX_Control(pDX, IDOK, m_selectSesBtn);
	DDX_Control(pDX, IDC_SELECT_SESSION_LIST, m_selectSessionList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SelectSesSubDutyDlg, CDialog)
	//{{AFX_MSG_MAP(SelectSesSubDutyDlg)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_SELECT_SESSION_LIST, OnItemchangedSelectSessionList)
	ON_BN_CLICKED(IDOK, OnSelectBtn)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SelectSesSubDutyDlg message handlers

void SelectSesSubDutyDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

BOOL SelectSesSubDutyDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	//m_dataCache = DutyDataStore::getDataCache();
	//m_sessionCache = DutyDataStore::getSessionCache();

	SessionIdList sessionIds(m_regionDutyMatrix.getRowIds());
	
	// TODO: Add extra initialization here
	m_selectSessionList.SetExtendedStyle(m_selectSessionList.GetExtendedStyle()| LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);
	m_selectSessionList.InsertColumn(SES_OPERATOR_COLUMN,_T(SES_OPERATOR_COLUMN_HEADING),LVCFMT_LEFT,SES_OPERATOR_COLUMN_WIDTH);
	m_selectSessionList.InsertColumn(SES_PROFILE_COLUMN,_T(SES_PROFILE_COLUMN_HEADING),LVCFMT_LEFT,SES_PROFILE_COLUMN_WIDTH);
	m_selectSessionList.InsertColumn(SES_WORKSTATION_COLUMN,_T(SES_WORKSTATION_COLUMN_HEADING),LVCFMT_LEFT,SES_WORKSTATION_COLUMN_WIDTH);
	
	m_selectSesBtn.EnableWindow(FALSE);
	
	SessionInfoEx sessionListInfo;
	try
	{
		for (SessionIdList::const_iterator sessionIt = sessionIds.begin(); sessionIt != sessionIds.end(); sessionIt++)
		{
			try
			{
				sessionListInfo = m_sessionCache.getSessionInfo(*sessionIt);
				
				// No need to display current session for select
				if(sessionListInfo.SessionId == m_sessionCache.getCurrentSessionId())
				{
					continue;
				}
				
				std::string operatorName("");
				std::string profileName("");
				std::string workstationName("");
				
				if ( sessionListInfo.OperatorId.empty() 
					|| sessionListInfo.ProfileId.empty() )
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "[CL-21882] OperatorId is empty, or ProfileId is empty.");
					continue;
				}
				
				operatorName = m_dataCache.getOperatorDescription(sessionListInfo.OperatorId[0]);
				profileName = m_dataCache.getProfileName(sessionListInfo.ProfileId[0]);
				if ( (sessionListInfo.OperatorId.size() > 1) && (sessionListInfo.ProfileId.size() > 1) )
				{
					operatorName += "/";
					operatorName += m_dataCache.getOperatorDescription(sessionListInfo.OperatorId[1]);

					profileName += "/";
					profileName += m_dataCache.getProfileName(sessionListInfo.ProfileId[1]);
				}
				workstationName = m_dataCache.getWorkstationName(sessionListInfo.WorkstationId);
				
				int item(m_selectSessionList.InsertItem(m_selectSessionList.GetItemCount(), _T("")));
				
                m_selectSessionList.SetItemText(item, SES_OPERATOR_COLUMN, _T(operatorName.c_str()));
                m_selectSessionList.SetItemText(item, SES_PROFILE_COLUMN, _T(profileName.c_str()));
                m_selectSessionList.SetItemText(item, SES_WORKSTATION_COLUMN, _T(workstationName.c_str()));
                //m_selectSessionList.SetItemData(item, sessionListInfo.OperatorId[0]);
				m_selectSessionList.SetItemData(item, sessionListInfo.sessionKey);
				
			}
			catch(...)
			{
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "[CL-21882] Error while get session list.");
			}
		}
		if (m_selectSessionList.GetItemCount() == 0)
        {
            m_selectSessionList.EnableWindow(FALSE);
            
            int item(m_selectSessionList.InsertItem(m_selectSessionList.GetItemCount(), _T("")));
			
            m_selectSessionList.SetItemText(item, SES_PROFILE_COLUMN, _T("No available other sessions."));
        }
		else
		{
			AdjustColumnWidth();
		}
		
	}
	catch(...)
	{
        EndDialog(IDABORT);
        throw;
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void SelectSesSubDutyDlg::OnItemchangedSelectSessionList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	if ((pNMListView->uNewState & LVIS_SELECTED) == LVIS_SELECTED ||
        (pNMListView->uOldState & LVIS_SELECTED) == LVIS_SELECTED)
    {
        m_selectSesBtn.EnableWindow(m_selectSessionList.GetSelectedCount() > 0);
    }


	*pResult = 0;
}

void SelectSesSubDutyDlg::OnSelectBtn() 
{
	LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,
		"begin SelectSesSubDutyDlg::OnSelSesBtn");
	// TODO: Add your control notification handler code here
	TA_ASSERT(m_selectSessionList.GetSelectedCount() > 0, "No sessions selected.");
	
	try
	{
		int item(LV_ERR);
		//	DWORD dwItem = 0;
		SessionInfoEx sessionListInfo;
		if ((item = m_selectSessionList.GetNextItem(item, LVNI_SELECTED)) != LV_ERR)
		{
			//selectedRegionKeys.push_back(m_transferableRegionList.GetItemData(item));
			DWORD dwItem = m_selectSessionList.GetItemData(item);
			
			sessionListInfo = m_sessionCache.getSessionInfo(dwItem);
			
			m_selectedSessionInfo = sessionListInfo;
			
			LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,
				"in SelectSesSubDutyDlg::OnSelSesBtn, before PostMessage WM_OTHER_SES_SUB_DUTY_UPDATE");
			
			//GetParent()->PostMessage(WM_OTHER_SES_SUB_DUTY_UPDATE,reinterpret_cast<WPARAM>(&sessionListInfo),0);
			
			if ((!sessionListInfo.OperatorId.empty()) && (!sessionListInfo.ProfileId.empty()))
			{
				LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,
					"in SelectSesSubDutyDlg::OnSelSesBtn, after PostMessage WM_OTHER_SES_SUB_DUTY_UPDATE, the SessionInfoEx in the parameter is: OperatorId=%ld, ProfileId=%ld, LocationId=%ld, WorkStationId=%ld",
					sessionListInfo.OperatorId[0],sessionListInfo.ProfileId[0],sessionListInfo.LocationId,sessionListInfo.WorkstationId);
			}
			
			EndDialog(IDOK);
		}
	}
	catch (const TA_Base_Core::TransactiveException& ex)
    {
        MessageBox::error("Error while select Session Duty from SelectSesSubDutyDlg.", ex);
    }
    catch (...)
    {
        MessageBox::error("Error while select Session Duty from SelectSesSubDutyDlg.");
	}
	LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,
		"end SelectSesSubDutyDlg::OnSelSesBtn");
}

int SelectSesSubDutyDlg::GetColumnCount()
{
	CHeaderCtrl* pHeaderCtrl = m_selectSessionList.GetHeaderCtrl();
	return(pHeaderCtrl->GetItemCount());
}

void SelectSesSubDutyDlg::AdjustColumnWidth()
{
	int nColumnCount = GetColumnCount();
	m_selectSessionList.SetColumnWidth(nColumnCount-1, LVSCW_AUTOSIZE_USEHEADER);
	
	m_selectSessionList.Invalidate();
}

TA_Base_Bus::SessionInfoEx SelectSesSubDutyDlg::getSelectedSessionInfo()
{
	/*
	try
	{
		return m_selectedSessionInfo;
	}
	catch (const TA_Base_Core::TransactiveException& ex)
    {
        MessageBox::error("Error while get Selected SessionInfo.", ex);
    }
    catch (...)
    {
        MessageBox::error("Error while get Selected SessionInfo.");
	}
	*/
	return m_selectedSessionInfo;
}