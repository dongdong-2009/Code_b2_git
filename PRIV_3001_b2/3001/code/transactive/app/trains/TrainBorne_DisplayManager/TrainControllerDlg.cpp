// TrainControllerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TrainControllerDlg.h"
#include "VideoDisplayPresenter.h"

#include "DisplayItem/DisplayItemManager.h"
#include "DisplayItem/Sequence.h"
#include "DisplayItem/Quad.h"
// TrainControllerDlg dialog
namespace TA_IRS_App
{
IMPLEMENT_DYNAMIC(TrainControllerDlg, CDialog)

TrainControllerDlg::TrainControllerDlg(VideoDisplayPresenter* presenter,CWnd* pParent /*=NULL*/)
	: CDialog(TrainControllerDlg::IDD, pParent),m_Operation(2),m_monitorScreen(NULL),m_presenter(presenter),m_bDragging(false),
	m_curSelCamera(-1),m_train(NULL)
{

}

TrainControllerDlg::~TrainControllerDlg()
{
}

void TrainControllerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_VD_RA_SINGLEMONITOR, m_Operation);
	DDX_Control(pDX, IDC_VD_LT_CAMERAS, m_LtCamera);
	DDX_Control(pDX, IDC_VD_CB_QUAD, m_CbQuad);
	DDX_Control(pDX, IDC_VD_CB_SEQUENCE, m_CbSequence);
}

