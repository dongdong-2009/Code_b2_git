/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3002_TIP/3002/transactive/app/cctv/video_display_manager/src/VideoDisplayManagerGUI.h $
  * @author:  Robert Young
  * @version: $Revision: #5 $
  *
  * Last modification: $DateTime: 2010/06/22 21:59:26 $
  * Last modified by:  $Author: haiyang.li $
  *
  * TODO: <description>
  */

// VideoDisplayManagerGUI.h: interface for the VideoDisplayManagerGUI class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_TRAIN_BORN_GUI_INCLUDED_)
#define _TRAIN_BORN_GUI_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bus/generic_gui/src/AbstractDialogGUI.h"


namespace TA_IRS_App
{
	class CTrainBorneManagerDlg;
	class TrainBorneTabManager;
    class TrainBorneGUI : public TA_Base_Bus::AbstractDialogGUI
    {

    public:
    /**
        * Constructor
        *
        * This must construct an instance of the dialog and store it in m_application. When
        * constructing the dialog this class passes in an instance of itself.
        */
        TrainBorneGUI();

        /**
        * Destructor
        *
        * This should delete the object stored at m_application
        */
        ~TrainBorneGUI();

		void checkEntity(TA_Base_Core::IEntityData* guiEntity) ;

		void checkCommandLine() ;

		void entityChanged(const std::vector<std::string>& changes);
		
		void serverIsDown();
		
		void serverIsUp();

		void loadAllUserSettings();

		void saveAllUserSettings();

		void prepareForClose() ;
		
		unsigned long getApplicationType();

		virtual void createApplicationObject() ;
        virtual void onInitGenericGUICompleted();
	private:
		CTrainBorneManagerDlg* m_pTrainBorneDlg;
		TrainBorneTabManager* m_trainBorneTabManager;
    };

} // TA_IRS_App

#endif // !defined(VIDEODISPLAYMANAGERGUI_H__52855386_6960_11D7_B175_0050BAB094A9__INCLUDED_)
