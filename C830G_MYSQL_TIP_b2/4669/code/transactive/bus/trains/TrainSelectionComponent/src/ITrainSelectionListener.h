/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/4669/transactive/bus/trains/TrainSelectionComponent/src/ITrainSelectionListener.h $
  * @author:   Adam Radics
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Implemented by the GUI to be notified when train selection changes.
  * This can be useful for enabling/disabling buttons depending on whether
  * a train or trains are selected.
  */


#if !defined(ITrainSelectionListener_H)
#define ITrainSelectionListener_H


namespace TA_IRS_Bus
{
    class ITrainSelectionListener
    {

    public:

        
        /** 
          * trainSelectionChanged
          *
          * Gets called when the train selection is changed.
          *
          * @param trainsAreSelected   true if at least one train is selected
          */
        virtual void trainSelectionChanged(bool trainsAreSelected) = 0;

    };

} // TA_IRS_Bus

#endif // !defined(ITrainSelectionListener_H)
