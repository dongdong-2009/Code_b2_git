#if !defined(TA_GF_3002_app_bas_BasParameterManager_CModeParameterCtrlList)
#define TA_GF_3002_app_bas_BasParameterManager_CModeParameterCtrlList
#pragma once

namespace TA_IRS_App
{
	// CModeParameterCtrlList
	class CModeParameterCtrlList : public CListCtrl
	{
		DECLARE_DYNAMIC(CModeParameterCtrlList)

	public:
		CModeParameterCtrlList();
		virtual ~CModeParameterCtrlList();

	protected:
		DECLARE_MESSAGE_MAP()
	};
}


#endif //TA_GF_3002_app_bas_BasParameterManager_CModeParameterCtrlList
