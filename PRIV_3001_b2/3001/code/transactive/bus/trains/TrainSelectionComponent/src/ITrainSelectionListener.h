/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/PRIV_3001/3001/transactive/bus/trains/TrainSelectionComponent/src/ITrainSelectionListener.h $
  * @author:   Adam Radics
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
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