BOOL TrainControllerDlg::OnInitDialog()
{
	__super::OnInitDialog();

	// TODO:  Add extra initialization here		
	UpdateData(FALSE);
	CString strCamera;
	m_LtCamera.InsertColumn(0," ",LVCFMT_CENTER,100);
	for(int i=0;i<8;i++)
	{
		strCamera.Format("Camera %d",i+1);
		m_LtCamera.InsertItem(i,strCamera);
	//	m_LtCamera.AddString(strCamera);
	}
	InitQuad();
	InitSequence();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void TrainControllerDlg::InitQuad()
{
	/*CString strQuadName;
	for(int i=0;i<20;i++)
	{
		strQuadName.Format("Quad %d",i+1);
		m_CbQuad.AddString(strQuadName);
	}*/
	std::map<std::string,Quad*> quads = DisplayItemManager::getInstance().getAllQuads();
	std::map<std::string,Quad*>::iterator quad_it = quads.begin();
	for(;quad_it!=quads.end();quad_it++)
	{
		CString strQuad((*quad_it).second->getName().c_str());
		m_CbQuad.AddString(strQuad);
	}
	m_CbQuad.SetCurSel(0);
}

void TrainControllerDlg::InitSequence()
{
	m_mapSequence = DisplayItemManager::getInstance().getAllSequencesByKeys();
	std::map<unsigned long,Sequence*>::iterator it = m_mapSequence.begin();
	for(;it!=m_mapSequence.end();it++)
	{

		CString strSequence((*it).second->getName().c_str());
		m_CbSequence.AddString(strSequence);
	}
	m_CbSequence.SetCurSel(0);
}

void TrainControllerDlg::SetTrainDetails(TA_IRS_Bus::TrainInformationTypes::TrainDetails* train)
{
	CString strServiceNum="Seleted Train: -";
	m_train=train;
	if(NULL!=train && !train->serviceNumber.empty())
		strServiceNum.Format("Selected Train: %s",train->serviceNumber.c_str());
	GetDlgItem(IDC_VD_ST_SELETED_TRAIN)->Invalidate();
	SetDlgItemText(IDC_VD_ST_SELETED_TRAIN,strServiceNum);
	
}

void TrainControllerDlg::ClearMonitorScreen()
{
	m_monitorScreen = NULL;
}

MonitorScreen* TrainControllerDlg::GetMonitorScreen()
{
	return m_monitorScreen;
}

void TrainControllerDlg::SetMonitorScreen(MonitorScreen* screen)
{
	if(screen==NULL)
		return;
	MonitorScreen* oldScreen=m_monitorScreen;
	m_monitorScreen=screen;
//	if(oldScreen == NULL || oldScreen!=screen ||bForceUpdate)
	{
		if(oldScreen!=NULL)
			RestoreScreen(oldScreen);
		BackupScreen(m_monitorScreen);
		
		std::string strOperation = m_monitorScreen->getScreenInfo(SCREEN_OPERATION);
		if(!strOperation.empty())
		{
			m_Operation = atoi(strOperation.c_str());
			
		}
		else
		{
			m_Operation = 2;
		}
		std::string strSelElement = m_monitorScreen->getScreenInfo(SCREEN_SELSCREEN);
		int selElement = atoi(strSelElement.c_str());
		if(!strSelElement.empty())
		{
			
			switch(m_Operation)
			{
			case 0:
				m_CbQuad.SetCurSel(0);
				m_CbSequence.SetCurSel(0);
				break;
			case 1:
				
				m_CbQuad.SetCurSel(GetQuadIndexFromAddress(selElement));
				m_CbSequence.SetCurSel(0);
				break;
			case 2:
				m_CbQuad.SetCurSel(0);
				m_CbSequence.SetCurSel(selElement-1);
				
				break;
			}
		}
		stuMonitorScreen* screen=m_monitorScreen->getScreen(1,ScreenSingleMonitor);
		screen->elementID = selElement;
		screen->elementType = m_Operation==0?ElementCamera :( m_Operation ==1?ElementQuad:ElementSequence);
		screen->bSelected = true;
		m_monitorScreen->drawAll();
	//	std::string strSelSequence = m_monitorScreen->getScreenInfo(SCREEN_SELSCREEN);
	//	if(!strSelSequence.empty())
	//	{
	//		int selSeqeunce = atoi(strSelSequence.c_str());
	//		m_CbSequence.SetCurSel(selSeqeunce);
	//	}
		UpdateData(FALSE);
	}
}

int TrainControllerDlg::GetQuadIndexFromAddress(int iAddr)
{
	std::map<std::string,Quad*> quads = DisplayItemManager::getInstance().getAllQuads();
	std::map<std::string,Quad*>::iterator quad_it = quads.begin();
	for(int index=0;quad_it!=quads.end();quad_it++,index++)
	{
		int thisAddress = atoi((*quad_it).second->getAddress().c_str());
		if(iAddr == thisAddress)
			return index;
	}
	return -1;
}

int TrainControllerDlg::GetQuadAddressFromIndex(int index)
{
	std::map<std::string,Quad*> quads = DisplayItemManager::getInstance().getAllQuads();
	std::map<std::string,Quad*>::iterator quad_it = quads.begin();
	for(int i=0;quad_it!=quads.end();quad_it++,i++)
	{
		if(i!=index)
			continue;
		int address = atoi((*quad_it).second->getAddress().c_str());
				return address;
	}
	return -1;
}

BEGIN_MESSAGE_MAP(TrainControllerDlg, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_NOTIFY(LVN_BEGINDRAG, IDC_VD_LT_CAMERAS, &TrainControllerDlg::OnLvnBegindragLbCameras)
	ON_CBN_SELCHANGE(IDC_VD_CB_QUAD, &TrainControllerDlg::OnCbnSelchangeDvQuad)
	ON_CBN_SELCHANGE(IDC_VD_CB_SEQUENCE, &TrainControllerDlg::OnCbnSelchangeDvSequence)
	ON_BN_CLICKED(IDC_VD_RA_SINGLEMONITOR, &TrainControllerDlg::OnBnClickedRaSingleMonitor)
	ON_BN_CLICKED(IDC_VD_RA_QUAD, &TrainControllerDlg::OnBnClickedRaQuad)
	ON_BN_CLICKED(IDC_VD_RA_SEQUENCE, &TrainControllerDlg::OnBnClickedRaSequence)
	ON_BN_CLICKED(IDC_VD_BU_CONTROLLER_APPLY, &TrainControllerDlg::OnBnClickedControllerApply)
	ON_BN_CLICKED(IDC_VD_BU_CONTROLLER_CLOSE, &TrainControllerDlg::OnBnClickedControllerClose)
END_MESSAGE_MAP()

HBRUSH TrainControllerDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Change any attributes of the DC here

	// TODO:  Return a different brush if the default is not desired
	if(nCtlColor==CTLCOLOR_STATIC && (NULL==pWnd ||( NULL!=pWnd &&pWnd->GetDlgCtrlID()!=IDC_VD_ST_SELETED_TRAIN && pWnd->GetDlgCtrlID()!=IDC_VD_ST_GROUP_TRAINSETTING)))
	{
		pDC->SetBkMode(TRANSPARENT);
		return (HBRUSH)GetStockObject(NULL_BRUSH);
	}
	return hbr;
}

void TrainControllerDlg::OnLvnBegindragLbCameras(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	m_curSelCamera = pNMLV->iItem;
	if(m_curSelCamera<0)
		return;
	POINT pt;
	pt.x=0; pt.y=0; 
	m_imgDrag= m_LtCamera.CreateDragImage(m_curSelCamera,&pt);
	m_imgDrag->BeginDrag(0, CPoint(0, 0));
	m_imgDrag->DragEnter(NULL,pNMLV->ptAction);
//	m_strDragText=m_LtCamera.GetItemText(pNMLV->iItem,pNMLV->iSubItem);
	SetCapture();
	m_bDragging=true;
	UpdateData();
	*pResult = 0;
}

void TrainControllerDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	if(m_bDragging)
	{
		CPoint pt(point);
		ClientToScreen(&pt);
		m_imgDrag->DragMove(pt);
		m_imgDrag->DragShowNolock(false);
		m_imgDrag->DragShowNolock(true);
		stuMonitorScreen* screen = m_monitorScreen->getScreenFromPointer(pt);
		if((NULL==screen ||
			( NULL!=screen && screen->type == ScreenSingleMonitor && m_Operation==0) ))
		//	||(screen->type == ScreenQuad && m_Operation==1)))
			m_monitorScreen->setSelScreen(screen);
	}
}

