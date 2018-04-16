
/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: depot\3001_TIP\3001\transactive\app\radio\radio_manager_gui\IDlgLoader.h
 * @author:  Dennis Abriol
 * @version: $Revision: #1 $
 *
 * Last modification: $ $
 * Last modified by:  $ $
 * 
 */

#pragma once 

namespace TA_IRS_App
{
	class IDlgLoader
	{
		public:
			virtual bool showDlg () = 0;
			virtual bool closeDlg () = 0;
			virtual bool isDialogActive () = 0;
	};

	class IRadioInitDlgMsg : public IDlgLoader
	{
		public:
			
			enum ERadioInitMsgState
			{
				EInit = 0,
				EOperation
			};

			virtual void setState (ERadioInitMsgState * radioInitMsgState) = 0;
			virtual IRadioInitDlgMsg::ERadioInitMsgState * getState () = 0;
	};
}

