#pragma once

#include "ICallContextMenu.h"

#include <vector>
#include <string>

namespace TA_IRS_App
{
	class CallContextMenu : public TA_IRS_App::ICallContextMenu
	{
		public:
			CallContextMenu(const UINT & nResourceID, 
							CWnd * hWndTargetCtrl, 
							const std::vector <std::string> & vctrMenu);

			~CallContextMenu(void);

			virtual void show ();

		private:
			UINT m_nContextResourceId;
			CMenu m_menu;
			CWnd * m_wndCtrl;
			std::vector <std::string> m_vctrMenu;

	};

}
