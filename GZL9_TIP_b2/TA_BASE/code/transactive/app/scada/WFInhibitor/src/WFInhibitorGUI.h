/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL9_TIP/TA_BASE/transactive/app/scada/wf_inhibitor/src/WFInhibitorGUI.h $
  * @author:  Andy Parker
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/07/05 10:04:52 $
  * Last modified by:  $Author: jinmin.huang $
  * 
  * This class provides the main dialog used to set WF Global Inhibit information.  
  */
  
#if !defined (WF_Inhibitor_GUI_H)
#define WF_Inhibitor_GUI_H
#if _MSC_VER > 1000
#pragma once
#endif

#include "core/exceptions/src/TransactiveException.h"
#include "bus/generic_gui/src/AbstractDialogGUI.h"

namespace TA_Base_App
{
	class WFInhibitorGUI : public TA_Base_Bus::AbstractDialogGUI
	{
	public:
		WFInhibitorGUI(void);
		~WFInhibitorGUI(void);

	protected:
		// The implement of AbstractGUIApplication
		virtual void createApplicationObject();
		virtual void prepareForClose();

		//
		// The implement of IGUIApplication
		//
		virtual void onInitGenericGUICompleted();
		virtual unsigned long getApplicationType();
		virtual void checkEntity(TA_Base_Core::IEntityData* guiEntity);
		virtual void checkCommandLine();
		virtual void entityChanged(const std::vector<std::string>& changes);
		virtual void serverIsDown();
		virtual void serverIsUp();

		//
		// The implement of IGUIAccess
		// 
		virtual void loadAllUserSettings();
		virtual void saveAllUserSettings();

	private:
		WFInhibitorGUI(const WFInhibitorGUI&);
		WFInhibitorGUI& operator=(const WFInhibitorGUI&);
	};
}

#endif //WF_Inhibitor_GUI_H