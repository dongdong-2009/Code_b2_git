#pragma once

#include "app\radio\radio_manager_gui\src\IDlgLoader.h"
//#include "bus\mfc_extensions\src\StdAfx.h"
#include "app\radio\radio_manager_gui\src\StdAfx.h"
#include "app\radio\radio_manager_gui\src\resource.h"
#include "app\radio\radio_manager_gui\src\RadioInitMsgDlg.h"
#include "core\utilities\src\DebugUtil.h"

namespace TA_IRS_App
{
	class RadioInitMsgDlgHelper : public TA_IRS_App::IRadioInitDlgMsg
	{
		public:
			RadioInitMsgDlgHelper(CWnd * parenWnd);
			~RadioInitMsgDlgHelper(void);

			virtual bool showDlg ();
			virtual bool closeDlg ();
			virtual void setState (ERadioInitMsgState * radioInitMsgState);
			
			virtual	bool isDialogActive ();
			IRadioInitDlgMsg::ERadioInitMsgState * getState ();

		private:
			ERadioInitMsgState m_radioMsgState;
			CRadioInitMsgDlg * m_radioMsgInitDlg;
			CWnd * m_parentWnd;
	};
}

