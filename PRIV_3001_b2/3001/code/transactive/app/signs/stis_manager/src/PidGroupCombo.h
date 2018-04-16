/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/signs/stis_manager/src/PidGroupCombo.h $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * A custom combo for PID groups.
  */

#if !defined(PidGroupCombo_H)
#define PidGroupCombo_H

#include "bus/mfc_extensions/src/coloured_controls/ColourCombo.h"

#include <string>
#include <vector>

namespace TA_IRS_App
{
    class PidSelectionManager;
}

namespace TA_IRS_App
{
    class PidGroupCombo : public TA_Base_Bus::ColourCombo
    {
    public:

        static const std::string HMIGroupName;

        PidGroupCombo();
        virtual ~PidGroupCombo();

        /** 
          * setPidSelectionManager
          *
          * Sets the object to tell when selection changes
          *
          * @param pidSelectionManager
          */
        void setPidSelectionManager(PidSelectionManager* pidSelectionManager);


        /** 
          * updateGroupNames
          *
          * Updates the contents of this combo box with the
          * new vector of group names.
          *
          * @param groupNames   The new list of names.
          */
        void updateGroupNames(const std::vector<std::string>& groupNames);


        /** 
          * getCurrentGroupName
          *
          * Gets the group name that is currently selected in the list
          *
          * @return The string that is in the combo box.
          */
        std::string getCurrentGroupName();


        /** 
          * selectGroupName
          *
          * Sets the given group name  to be the selected group.
          *
          */
        void selectGroupName(std::string groupName);


        /** 
          * groupNameIsAnExistingGroup
          *
          * Checks whether the given text is an existing group.
          * 
          * This can be used for confirmation dialogs etc.
          *
          * @return Whether the text represents an existing group name.
          */
        bool groupNameIsAnExistingGroup(std::string groupName);


    public:

        // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(PidGroupCombo)
	    virtual BOOL onSelectionChange();
        protected:
	    virtual void PreSubclassWindow();
	    //}}AFX_VIRTUAL


    protected:
	    //{{AFX_MSG(PidGroupCombo)
		    // NOTE - the ClassWizard will add and remove member functions here.
	    //}}AFX_MSG

	    DECLARE_MESSAGE_MAP()

    private:

        // passes events on
        PidSelectionManager* m_pidSelectionManager;

    };


} // TA_IRS_App

#endif // !defined(PidGroupCombo_H)
