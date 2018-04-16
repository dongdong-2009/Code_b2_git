#pragma once

namespace TA_IRS_Bus
{
	class TrainSelector;
}
// TrainDetailTest dialog
namespace TA_IRS_App
{
class TrainDetailTest : public CDialog
{
	DECLARE_DYNAMIC(TrainDetailTest)

public:
	TrainDetailTest(TA_IRS_Bus::TrainSelector* selector,CWnd* pParent = NULL);   // standard constructor
	virtual ~TrainDetailTest();

// Dialog Data
	enum { IDD = IDD_TRAIN_DETAIL_TEST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	short m_trainID;
	BOOL m_ComValid;
	unsigned long m_communicationLocation;
	CString m_primaryTSI;
	CString m_secondaryTSI;
	BOOL m_atsValid;
	BOOL m_locationValid;
	unsigned long m_currentLocation;
	unsigned long m_currentCCTVZone;
	CString m_seviceNumber;
	BOOL m_isMute;
	unsigned int m_track;
	afx_msg void OnBnClickedBuSet();
private:
	TA_IRS_Bus::TrainSelector*  m_trainSelector;
	virtual BOOL OnInitDialog();
};
}

