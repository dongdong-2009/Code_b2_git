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
#if !defined(DataCallEndWorkItem_H)
#define DataCallEndWorkItem_H

#include "bus/trains/TrainCommonLibrary/src/CommonTypes.h"
#include "bus/trains/TrainCommonLibrary/src/StateUpdateTypes.h"

#include "core/threads/src/IWorkItem.h"

// forward declarations
namespace TA_IRS_App
{
    class IDataCallEndObserver;
}

namespace TA_IRS_App
{

    /**
     * Work item used to notify observers of a call end event
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:47:30 PM
     */
    class DataCallEndWorkItem : public TA_Base_Core::IWorkItem
    {

    public:

        /**
         *
         * @param observer    The observer to notify
         * @param trainId    The train id
         * @param success    The success/failure status
         * @param details    Any additional details
         */
        DataCallEndWorkItem( IDataCallEndObserver& observer,
                             TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                             bool success,
                             const std::string& details );


        /**
         * Destructor
         */
        virtual ~DataCallEndWorkItem();


        /**
         * Executes the work item
         */
        virtual void executeWorkItem();


    private:


        /**
         * Private default constructor
         */
        DataCallEndWorkItem();


        /**
         * The observer to call
         */
        IDataCallEndObserver& m_observer;


        /**
         * The train id to pass to the observer
         */
        TA_IRS_Bus::CommonTypes::TrainIdType m_trainId;


        /**
         * The success to pass to the observer
         */
        bool m_success;


        /**
         * The details to pass to the observer
         */
        std::string m_details;

    };

} // TA_IRS_App

#endif // !defined(DataCallEndWorkItem_H)
