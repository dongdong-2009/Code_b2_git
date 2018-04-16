/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  Adam Radics
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */
#if !defined(TrainEventWorkItem_H)
#define TrainEventWorkItem_H


#include "bus/trains/TrainTransactionModel/src/TrainEvent.h"
#include "core/threads/src/IWorkItem.h"


// forward declarations
namespace TA_IRS_Bus
{
    class ITrainEventProcessor;
}


namespace TA_IRS_Bus
{
    /**
     * @author Adam Radics
     * @version 1.0
     * @created 22-Oct-2007 12:19:50 PM
     */

    class TrainEventWorkItem : public TA_Base_Core::IWorkItem
    {

    public:


        /**
         *
         * @param eventProcessor
         * @param event
         */
        TrainEventWorkItem( ITrainEventProcessor& eventProcessor, TrainEventPtr event );


        /**
         * @Destructor
         */
        virtual ~TrainEventWorkItem();


        /**
         * @Execute the Item
         */
        virtual void executeWorkItem();

    private:


        /** 
          * Private default constructor
          */
        TrainEventWorkItem();


        /**
         * The event processor to callback
         */
        ITrainEventProcessor& m_eventProcessor;


        /**
         * The event that occurred
         */
        TrainEventPtr m_event;

    };

}

#endif // !defined(TrainEventWorkItem_H)
