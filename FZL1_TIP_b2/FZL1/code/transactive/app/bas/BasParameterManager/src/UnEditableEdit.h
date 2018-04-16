#if !defined (Transactive_3002_GF_App_Bas_TimeTableManager_CUnEditableEdit)
#define Transactive_3002_GF_App_Bas_TimeTableManager_CUnEditableEdit
#if _MSC_VER > 1000
#pragma once
#endif

namespace TA_IRS_App
{
	// CUnEditableEdit
	class CUnEditableEdit : public CEdit
	{
		DECLARE_DYNAMIC(CUnEditableEdit)

	public:
		CUnEditableEdit();
		virtual ~CUnEditableEdit();
	
	protected:
		afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
		DECLARE_MESSAGE_MAP()
	};
}

#endif //Transactive_3002_GF_App_Bas_TimeTableManager_CUnEditableEdit