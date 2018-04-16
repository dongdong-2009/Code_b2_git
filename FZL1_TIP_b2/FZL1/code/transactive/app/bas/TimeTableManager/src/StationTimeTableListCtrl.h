#if !defined (Transactive_3002_GF_App_Bas_TimeTableManager_StationTimeTableListCtrl)
#define Transactive_3002_GF_App_Bas_TimeTableManager_StationTimeTableListCtrl
#if _MSC_VER > 1000
#pragma once
#endif

#include "app/bas/TimeTableManager/src/TimeTableListCtrl.h"
namespace TA_IRS_App
{
	// StationTimeTableListCtrl
	class StationTimeTableListCtrl : public TimeTableListCtrl
	{
		DECLARE_DYNAMIC(StationTimeTableListCtrl)

	public:
		StationTimeTableListCtrl();
		virtual ~StationTimeTableListCtrl();

	protected:
		DECLARE_MESSAGE_MAP()
	};

}


#endif //Transactive_3002_GF_App_Bas_TimeTableManager_StationTimeTableListCtrl