void TrainControllerDlg::OnLButtonDown(UINT nFlags, CPoint point)
{

}

void TrainControllerDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(m_bDragging && m_curSelCamera>=0)
	{
		m_imgDrag->DragLeave(this);
		m_imgDrag->EndDrag();
		ReleaseCapture();
		m_bDragging = false;
		CPoint pt(point);
		ClientToScreen(&pt);
		stuMonitorScreen* screen = m_monitorScreen->getScreenFromPointer(pt);
		if(NULL==screen)
			return;
		
		if((screen->type == ScreenSingleMonitor && m_Operation==0) )
		//	||(screen->type == ScreenQuad && m_Operation==1))
		{
			screen->elementType=ElementCamera;
			screen->elementID=m_curSelCamera+1;	
			CString strIndex ;
			strIndex.Format("%d",screen->elementID);
			std::string index(strIndex);
			m_monitorScreen->setScreenInfo(SCREEN_SELSCREEN,index);
			m_monitorScreen->draw(*screen);
		}
	}
}

void TrainControllerDlg::OnCbnSelchangeDvQuad()
{
	UpdateData();
	int curQuad = m_CbQuad.GetCurSel();
	if(0>curQuad)
	{
		m_CbQuad.SetCurSel(0);
		curQuad = 0;
	}
	if(m_Operation==1)
	{
		stuMonitorScreen* screen=m_monitorScreen->getScreen(1,ScreenSingleMonitor);
		m_monitorScreen->setSelScreen(screen);
		screen->elementType=ElementQuad;
		screen->elementID=GetQuadAddressFromIndex(curQuad);
		m_monitorScreen->draw(*screen);
		CString strIndex ;
		strIndex.Format("%d",curQuad+1);
		std::string index(strIndex);
		m_monitorScreen->setScreenInfo(SCREEN_SELSCREEN,index);
	}
}

void TrainControllerDlg::OnCbnSelchangeDvSequence()
{
	UpdateData();
	int curSequence = m_CbSequence.GetCurSel();
	if(0>curSequence)
	{
		m_CbSequence.SetCurSel(0);
		curSequence = 0;
	}
	if(m_Operation==2)
	{
		stuMonitorScreen* screen=m_monitorScreen->getScreen(1,ScreenSingleMonitor);
		m_monitorScreen->setSelScreen(screen);
		screen->elementType=ElementSequence;
		screen->elementID=curSequence+1;
		m_monitorScreen->draw(*screen);
		CString strIndex ;
		strIndex.Format("%d",curSequence+1);
		std::string index(strIndex);
		m_monitorScreen->setScreenInfo(SCREEN_SELSCREEN,index);
	}
}

void TrainControllerDlg::OnBnClickedRaSingleMonitor()
{
	UpdateData();
	m_monitorScreen->setScreenInfo(SCREEN_OPERATION,"0");
	stuMonitorScreen* screen = m_monitorScreen->getScreen(1,ScreenSingleMonitor);
	screen->elementType= ElementCamera;
	screen->elementID=0;
	m_monitorScreen->draw(*screen);
}

