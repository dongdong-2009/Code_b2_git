#pragma once

//#include "bus\mfc_extensions\src\StdAfx.h"
#include "app\radio\radio_manager_gui\src\StdAfx.h"
#include "app\radio\radio_manager_gui\src\IDlgLoader.h"
#include "core\utilities\src\DebugUtil.h"
#include "core\synchronisation\src\ReEntrantThreadLockable.h"
#include <map>

namespace TA_IRS_App
{

	class SingletonDlgLoader
	{
		public:
			
			//SingletonDlgLoader(void);
		
			enum EDlgType
			{
				ERadioMsgDlgInit = 0
			};

			struct SDialogSpecParam
			{
				EDlgType dlgType;
				void * vData;
			};

			~SingletonDlgLoader(void);
			
			static SingletonDlgLoader * getInstance ();
			static void removeInstance ();
			
			// Set the Dialog type to be load or close
			void addDlgToLoad (EDlgType dlgType,CWnd * parentWnd);

			// Set the Dialog type to be load or close
			void removeDlg (EDlgType dlgType);

			// Set specific values needed for a certain dialog
			void setSpecAssignableForDlg (SDialogSpecParam & dlgParam);
			
			// open the dialog
			bool openDlg (EDlgType dlgType);
			// close the dialog
			bool closeDlg(EDlgType dlgType);

			bool isDlgExist (EDlgType dlgType);

			void * getState (EDlgType dlgType);

		private:

			SingletonDlgLoader(void);

			static SingletonDlgLoader * m_SingletonDlgLoader;
			static TA_Base_Core::ReEntrantThreadLockable m_ThreadLock;
			
			std::map <EDlgType,TA_IRS_App::IDlgLoader *> m_mapDialogCollection;
			
	};
}

