/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/bus/trains/TrainSelectionComponent/src/TrainGroupComboBox.h $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * A Combo box that accepts updateSelectionCombo messages.
  * In the resource editor it must be a dropdown box, not a drop list box
  * to allow typing of new group names.
  */

#if !defined(TrainGroupComboBox_H)
#define TrainGroupComboBox_H

#include "bus/trains/TrainSelectionComponent/src/ITrainGroupComboEventListener.h"
#include <string>
#include <vector>


namespace TA_IRS_Bus
{
    class TrainGroupComboBox : public CComboBox
    {
    public:

        TrainGroupComboBox();
        virtual ~TrainGroupComboBox();


        /** 
          * updateGroupNames
          *
          * Updates the contents of this combo box with the
          * new vector of group names.
          *
          * @param trainListNames   The new list of names.
          */
        void updateGroupNames(const std::vector<std::string>& trainListNames);


        /** 
          * getCurrentGroupName
          *
          * Gets the group name that is currently selected in the list,
          * or if there isnt one in the list an empty string will be returned.
          *
          * @return The string that is in the combo box.
          */
        std::string getCurrentGroupName();


        /** 
          * currentGroupNameIsAnExistingGroup
          *
          * Checks whether the text in the combo is a current
          * group. Ie if save was pressed now, would an existing train
          * selection group be overwritten.
          * 
          * This can be used for confirmation dialogs etc.
          *
          * @return Whether the current text represents an existing group name.
          */
        bool currentGroupNameIsAnExistingGroup();


    public:
        /* The following are called by the TrainSelectorGUI object */

        /** 
          * setEventListener
          *
          * Sets the object that receives and handles some events
          *
          * @param eventListener
          */
        void setEventListener(ITrainGroupComboEventListener* eventListener);


    public:

        // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(TrainGroupComboBox)
        afx_msg BOOL onEditchange();
	    afx_msg BOOL onSelectionChange();
	    //}}AFX_VIRTUAL


    protected:
	    //{{AFX_MSG(TrainGroupComboBox)
		    // NOTE - the ClassWizard will add and remove member functions here.
	    //}}AFX_MSG

	    DECLARE_MESSAGE_MAP()

    private:

        // passes events on
        ITrainGroupComboEventListener* m_eventListener;

    };


} // TA_IRS_Bus

#endif // !defined(TrainGroupComboBox_H)