void TrainControllerDlg::OnBnClickedRaQuad()
{
	UpdateData();
	m_monitorScreen->setScreenInfo(SCREEN_OPERATION,"1");
	OnCbnSelchangeDvQuad();
}

void TrainControllerDlg::OnBnClickedRaSequence()
{
	UpdateData();
	m_monitorScreen->setScreenInfo(SCREEN_OPERATION,"2");
	OnCbnSelchangeDvSequence();
}

void TrainControllerDlg::OnBnClickedControllerApply()
{
	UpdateData();
	bool rtn=false;
	switch(m_Operation)
	{
	case 0:
		rtn = AssignSingleMonitor();
		break;
	case 1:
		rtn = AssignQuad();
		break;
	case 2:
		rtn = AssignSequence();
		break;
	default:
		break;
	}
	if(rtn)
		BackupScreen(m_monitorScreen);
}


void TrainControllerDlg::OnBnClickedControllerClose()
{
	RestoreScreen(m_monitorScreen);
	ShowWindow(SW_HIDE);
}

bool TrainControllerDlg::AssignSingleMonitor()
{
	unsigned char camera=0;
	stuMonitorScreen* screen = m_monitorScreen->getScreen(1,ScreenSingleMonitor);
	if(screen!=NULL)
		camera=screen->elementID;
	if(camera==0)
	{
		MessageBox("Please assign camera to monitor at first");
		return false;

	}
	return m_presenter->AssignSingleMonitor(camera);
}

bool TrainControllerDlg::AssignQuad()
{
	/*stuMonitorScreen* screen=NULL;
	unsigned char cameras[]={0,0,0,0};
	bool bIncludeAtLeastOneCamera=false;
	for(int i=0;i<4;i++)
	{
		screen = m_monitorScreen->getScreen(i+1,ScreenQuad);
		if(NULL==screen)
			continue;
		cameras[i]=screen->elementID;
		if(!bIncludeAtLeastOneCamera && cameras[i]>0)
			bIncludeAtLeastOneCamera=true;
	}
	if(!bIncludeAtLeastOneCamera)
	{
		MessageBox("Please assign at lease one camera to quad at first");
		return;

	}
	m_presenter->AssignQuad(cameras[0],cameras[1],cameras[2],cameras[3]);*/
	unsigned char quadId=0;
	int curQuad = m_CbQuad.GetCurSel();
	if(0>curQuad)
	{
		MessageBox("Please select quad at first");
		return false;
	}
	stuMonitorScreen* screen = m_monitorScreen->getScreen(1,ScreenSingleMonitor);
	if(screen!=NULL)
		quadId=screen->elementID;
	return m_presenter->AssignQuad(quadId);
}

bool TrainControllerDlg::AssignSequence()
{
	unsigned char sequence=0;
	int curSequence = m_CbSequence.GetCurSel();
	if(0>curSequence)
	{
		MessageBox("Please select sequence at first");
		return false;
	}
	stuMonitorScreen* screen = m_monitorScreen->getScreen(1,ScreenSingleMonitor);
	if(screen!=NULL)
		sequence=screen->elementID;
	return m_presenter->AssignSequence(sequence);
}

void TrainControllerDlg::BackupScreen(MonitorScreen* monitorScreen)
{
	stuMonitorScreen* screen=monitorScreen->getScreen(1,ScreenSingleMonitor);
	m_backupScreen.monitorElement = screen->elementID ;
	m_backupScreen.monitorElementType = screen->elementType;
	m_backupScreen.monitorSelected = screen->bSelected;
	m_backupScreen.Operation = monitorScreen->getScreenInfo(SCREEN_OPERATION);
	m_backupScreen.SelScreen = monitorScreen->getScreenInfo(SCREEN_SELSCREEN);

}

void TrainControllerDlg::RestoreScreen(MonitorScreen* monitorScreen)
{
	stuMonitorScreen* screen=monitorScreen->getScreen(1,ScreenSingleMonitor);
	screen->elementID = m_backupScreen.monitorElement ;
	screen->elementType = m_backupScreen.monitorElementType ;
	screen->bSelected = m_backupScreen.monitorSelected ;
	monitorScreen->setScreenInfo(SCREEN_OPERATION,m_backupScreen.Operation) ;
	monitorScreen->setScreenInfo(SCREEN_SELSCREEN,m_backupScreen.SelScreen) ;
}

}
// TrainControllerDlg message handlers
