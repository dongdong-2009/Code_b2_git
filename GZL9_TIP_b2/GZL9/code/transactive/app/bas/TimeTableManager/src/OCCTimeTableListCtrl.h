#if !defined (Transactive_3002_GF_App_Bas_TimeTableManager_OCCTimeTableListCtrl)
#define Transactive_3002_GF_App_Bas_TimeTableManager_OCCTimeTableListCtrl
#if _MSC_VER > 1000
#pragma once
#endif

#include "app/bas/TimeTableManager/src/TimeTableListCtrl.h"

namespace TA_IRS_App
{
	// OCCTimeTableListCtrl
	class OCCTimeTableListCtrl : public TimeTableListCtrl
	{
		DECLARE_DYNAMIC(OCCTimeTableListCtrl)

	public:
		OCCTimeTableListCtrl();
		virtual ~OCCTimeTableListCtrl();

	protected:
		DECLARE_MESSAGE_MAP()
	};

}


#endif //Transactive_3002_GF_App_Bas_TimeTableManager_OCCTimeTableListCtrl