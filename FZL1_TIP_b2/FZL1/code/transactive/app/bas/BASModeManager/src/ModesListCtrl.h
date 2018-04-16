#pragma once

#include "core/data_access_interface/bas/src/ModeAccessFactory.h"
#include "app/bas/BASModeManager/src/ReportListCtrl.h"
#include "app/bas/BASModeManager/src/BASBaseListCtrl.h"
#include "app/bas/BASModeManager/src/ModeRunningStatus.h"
#include "app/bas/BASModeManager/src/DeviceEquipmentManager.h"
Namespace_TA_IRS_APP_Begin
// ModesListCtrl
class ModesListCtrl : public BASBaseListCtrl
{
	DECLARE_DYNAMIC(ModesListCtrl)
public:
	static const unsigned int COLUMN_INDEX_MODE_INDEX = 0;
	static const unsigned int COLUMN_INDEX_MODE_VALUE = 1;
	static const unsigned int COLUMN_INDEX_MODE_LOCATION = 2;
	static const unsigned int COLUMN_INDEX_MODE_SUBSYSTEM = 3;
	static const unsigned int COLUMN_INDEX_MODE_TYPE = 4;
	static const unsigned int COLUMN_INDEX_MODE_DESC = 5;
	static const unsigned int COLUMN_INDEX_MODE_STATUS = 6;
public:
	ModesListCtrl();
	virtual ~ModesListCtrl();
public:
	void UpdateModeSatus(TA_IRS_Core::Mode_SmartPtr& mode_ptr, int status);
	void UpdateModeSatus(unsigned long modeKey, int status);
	void RefreshGUI(UINT nIDEvent);
protected:
	afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()
private:
	bool m_isFreshed;
};
Namespace_TA_IRS_APP_End
