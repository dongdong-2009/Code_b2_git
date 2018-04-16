/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  oliverk
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */

#if !defined(TrainStaticGroupComboBox_H)
#define TrainStaticGroupComboBox_H

#include <string>
#include <vector>
#include "bus/trains/TrainSelectionComponent/src/ITrainStaticGroupComboEventListener.h"


namespace TA_IRS_Bus
{
    /**
     * Contains the TrainRadioStaticGroups. Must also contain an item that selects no group at all.
     * @author oliverk
     * @version 1.0
     * @created 18-Mar-2008 5:46:31 PM
     */

    class TrainStaticGroupComboBox : public CComboBox
    {

    public:

        TrainStaticGroupComboBox();
        virtual ~TrainStaticGroupComboBox();


        /**
         * Updates the contents of this combo box with the new vector of group names.
         * 
         * @param trainListNames    The new list of names.
         */
        void updateGroupNames(const std::vector<std::string>& trainListNames);


        /**
         * Gets the group name that is currently selected in the list, or if there isnt one in the list an empty string will be returned.
         * 
         * @return The string that is in the combo box.
         */
        std::string getCurrentGroupName();


        /**
         * Sets the object that receives and handles some events
         * 
         * @param eventListener
         */
        void setEventListener(ITrainStaticGroupComboEventListener* eventListener);


	public:

		//Overrides
		//ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(TrainStaticGroupComboBox)
        afx_msg BOOL onEditChange();
        afx_msg BOOL onSelectionChange();
		//}}AFX_VIRTUAL


	protected:
		//{{AFX_MSG(TrainStaticGroupComboBox)
			// NOTE - the ClassWizard will add and remove member functions here.
		//}}AFX_MSG
		
	    DECLARE_MESSAGE_MAP()

    private:

		// passes events on
        ITrainStaticGroupComboEventListener* m_eventListener;

    };

}

#endif // !defined(TrainStaticGroupComboBox_H)
