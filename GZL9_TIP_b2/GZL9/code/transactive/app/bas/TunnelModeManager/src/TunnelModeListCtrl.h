#pragma once

#include "app/bas/TunnelModeManager/src/CommonDefine.h"

namespace TA_IRS_App
{
	// CTunnelModeListCtrl
	class CTunnelModeListCtrl : public CListCtrl
	{
		DECLARE_DYNAMIC(CTunnelModeListCtrl)

	public:
		CTunnelModeListCtrl();
		virtual ~CTunnelModeListCtrl();
	public:
		virtual void SetupListCtrl();
		virtual void InsertItemTexts(const CString& modeType, const CString& modeDesc, const CString& leftBound, const CString rightBound, DWORD itemData);
	protected:
		DECLARE_MESSAGE_MAP()
	protected:
		DWORD setRowFullSelect();
	public:
		const static std::string ColumnName_ModeType;
		const static std::string ColumnName_ModeDesc;
		const static std::string ColumnName_LeftStation;
		const static std::string ColumnName_RightStation;
		const static unsigned long ColumnIndex_ModeType;
		const static unsigned long ColumnIndex_ModeDesc;
		const static unsigned long ColumnIndex_LeftStation;
		const static unsigned long ColumnIndex_RightStation;
    public:
        //afx_msg void OnSize(UINT nType, int cx, int cy);
	};

}
