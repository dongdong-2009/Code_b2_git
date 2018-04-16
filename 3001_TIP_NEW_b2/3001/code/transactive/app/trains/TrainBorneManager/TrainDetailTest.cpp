// TrainDetailTest.cpp : implementation file
//

#include "stdafx.h"
#include "TrainBorneManager.h"
#include "TrainDetailTest.h"
#include "bus/trains/TrainSelectionComponent/src/TrainSelector.h"

// TrainDetailTest dialog
namespace TA_IRS_App
{
IMPLEMENT_DYNAMIC(TrainDetailTest, CDialog)

TrainDetailTest::TrainDetailTest(TA_IRS_Bus::TrainSelector* selector,CWnd* pParent /*=NULL*/)
	: CDialog(TrainDetailTest::IDD, pParent)
	,m_trainSelector(selector) 
	, m_trainID(0)
	, m_ComValid(TRUE)
	, m_communicationLocation(0)
	, m_primaryTSI(_T(""))
	, m_secondaryTSI(_T(""))
	, m_atsValid(TRUE)
	, m_locationValid(TRUE)
	, m_currentLocation(0)
	, m_currentCCTVZone(0)
	, m_seviceNumber(_T(""))
	, m_isMute(FALSE)
	, m_track(0)
{

}

TrainDetailTest::~TrainDetailTest()
{
}

void TrainDetailTest::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_ED_TRAINID, m_trainID);
	DDX_Check(pDX, IDC_CH_COM, m_ComValid);
	DDX_Text(pDX, IDC_ED_COM_LOCATION, m_communicationLocation);
	DDX_Text(pDX, IDC_ED_TSI0, m_primaryTSI);
	DDX_Text(pDX, IDC_ED_TSI1, m_secondaryTSI);
	DDX_Check(pDX, IDC_CH_ATS, m_atsValid);
	DDX_Check(pDX, IDC_CH_LOCATION, m_locationValid);
	DDX_Text(pDX, IDC_ED_CUR_LOCATION, m_currentLocation);
	DDX_Text(pDX, IDC_ED_CCTVZONE, m_currentCCTVZone);
	DDX_Text(pDX, IDC_ED_SERVICE_NUMBER, m_seviceNumber);
	DDX_Check(pDX, IDC_CH_MUTE, m_isMute);
	DDX_Text(pDX, IDC_ED_TRACK, m_track);
}


BEGIN_MESSAGE_MAP(TrainDetailTest, CDialog)
	ON_BN_CLICKED(IDC_BU_SET, &TrainDetailTest::OnBnClickedBuSet)
END_MESSAGE_MAP()


// TrainDetailTest message handlers

void TrainDetailTest::OnBnClickedBuSet()
{
	// TODO: Add your control notification handler code here
	UpdateData();
	

	TA_IRS_Bus::TrainInformationTypes::TrainDetails detail;
	detail.atsValid = this->m_atsValid;
	detail.communicatingLocation = this->m_communicationLocation;
	detail.communicationValid = this->m_ComValid;
	detail.currentCctvZone = this->m_currentCCTVZone;
	
	
	detail.currentLocation = this->m_currentLocation;
	detail.isLocalisationValid = this->m_locationValid;
	detail.isMute = this->m_isMute;
	CT2CA strPrimaryTSI (m_primaryTSI);
	detail.primaryTsi = strPrimaryTSI;
	CT2CA strSecondaryTSI (m_secondaryTSI);
	detail.secondaryTsi = strSecondaryTSI;
	CT2CA strSeviceNumber (m_seviceNumber);
	detail.serviceNumber = strSeviceNumber;
	detail.track = (TA_IRS_Bus::TrainInformationTypes::EAtsTrack)this->m_track;
	detail.trainId = this->m_trainID;
	TA_IRS_Bus::TrainInformationTypes::AgentOperationMode mode;
	mode.controlledLocations.insert(1);
	mode.controlledLocations.insert(50);
	mode.controlledLocations.insert(51);
	mode.controlledLocations.insert(52);
	TA_IRS_Bus::LocationCache::getInstance().cacheLocationControl( mode );
	m_trainSelector->processTrainDetails(detail);
}

}
BOOL TA_IRS_App::TrainDetailTest::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
