/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  adamr
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */
#if !defined(CallEndWorkItem_H)
#define CallEndWorkItem_H

#include "bus/trains/TrainCommonLibrary/src/CommonTypes.h"
#include "bus/trains/TrainCommonLibrary/src/StateUpdateTypes.h"

#include "core/threads/src/IWorkItem.h"

// forward declarations
namespace TA_IRS_App
{
    class ICallEndObserver;
}

namespace TA_IRS_App
{

    /**
     * Work item used to notify observers of a call end event
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:47:19 PM
     */
    class CallEndWorkItem : public TA_Base_Core::IWorkItem
    {

    public:

        /**
         * Constructor
         *
         * @param observer    The observer to notify
         * @param callDetails    The details of the call that ended
         * @param trainList    The trains involved in the radio call
         */
        CallEndWorkItem( ICallEndObserver& observer,
                         const TA_IRS_Bus::StateUpdateTypes::RadioCallDetails& callDetails,
                         const TA_IRS_Bus::CommonTypes::TrainIdList& trainList );


        /**
         * Destructor
         */
        virtual ~CallEndWorkItem();


        /**
         * Executes the work item
         */
        virtual void executeWorkItem();


    private:

        /**
         * Private default constructor
         */
        CallEndWorkItem();


        /**
         * The observer to call
         */
        ICallEndObserver& m_observer;


        /**
         * The call details to pass to the observer
         */
        TA_IRS_Bus::StateUpdateTypes::RadioCallDetails m_callDetails;


        /**
         * The train list to pass to the observer
         */
        TA_IRS_Bus::CommonTypes::TrainIdList m_trainList;

    };

} // TA_IRS_App

#endif // !defined(CallEndWorkItem_H)
