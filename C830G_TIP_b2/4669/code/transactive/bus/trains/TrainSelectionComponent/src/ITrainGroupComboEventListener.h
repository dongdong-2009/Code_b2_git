/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/4669/transactive/bus/trains/TrainSelectionComponent/src/ITrainGroupComboEventListener.h $
  * @author:   Adam Radics
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This is implemented by the TrainSelectorGUI to be notified of when
  * the selection or text in the train group combo changes.
  */


#if !defined(ITrainGroupComboEventListener_H)
#define ITrainGroupComboEventListener_H


namespace TA_IRS_Bus
{
    // forward declaration
    class TrainGroupComboBox;

    class ITrainGroupComboEventListener
    {

    public:


        /** 
          * selectionChangedGroupCombo
          *
          * Called by the train group combo when its selection
          * is changed.
          */
        virtual void selectionChangedGroupCombo() = 0;


        /** 
          * editChangedGroupCombo
          *
          * Called by the train group combo when its edit box
          * is changed.
          */
        virtual void editChangedGroupCombo() = 0;

    };

} // TA_IRS_Bus

#endif // !defined(ITrainGroupComboEventListener_H)
