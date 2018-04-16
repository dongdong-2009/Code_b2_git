#pragma once
#include "app/bas/BASModeManager/src/BASBaseListCtrl.h"
#include "core/data_access_interface/bas/src/ModeAccessFactory.h"
#include "app/bas/BASModeManager/src/CommonDef.h"
Namespace_TA_IRS_APP_Begin
// ReportListCtrl
class ReportListCtrl : public BASBaseListCtrl
{
	DECLARE_DYNAMIC(ReportListCtrl)
public:
	static const unsigned int COLUMN_INDEX_REPORT_SUBSYSTEM = 0;
	static const unsigned int COLUMN_INDEX_REPORT_TARGETMODE = 1;
	static const unsigned int COLUMN_INDEX_REPORT_CURRENTMODE = 2;
	static const unsigned int COLUMN_INDEX_REPORT_AUTOMANUAL = 3;
public:
	ReportListCtrl();
	virtual ~ReportListCtrl();
public:
	void UpdateCurrentMode(TA_IRS_Core::Mode_SmartPtr& mode_ptr);
	void UpdateExpectMode(TA_IRS_Core::Mode_SmartPtr& mode_ptr);
	void UpdateCurrentModeFail(unsigned long subsystemKey);
	void UpdateExpectModeFail(unsigned long subsystemKey);
	void UpdateAutoManual(unsigned long autoManal, unsigned long subsystemKey);

	void RefreshGUI(UINT nIDEvent);

	void prepareChangeToLocation(unsigned long locationKey);
protected:
	afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()

public:
	int InsertRow(int nPos,int nNoOfCols, LPCTSTR pText, ...);
private:
	CString getAutoManualStringByValue(unsigned long value);
};
Namespace_TA_IRS_APP_End