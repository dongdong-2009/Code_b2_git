#pragma once
#include "app/bas/TunnelModeManager/src/TunnelModeListCtrl.h"

// CCongestionModeListCtrl
namespace TA_IRS_App
{
	class CCongestionModeListCtrl : public CTunnelModeListCtrl
	{
		DECLARE_DYNAMIC(CCongestionModeListCtrl)

	public:
		CCongestionModeListCtrl();
		virtual ~CCongestionModeListCtrl();
	public:

	protected:
		DECLARE_MESSAGE_MAP()
	};
}



