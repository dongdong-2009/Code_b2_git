#pragma once
#include "app/bas/TunnelModeManager/src/TunnelModeListCtrl.h"

namespace TA_IRS_App
{
	// CFireModeListCtrl
	class CFireModeListCtrl : public CTunnelModeListCtrl
	{
		DECLARE_DYNAMIC(CFireModeListCtrl)

	public:
		CFireModeListCtrl();
		virtual ~CFireModeListCtrl();
	public:

	protected:
		DECLARE_MESSAGE_MAP()
	public:

	};

}


